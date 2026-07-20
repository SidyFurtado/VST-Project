#include "InferenceCore.h"

InferenceCore::InferenceCore()
    : env (ORT_LOGGING_LEVEL_WARNING, "VoidInference")
{
    // Clear pre-allocated arrays
    inputFrame.fill (0.0f);
    outputFrame.fill (0.0f);
}

InferenceCore::~InferenceCore()
{
    unloadModel();
}

bool InferenceCore::loadModel (const juce::String& modelPath)
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
        
        // Query input and output names/shapes using C++ API
        Ort::AllocatorWithDefaultOptions allocator;
        
        // Check input count
        size_t numInputNodes = session->GetInputCount();
        if (numInputNodes == 0)
        {
            unloadModel();
            return false;
        }
        
        // Get input name and shape
        auto inputNameAllocated = session->GetInputNameAllocated (0, allocator);
        inputName = inputNameAllocated.get();
        
        Ort::TypeInfo inputTypeInfo = session->GetInputTypeInfo (0);
        auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
        inputShape = inputTensorInfo.GetShape();
        
        // Re-evaluate dynamic shapes to match standard frames
        for (auto& dim : inputShape)
        {
            if (dim <= 0)
                dim = (dim == -1) ? MODEL_FRAME_SIZE : 1;
        }
        if (inputShape.empty())
        {
            inputShape = { 1, 1, MODEL_FRAME_SIZE };
        }
        
        // Check output count
        size_t numOutputNodes = session->GetOutputCount();
        if (numOutputNodes == 0)
        {
            unloadModel();
            return false;
        }
        
        // Get output name and shape
        auto outputNameAllocated = session->GetOutputNameAllocated (0, allocator);
        outputName = outputNameAllocated.get();
        
        Ort::TypeInfo outputTypeInfo = session->GetOutputTypeInfo (0);
        auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
        outputShape = outputTensorInfo.GetShape();
        
        for (auto& dim : outputShape)
        {
            if (dim <= 0)
                dim = (dim == -1) ? MODEL_FRAME_SIZE : 1;
        }
        if (outputShape.empty())
        {
            outputShape = { 1, 1, MODEL_FRAME_SIZE };
        }
        
        loadedModelPath = modelPath;
        return true;
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog ("ONNX Model Load Error: " + juce::String (e.what()));
        unloadModel();
        return false;
    }
}

void InferenceCore::unloadModel()
{
    juce::ScopedLock sl (sessionLock);
    session.reset();
    inputName.clear();
    outputName.clear();
    inputShape.clear();
    outputShape.clear();
    loadedModelPath.clear();
}

bool InferenceCore::isModelLoaded() const noexcept
{
    return session != nullptr;
}

bool InferenceCore::process()
{
    juce::ScopedLock sl (sessionLock);
    
    if (session == nullptr)
        return false;
        
    try
    {
        // memoryInfo describes where the tensors reside (CPU, Arena Allocator)
        auto memoryInfo = Ort::MemoryInfo::CreateCpu (OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
        
        // Wrap our pre-allocated std::array backing stores in ONNX Tensors
        // NOTE: CreateTensor references the arrays, no memory allocation occurs
        Ort::Value inputTensor = Ort::Value::CreateTensor<float> (
            memoryInfo,
            inputFrame.data(),
            inputFrame.size(),
            inputShape.data(),
            inputShape.size()
        );
        
        Ort::Value outputTensor = Ort::Value::CreateTensor<float> (
            memoryInfo,
            outputFrame.data(),
            outputFrame.size(),
            outputShape.data(),
            outputShape.size()
        );
        
        const char* inputNames[]  = { inputName.c_str() };
        const char* outputNames[] = { outputName.c_str() };
        
        // Execute the neural network in-place
        session->Run (
            Ort::RunOptions{nullptr},
            inputNames,
            &inputTensor,
            1,
            outputNames,
            &outputTensor,
            1
        );
        
        return true;
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog ("ONNX Process Error: " + juce::String (e.what()));
        return false;
    }
}
