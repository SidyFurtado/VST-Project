#include "InferenceCore.h"
#include <cmath>
#include <algorithm>
#include <numeric>

// ==============================================================================
// Helpers
// ==============================================================================

static bool nameContainsAny (const std::string& name, std::initializer_list<const char*> keywords)
{
    for (auto* kw : keywords)
        if (name.find (kw) != std::string::npos)
            return true;
    return false;
}

static const char* tensorTypeToString (ONNXTensorElementDataType type)
{
    switch (type)
    {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:  return "Float32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:  return "UInt8";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:   return "Int8";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16: return "UInt16";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:  return "Int16";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:  return "Int32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:  return "Int64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE: return "Float64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING: return "String";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:   return "Bool";
        default: return "Unknown";
    }
}

static juce::String shapeToString (const std::vector<int64_t>& shape)
{
    juce::StringArray parts;
    for (auto dim : shape)
        parts.add (juce::String (dim));
    return "[" + parts.joinIntoString (", ") + "]";
}

// ==============================================================================
// Construction / Destruction
// ==============================================================================

InferenceCore::InferenceCore()
    : env (ORT_LOGGING_LEVEL_WARNING, "VoidInference")
{
    resetCrossfade();
    resampleInStage.fill (0.0f);
    resampleOutStage.fill (0.0f);
}

InferenceCore::~InferenceCore()
{
    unloadModel();
}

// ==============================================================================
// RNN State Node Detection
// ==============================================================================

void InferenceCore::detectRnnStateNodes (Ort::AllocatorWithDefaultOptions& allocator)
{
    stateNodes.clear();
    stateInputNames.clear();
    stateOutputNames.clear();

    const size_t numInputs  = session->GetInputCount();
    const size_t numOutputs = session->GetOutputCount();

    // Gather all input names
    std::vector<std::string> inputNames, outputNames;
    for (size_t i = 0; i < numInputs; ++i)
    {
        auto n = session->GetInputNameAllocated (i, allocator);
        inputNames.emplace_back (n.get());
    }
    for (size_t i = 0; i < numOutputs; ++i)
    {
        auto n = session->GetOutputNameAllocated (i, allocator);
        outputNames.emplace_back (n.get());
    }

    // A state node is identified by common RNN hidden-state naming conventions:
    // e.g. h_in/h_out, c_in/c_out, hidden_in/hidden_out, state_in/state_out
    static const std::vector<std::pair<std::string, std::string>> suffixPairs = {
        {"_in",  "_out"},
        {"_0",   "_out_"},
        {"h0",   "hn"},
        {"c0",   "cn"},
    };

    for (size_t i = 1; i < numInputs; ++i)   // skip index 0 (primary audio input)
    {
        const auto& inName = inputNames[i];
        if (!nameContainsAny (inName, {"h", "c", "state", "hidden", "cell", "rnn", "gru", "lstm"}))
            continue;

        // Try to find a matching output
        std::string matchedOutName;
        for (size_t j = 1; j < numOutputs; ++j)   // skip index 0 (primary audio output)
        {
            const auto& outName = outputNames[j];
            // Simple heuristic: replace _in with _out or common variants
            auto candidate = inName;
            auto pos = candidate.rfind ("_in");
            if (pos != std::string::npos)
            {
                candidate.replace (pos, 3, "_out");
                if (candidate == outName)
                {
                    matchedOutName = outName;
                    break;
                }
            }
            // Also try h0->hn, c0->cn
            if ((inName == "h0" && outName == "hn") || (inName == "c0" && outName == "cn"))
            {
                matchedOutName = outName;
                break;
            }
            // General: same root, different suffix
            if (outName.find (inName.substr (0, inName.size() / 2)) != std::string::npos)
            {
                matchedOutName = outName;
                break;
            }
        }

        // Resolve shape for this state input
        Ort::TypeInfo ti = session->GetInputTypeInfo (i);
        auto tsi  = ti.GetTensorTypeAndShapeInfo();
        auto shape = tsi.GetShape();
        for (auto& d : shape)
            if (d <= 0) d = 1;

        int64_t totalElems = 1;
        for (auto d : shape) totalElems *= d;

        StateNode node;
        node.inputName  = inName;
        node.outputName = matchedOutName.empty() ? inName : matchedOutName;
        node.shape      = shape;
        node.data.assign (static_cast<size_t> (totalElems), 0.0f);   // initialise to zeros

        stateNodes.push_back (std::move (node));
        stateInputNames.push_back  (inName);
        stateOutputNames.push_back (matchedOutName.empty() ? inName : matchedOutName);
    }

    if (!stateNodes.empty())
        juce::Logger::writeToLog ("[VOID] RNN state nodes detected: " + juce::String ((int)stateNodes.size()));
    else
        juce::Logger::writeToLog ("[VOID] No RNN state nodes detected (stateless model).");
}

// ==============================================================================
// Model Loading
// ==============================================================================

bool InferenceCore::loadModel (const juce::String& modelPath, double hostRate)
{
    juce::ScopedLock sl (sessionLock);
    
    try
    {
        unloadModel();
        
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads (1);
        sessionOptions.SetGraphOptimizationLevel (GraphOptimizationLevel::ORT_ENABLE_ALL);
        
        #if JUCE_WINDOWS
        session = std::make_unique<Ort::Session> (env, modelPath.toWideCharPointer(), sessionOptions);
        #else
        session = std::make_unique<Ort::Session> (env, modelPath.toRawUTF8(), sessionOptions);
        #endif
        
        Ort::AllocatorWithDefaultOptions allocator;
        
        // Check input/output count (must have at least 1 of each)
        size_t numInputNodes  = session->GetInputCount();
        size_t numOutputNodes = session->GetOutputCount();
        if (numInputNodes == 0 || numOutputNodes == 0) { unloadModel(); return false; }
        
        // Get primary input name and shape
        auto inNameAlloc = session->GetInputNameAllocated (0, allocator);
        inputName = inNameAlloc.get();
        
        Ort::TypeInfo inputTypeInfo = session->GetInputTypeInfo (0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        inputShape = inputTensorInfo.GetShape();
        for (auto& dim : inputShape)
            if (dim <= 0) dim = (dim == -1) ? MODEL_FRAME_SIZE : 1;
        if (inputShape.empty())
            inputShape = { 1, 1, MODEL_FRAME_SIZE };
        
        // Get primary output name and shape
        auto outNameAlloc = session->GetOutputNameAllocated (0, allocator);
        outputName = outNameAlloc.get();
        
        Ort::TypeInfo outputTypeInfo = session->GetOutputTypeInfo (0);
        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
        outputShape = outputTensorInfo.GetShape();
        for (auto& dim : outputShape)
            if (dim <= 0) dim = (dim == -1) ? MODEL_FRAME_SIZE : 1;
        if (outputShape.empty())
            outputShape = { 1, 1, MODEL_FRAME_SIZE };
        
        // Detect RNN state nodes (inputs after index 0)
        detectRnnStateNodes (allocator);

        loadedModelPath = modelPath;

        // Reset crossfade history
        resetCrossfade();

        // Set up resampling
        resetResamplers (hostRate);
        
        // Log ONNX Model topology details
        juce::String logMsg;
        logMsg << "\n========================================================\n"
               << "[VOID] ONNX Model Loaded Successfully!\n"
               << "Path: " << modelPath << "\n"
               << "--------------------------------------------------------\n"
               << "INPUTS (" << juce::String (numInputNodes) << "):\n";
               
        for (size_t i = 0; i < numInputNodes; ++i)
        {
            auto nodeNameAlloc = session->GetInputNameAllocated (i, allocator);
            std::string name = nodeNameAlloc.get();
            Ort::TypeInfo typeInfo = session->GetInputTypeInfo (i);
            auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
            auto type = tensorInfo.GetElementType();
            auto shape = tensorInfo.GetShape();
            
            logMsg << "  [" << juce::String (i) << "] Name: \"" << juce::String (name)
                   << "\", Type: " << tensorTypeToString (type)
                   << ", Shape: " << shapeToString (shape) << "\n";
        }
        
        logMsg << "--------------------------------------------------------\n"
               << "OUTPUTS (" << juce::String (numOutputNodes) << "):\n";
               
        for (size_t i = 0; i < numOutputNodes; ++i)
        {
            auto nodeNameAlloc = session->GetOutputNameAllocated (i, allocator);
            std::string name = nodeNameAlloc.get();
            Ort::TypeInfo typeInfo = session->GetOutputTypeInfo (i);
            auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
            auto type = tensorInfo.GetElementType();
            auto shape = tensorInfo.GetShape();
            
            logMsg << "  [" << juce::String (i) << "] Name: \"" << juce::String (name)
                   << "\", Type: " << tensorTypeToString (type)
                   << ", Shape: " << shapeToString (shape) << "\n";
        }
        
        // Check for hidden states or memory-related inputs
        bool requiresStates = false;
        juce::StringArray hiddenStateNames;
        for (size_t i = 1; i < numInputNodes; ++i)
        {
            auto nodeNameAlloc = session->GetInputNameAllocated (i, allocator);
            std::string name = nodeNameAlloc.get();
            if (nameContainsAny (name, {"h", "c", "state", "hidden", "cell", "rnn", "gru", "lstm", "vad", "att"}))
            {
                requiresStates = true;
                hiddenStateNames.add (name);
            }
        }
        
        logMsg << "--------------------------------------------------------\n";
        if (requiresStates)
        {
            logMsg << "[VOID] Hidden States/Memory detected: YES (Count: "
                   << juce::String (hiddenStateNames.size()) << ")\n"
                   << "  State names: " << hiddenStateNames.joinIntoString (", ") << "\n";
        }
        else
        {
            logMsg << "[VOID] Hidden States/Memory detected: NO\n";
        }
        logMsg << "========================================================\n";

        // Print to standard log
        juce::Logger::writeToLog (logMsg);

        return true;
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog ("ONNX Model Load Error: " + juce::String (e.what()));
        unloadModel();
        return false;
    }
}

void InferenceCore::resetCrossfade()
{
    prevOutputTail.fill (0.0f);
    firstFrame = true;
    hasProcessedAnyFrames = false;
    currentReductionDb.store (0.0f);
    smoothedReductionDb = 0.0f;
}

void InferenceCore::unloadModel()
{
    // Always acquire the session lock to prevent data races with process()
    // which is called concurrently from the inference thread.
    juce::ScopedLock sl (sessionLock);

    session.reset();
    inputName.clear();
    outputName.clear();
    inputShape.clear();
    outputShape.clear();
    loadedModelPath.clear();
    stateNodes.clear();
    stateInputNames.clear();
    stateOutputNames.clear();
    allInputTensors.clear();
    allOutputTensors.clear();
    allInputNamePtrs.clear();
    allOutputNamePtrs.clear();
    resetCrossfade();
}

bool InferenceCore::isModelLoaded() const noexcept
{
    return session != nullptr;
}

// ==============================================================================
// Resampling
// ==============================================================================

void InferenceCore::resetResamplers (double hostRate)
{
    hostSampleRate   = hostRate;
    resamplingEnabled = (std::abs (hostRate - MODEL_SAMPLE_RATE) > 1.0);

    inputResampler.reset();
    outputResampler.reset();

    if (resamplingEnabled)
        juce::Logger::writeToLog ("[VOID] Resampling enabled: " + juce::String (hostRate) + " Hz <-> 48000 Hz");
    else
        juce::Logger::writeToLog ("[VOID] Resampling disabled (host is already 48000 Hz)");
}

int InferenceCore::resampleToModelRate (const float* src, int numSrcSamples)
{
    if (!resamplingEnabled)
    {
        // Just copy directly
        const int n = std::min (numSrcSamples, MODEL_FRAME_SIZE);
        std::copy (src, src + n, inputFrame.begin());
        return n;
    }

    // speed ratio: how many host samples per model sample
    const double ratio = hostSampleRate / MODEL_SAMPLE_RATE;

    // CRITICAL FIX: clamp numOut so the resampler doesn't read past
    // the end of src (which has numSrcSamples elements).
    // The resampler reads ceil(numOut * ratio) input samples.
    // We limit: numOut <= floor(numSrcSamples / ratio)
    const int safeNumOut = static_cast<int> (std::floor ((double) numSrcSamples / ratio + 1e-9));
    const int clampedOut = std::min (MODEL_FRAME_SIZE, safeNumOut);

    const int produced = inputResampler.process (ratio, src, inputFrame.data(), clampedOut);
    return produced;
}

int InferenceCore::resampleToHostRate (float* dest, int maxDestSamples)
{
    if (!resamplingEnabled)
    {
        const int n = std::min (MODEL_FRAME_SIZE, maxDestSamples);
        std::copy (outputFrame.begin(), outputFrame.begin() + n, dest);
        return n;
    }

    // speed ratio: model samples per host sample
    const double ratio = MODEL_SAMPLE_RATE / hostSampleRate;

    // CRITICAL FIX: clamp maxDestSamples so the resampler doesn't read past
    // the end of outputFrame (which has MODEL_FRAME_SIZE elements).
    // At 96kHz (ratio=0.5): process(0.5, ... , 1920) would read 960 from outputFrame (OOB!).
    // We limit to: ceil(MODEL_FRAME_SIZE / ratio) with a small epsilon to guard
    // against floating-point imprecision (e.g., 480 / (48000/44100) might give
    // 441.0000001 instead of exactly 441, and ceil would wrongly give 442).
    const int safeMaxOut = static_cast<int> (std::ceil ((double) MODEL_FRAME_SIZE / ratio - 1e-9));
    const int clampedDest = std::min (maxDestSamples, safeMaxOut);

    const int produced = outputResampler.process (ratio, outputFrame.data(), dest, clampedDest);
    return produced;
}

// ==============================================================================
// Crossfade (Hann ramp at output frame boundaries)
// ==============================================================================

void InferenceCore::applyCrossfade()
{
    if (firstFrame)
    {
        // Nothing to blend with on first frame — just save the tail and return
        std::copy (outputFrame.begin() + (MODEL_FRAME_SIZE - CROSSFADE_LEN),
                   outputFrame.end(),
                   prevOutputTail.begin());
        firstFrame = false;
        return;
    }

    // --- Fade-in ramp at the START of outputFrame ---
    // Blend with prevOutputTail: new[i] = prevTail[i] * (1-w) + new[i] * w
    for (int i = 0; i < CROSSFADE_LEN; ++i)
    {
        const float w = 0.5f * (1.0f - std::cos (juce::MathConstants<float>::pi * (float)i / (float)CROSSFADE_LEN));
        outputFrame[i] = prevOutputTail[i] * (1.0f - w) + outputFrame[i] * w;
    }

    // Save new tail for next frame
    std::copy (outputFrame.begin() + (MODEL_FRAME_SIZE - CROSSFADE_LEN),
               outputFrame.end(),
               prevOutputTail.begin());
}

// ==============================================================================
// Reduction Metering
// ==============================================================================

void InferenceCore::updateReductionMeter()
{
    // Compute RMS of input frame
    float inSumSq = 0.0f;
    for (int i = 0; i < MODEL_FRAME_SIZE; ++i)
        inSumSq += inputFrame[i] * inputFrame[i];
    inputFrameRms = std::sqrt (inSumSq / MODEL_FRAME_SIZE);

    // Compute RMS of output frame
    float outSumSq = 0.0f;
    for (int i = 0; i < MODEL_FRAME_SIZE; ++i)
        outSumSq += outputFrame[i] * outputFrame[i];
    outputFrameRms = std::sqrt (outSumSq / MODEL_FRAME_SIZE);

    // Compute reduction in dB
    float inDb  = 20.0f * std::log10 (std::max (inputFrameRms, 1e-8f));
    float outDb = 20.0f * std::log10 (std::max (outputFrameRms, 1e-8f));
    float reduction = inDb - outDb;

    // Smooth for metering
    const float meterSmooth = 0.3f;
    smoothedReductionDb = (1.0f - meterSmooth) * smoothedReductionDb + meterSmooth * std::max (0.0f, reduction);
    currentReductionDb.store (smoothedReductionDb);
}

// ==============================================================================
// Inference
// ==============================================================================

bool InferenceCore::process()
{
    juce::ScopedLock sl (sessionLock);
    
    if (session == nullptr)
    {
        outputFrame = inputFrame;
        applyCrossfade();
        return true;
    }

    if (useDummyPassthrough)
    {
        outputFrame = inputFrame;
        applyCrossfade();
        updateReductionMeter();
        hasProcessedAnyFrames = true;
        return true;
    }
        
    try
    {
        auto memoryInfo = Ort::MemoryInfo::CreateCpu (OrtAllocatorType::OrtArenaAllocator,
                                                       OrtMemType::OrtMemTypeDefault);

        // --- Build input tensor list ---
        // Slot 0: primary audio input
        allInputTensors.clear();
        allInputNamePtrs.clear();

        allInputTensors.push_back (
            Ort::Value::CreateTensor<float> (
                memoryInfo,
                inputFrame.data(), inputFrame.size(),
                inputShape.data(), inputShape.size()
            )
        );
        allInputNamePtrs.push_back (inputName.c_str());

        // State input tensors (persistent across frames)
        for (auto& node : stateNodes)
        {
            allInputTensors.push_back (
                Ort::Value::CreateTensor<float> (
                    memoryInfo,
                    node.data.data(), node.data.size(),
                    node.shape.data(), node.shape.size()
                )
            );
            allInputNamePtrs.push_back (node.inputName.c_str());
        }

        // --- Build output tensor list ---
        allOutputTensors.clear();
        allOutputNamePtrs.clear();

        allOutputTensors.push_back (
            Ort::Value::CreateTensor<float> (
                memoryInfo,
                outputFrame.data(), outputFrame.size(),
                outputShape.data(), outputShape.size()
            )
        );
        allOutputNamePtrs.push_back (outputName.c_str());

        // State output tensors (write back in-place into node.data)
        for (auto& node : stateNodes)
        {
            allOutputTensors.push_back (
                Ort::Value::CreateTensor<float> (
                    memoryInfo,
                    node.data.data(), node.data.size(),
                    node.shape.data(), node.shape.size()
                )
            );
            allOutputNamePtrs.push_back (node.outputName.c_str());
        }

        // --- Execute the neural network ---
        session->Run (
            Ort::RunOptions { nullptr },
            allInputNamePtrs.data(),
            allInputTensors.data(),
            allInputTensors.size(),
            allOutputNamePtrs.data(),
            allOutputTensors.data(),
            allOutputTensors.size()
        );

        // State tensors in stateNodes.data are now updated in-place for the next frame.

        // Apply crossfade at the output boundary
        applyCrossfade();

        // Update reduction metering
        updateReductionMeter();
        hasProcessedAnyFrames = true;
        
        return true;
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog ("ONNX Process Error: " + juce::String (e.what()));
        return false;
    }
}
