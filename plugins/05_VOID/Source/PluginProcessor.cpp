#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// ==============================================================================
// VoidAudioProcessor — Implementation
// ==============================================================================

VoidAudioProcessor::VoidAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "VOID_PARAMS", createParameterLayout()),
      inferenceThread (inferenceCore, inputFifo, outputFifo, signalEvent)
{
    // AI parameters
    vacuumIntensityParam = apvts.getRawParameterValue (VoidParams::vacuumIntensity());
    bypassParam          = apvts.getRawParameterValue (VoidParams::bypass());
    reductionModeParam   = apvts.getRawParameterValue (VoidParams::reductionMode());

    // DSP parameters
    noiseThresholdParam   = apvts.getRawParameterValue (VoidParams::noiseThreshold());
    noiseFloorParam       = apvts.getRawParameterValue (VoidParams::noiseFloor());
    reductionAmountParam  = apvts.getRawParameterValue (VoidParams::reductionAmount());
    attackTimeParam       = apvts.getRawParameterValue (VoidParams::attackTime());
    releaseTimeParam      = apvts.getRawParameterValue (VoidParams::releaseTime());
    spectralSmoothParam   = apvts.getRawParameterValue (VoidParams::spectralSmooth());
    learnNoiseParam       = apvts.getRawParameterValue (VoidParams::learnNoise());
}

VoidAudioProcessor::~VoidAudioProcessor()
{
}

// ==============================================================================
// Plugin metadata
// ==============================================================================

const juce::String VoidAudioProcessor::getName() const { return "VOID"; }

bool VoidAudioProcessor::acceptsMidi()  const { return false; }
bool VoidAudioProcessor::producesMidi() const { return false; }
bool VoidAudioProcessor::isMidiEffect() const { return false; }

double VoidAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int VoidAudioProcessor::getNumPrograms() { return 1; }
int VoidAudioProcessor::getCurrentProgram() { return 0; }
void VoidAudioProcessor::setCurrentProgram (int) {}
const juce::String VoidAudioProcessor::getProgramName (int) { return {}; }
void VoidAudioProcessor::changeProgramName (int, const juce::String&) {}

// ==============================================================================
// Audio lifecycle
// ==============================================================================

void VoidAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // ---- 0. Auto-restore model from persisted path (once per session) -----------
    if (!modelRestoreAttempted)
    {
        modelRestoreAttempted = true;
        juce::String savedModelPath = apvts.state.getProperty (VoidParams::modelPath(), juce::String()).toString();
        if (savedModelPath.isNotEmpty())
        {
            juce::File modelFile (savedModelPath);
            if (modelFile.existsAsFile())
            {
                juce::Logger::writeToLog ("[VOID] Auto-restoring model: " + savedModelPath);
                loadModel (savedModelPath);
            }
            else
            {
                juce::Logger::writeToLog ("[VOID] Saved model not found: " + savedModelPath);
            }
        }
    }

    // ---- 1. Reset AI crossfade state to prevent stale tail from prev session ---
    inferenceCore.resetCrossfade();

    // ---- 2. Prepare DSP noise reduction engine ---
    spectralReducer.prepare (sampleRate, 2048, 512);
    spectralReducer.startLearnProfile();  // Auto-learn noise profile on startup
    dspPrepared = true;

    // ---- 3. AI FIFOs ---
    const int windowSize = InferenceCore::MODEL_FRAME_SIZE;
    const int fifoCapacity = std::max (8192, windowSize * 16);

    inputFifo.prepare (1, fifoCapacity);
    outputFifo.prepare (1, fifoCapacity);
    dryFifo.prepare (1, fifoCapacity);

    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();

    // Pre-fill dry and output FIFOs with windowSize samples of silence for PDC alignment
    juce::AudioBuffer<float> silence (1, windowSize);
    silence.clear();
    dryFifo.write (silence, 0, windowSize);
    outputFifo.write (silence, 0, windowSize);

    // ---- 4. If a model is loaded, pre-fill the output FIFO with processed silence ---
    // This ensures the audio thread always has enough wet samples to read.
    if (inferenceCore.isModelLoaded())
        prefillOutputFifoWithSilentFrames (16);  // 16 * 480 = 7680 samples buffer

    // ---- 5. Pre-allocate ALL audio thread buffers (zero allocations in processBlock) -
    const int bufferSize = samplesPerBlock;

    monoInputBuffer.setSize  (1, bufferSize);
    monoOutputBuffer.setSize (1, bufferSize);
    monoDryBuffer.setSize    (1, bufferSize);
    dspProcessBuffer.setSize (1, bufferSize);
    processedBuffer.setSize  (1, bufferSize);
    aiBuffer.setSize         (1, bufferSize);
    pipelineDryBuffer.setSize (1, bufferSize);
    pipelineWetBuffer.setSize (1, bufferSize);

    monoInputBuffer.clear();
    monoOutputBuffer.clear();
    monoDryBuffer.clear();
    dspProcessBuffer.clear();
    processedBuffer.clear();
    aiBuffer.clear();
    pipelineDryBuffer.clear();
    pipelineWetBuffer.clear();

    // ---- 6. Resamplers for AI ---
    inferenceCore.resetResamplers (sampleRate);

    // ---- 7. Start inference thread ---
    inferenceThread.prepare();

    // ---- 8. Report PDC latency (only for AI mode) ---
    setLatencySamples (windowSize);

    // ---- 9. Reset metering ---
    inputLevelDb.store  (-90.0f);
    outputLevelDb.store (-90.0f);
}

// ==============================================================================
// Factory Presets
// ==============================================================================

const std::vector<Preset>& VoidAudioProcessor::getFactoryPresets()
{
    static const std::vector<Preset> presets =
    {
        { "Custom",              100.0f, 1, -40.0f, -70.0f, 70.0f, 10.0f, 200.0f, 3.0f },
        { "Podcast / Voiceover",  65.0f, 0, -30.0f, -75.0f, 60.0f,  5.0f, 100.0f, 3.0f },
        { "Extreme Noise",        95.0f, 2, -50.0f, -80.0f, 85.0f,  2.0f,  50.0f, 5.0f },
        { "Light Room Tone",      35.0f, 1, -20.0f, -60.0f, 40.0f, 15.0f, 300.0f, 1.0f },
        { "Hybrid Precision",     80.0f, 2, -40.0f, -70.0f, 70.0f,  8.0f, 150.0f, 3.0f },
    };
    return presets;
}

void VoidAudioProcessor::loadPreset (int presetIndex)
{
    const auto& presets = getFactoryPresets();
    if (presetIndex < 0 || presetIndex >= (int) presets.size())
        return;

    const Preset& p = presets[presetIndex];

    // Use beginChangeGesture / endChangeGesture for proper DAW automation recording
    auto setParam = [&] (const juce::String& paramID, float value)
    {
        auto* param = apvts.getParameter (paramID);
        if (param != nullptr)
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (param->convertTo0to1 (value));
            param->endChangeGesture();
        }
    };

    setParam (VoidParams::vacuumIntensity(), p.vacuumIntensity);
    setParam (VoidParams::reductionMode(),   (float) p.reductionMode);
    setParam (VoidParams::noiseThreshold(),  p.noiseThreshold);
    setParam (VoidParams::noiseFloor(),      p.noiseFloor);
    setParam (VoidParams::reductionAmount(), p.reductionAmount);
    setParam (VoidParams::attackTime(),      p.attackTime);
    setParam (VoidParams::releaseTime(),     p.releaseTime);
    setParam (VoidParams::spectralSmooth(),  p.spectralSmooth);

    juce::Logger::writeToLog ("[VOID] Preset loaded: " + p.name);
}

void VoidAudioProcessor::releaseResources()
{
    inferenceThread.stopThread (2000);
    inputFifo.reset();
    outputFifo.reset();
    dryFifo.reset();
    spectralReducer.reset();
    dspPrepared = false;
}

bool VoidAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

// ==============================================================================
// processBlock — Hybrid AI + DSP Noise Reduction Pipeline
// ==============================================================================

void VoidAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

   #if JUCE_DEBUG
    // Performance benchmark: measures actual processing time inside processBlock
    static int benchCounter = 0;
    static double benchAccum = 0.0;
    static double benchMax = 0.0;
    juce::uint64 blockStart = juce::Time::getHighResolutionTicks();
    #endif

    const bool bypassed = bypassParam->load() > 0.5f;
    {
        float sumSq = 0.0f;
        for (int ch = 0; ch < getTotalNumInputChannels(); ++ch)
        {
            auto* data = buffer.getReadPointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                sumSq += data[i] * data[i];
        }
        float rms = std::sqrt (sumSq / (std::max (1, buffer.getNumSamples()) * std::max (1, getTotalNumInputChannels())));
        float db = 20.0f * std::log10 (std::max (rms, 1e-8f));
        const float meterSmooth = 0.1f;
        float prev = inputLevelDb.load();
        inputLevelDb.store (prev + meterSmooth * (db - prev));
    }

    // ---- Full bypass: pass audio through untouched ---
    if (bypassed)
    {
        // If DSP is bypassed but we were learning noise, tell the reducer to not learn
        outputLevelDb.store (inputLevelDb.load());
        return;
    }

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    // Clear unused output channels
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    // ---- Read parameters ---
    const float alpha = juce::jlimit (0.0f, 1.0f, vacuumIntensityParam->load() / 100.0f);
    const int effectiveMode = getEffectiveReductionMode();

    // ---- Check for learn noise trigger ---
    if (learnNoiseParam->load() > 0.5f && !learnNoiseRequested)
    {
        learnNoiseRequested = true;
        spectralReducer.startLearnProfile();
        juce::Logger::writeToLog ("[VOID] Learning noise profile...");
    }
    else if (learnNoiseParam->load() < 0.5f && learnNoiseRequested)
    {
        learnNoiseRequested = false;
        spectralReducer.freezeProfile();
        juce::Logger::writeToLog ("[VOID] Noise profile frozen.");
    }

    // ---- Downmix to mono (optimized with SIMD-friendly ops) ---
    monoInputBuffer.clear();
    if (totalNumInputChannels > 0)
    {
        if (totalNumInputChannels == 1)
        {
            monoInputBuffer.copyFrom (0, 0, buffer.getReadPointer (0), numSamples);
        }
        else
        {
            // SIMD-optimized mono downmix via FloatVectorOperations
            auto* writePtr = monoInputBuffer.getWritePointer (0);
            auto* leftPtr  = buffer.getReadPointer (0);
            auto* rightPtr = buffer.getReadPointer (1);
            juce::FloatVectorOperations::add (writePtr, leftPtr, rightPtr, numSamples);
            juce::FloatVectorOperations::multiply (writePtr, 0.5f, numSamples);
        }
    }

    // ---- Use pre-allocated buffers (zero allocations in audio thread) ---
    processedBuffer.clear();

    // ---- HYBRID PIPELINE ---

    if (effectiveMode == static_cast<int> (ReductionMode::DSP))
    {
        // MODE 1: Pure DSP spectral subtraction (works without any model)
        auto* inputPtr  = monoInputBuffer.getReadPointer (0);
        auto* outputPtr = processedBuffer.getWritePointer (0);
        applyDSPNoiseReduction (inputPtr, outputPtr, numSamples);
    }
    else if (effectiveMode == static_cast<int> (ReductionMode::AI))
    {
        // MODE 2: Pure AI (ONNX inference)
        processAIPipeline (monoInputBuffer, processedBuffer, numSamples);
    }
    else // Hybrid
    {
        // MODE 3: AI + DSP enhancement (AI first, then DSP refines)
        aiBuffer.clear();

        processAIPipeline (monoInputBuffer, aiBuffer, numSamples);

        // DSP refines the AI output
        auto* aiPtr    = aiBuffer.getReadPointer (0);
        auto* outPtr   = processedBuffer.getWritePointer (0);
        applyDSPNoiseReduction (aiPtr, outPtr, numSamples);
    }

    auto* wetPtr = processedBuffer.getReadPointer (0);
    auto* dryPtr = monoInputBuffer.getReadPointer (0);

    // ---- Compute output level for metering ---
    {
        float sumSq = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            sumSq += wetPtr[i] * wetPtr[i];
        float rms = std::sqrt (sumSq / numSamples);
        float db = 20.0f * std::log10 (std::max (rms, 1e-8f));
        const float meterSmooth = 0.1f;
        float prev = outputLevelDb.load();
        outputLevelDb.store (prev + meterSmooth * (db - prev));
    }

    // ---- Upmix to stereo with dry/wet mix ---
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            // Pure linear crossfade: 0% = dry, 100% = processed
            float outVal = (1.0f - alpha) * dryPtr[i] + alpha * wetPtr[i];
            channelData[i] = juce::jlimit (-1.0f, 1.0f, outVal);
        }
    }

   #if JUCE_DEBUG
    // End benchmark: measure actual processing duration in microseconds
    {
        juce::uint64 blockEnd = juce::Time::getHighResolutionTicks();
        double elapsedMs = juce::Time::highResolutionTicksToSeconds (blockEnd - blockStart) * 1000.0;
        benchAccum += elapsedMs;
        if (elapsedMs > benchMax) benchMax = elapsedMs;
        static double benchAllTimeMax = 0.0;
        if (elapsedMs > benchAllTimeMax) benchAllTimeMax = elapsedMs;
        ++benchCounter;
        if (benchCounter >= 500)  // Log every ~500 blocks
        {
            juce::Logger::writeToLog (juce::String::formatted (
                "[VOID BENCH] processBlock — avg: %.4f ms, max: %.4f ms, all-time peak: %.4f ms (%d blocks)",
                benchAccum / benchCounter, benchMax, benchAllTimeMax, benchCounter));
            benchAccum = 0.0;
            benchMax = 0.0;
            benchCounter = 0;
        }
    }
    #endif
}

// ==============================================================================
// AI Pipeline — Process through ONNX inference
// ==============================================================================

void VoidAudioProcessor::processAIPipeline (const juce::AudioBuffer<float>& input,
                                            juce::AudioBuffer<float>& output,
                                            int numSamples)
{
    auto* outputPtr = output.getWritePointer (0);

    // ---- Write to input FIFO for inference thread ----
    // Separated from dryFifo write to avoid shared-failure bug
    if (inputFifo.getFreeSpace() >= numSamples)
    {
        inputFifo.write (input, 0, numSamples);
    }

    // ---- Write to dry FIFO for phase-aligned fallback ----
    if (dryFifo.getFreeSpace() >= numSamples)
    {
        dryFifo.write (input, 0, numSamples);
    }

    // Signal inference thread
    signalEvent.signal();

    // ---- Drain dry FIFO using pre-allocated buffer (no allocation in audio thread) --
    pipelineDryBuffer.clear();
    if (dryFifo.getNumReady() >= numSamples)
    {
        dryFifo.read (pipelineDryBuffer, 0, numSamples);
    }
    else if (dryFifo.getNumReady() > 0)
    {
        // Drain whatever is available
        float* drainPtr = pipelineDryBuffer.getWritePointer (0);
        dryFifo.read (&drainPtr, 1, dryFifo.getNumReady());
    }

    // ---- Read wet samples from output FIFO with sample-hold underrun protection ----
    const int wetAvailable = outputFifo.getNumReady();
    const int wetToRead = std::min (numSamples, wetAvailable);

    pipelineWetBuffer.clear();

    if (wetToRead > 0)
        outputFifo.read (pipelineWetBuffer, 0, wetToRead);

    // Sample-hold: use last available wet sample instead of falling back to dry
    auto* wetData = pipelineWetBuffer.getReadPointer (0);
    float lastWetSample = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        if (i < wetToRead)
        {
            lastWetSample = wetData[i];
            outputPtr[i] = lastWetSample;
        }
        else
        {
            // Sample-hold prevents periodic dry-signal bleed-through
            outputPtr[i] = lastWetSample;
        }
    }
}

// ==============================================================================
// DSP Pipeline — Spectral noise reduction (works without any AI model)
// ==============================================================================

void VoidAudioProcessor::applyDSPNoiseReduction (const float* input, float* output, int numSamples)
{
    if (!dspPrepared)
    {
        if (output != input)
            std::memcpy (output, input, numSamples * sizeof (float));
        return;
    }

    // Sync DSP parameters from APVTS
    updateDSPParameters();

    // Process through spectral reducer
    spectralReducer.processBlock (input, output, numSamples, learnNoiseRequested);

    // Reset learn flag after one block to prevent continuous learning
    if (learnNoiseRequested)
    {
        // The learn flag should persist for several blocks to get a good profile
        // It will be reset when learnNoiseParam goes back to 0
        learnNoiseRequested = (learnNoiseParam->load() > 0.5f);
    }
}

// ==============================================================================
// Helper: Pre-fill output FIFO with processed silence
// ==============================================================================

void VoidAudioProcessor::prefillOutputFifoWithSilentFrames (int numFrames)
{
    if (!inferenceCore.isModelLoaded())
        return;

    juce::Logger::writeToLog ("[VOID] Pre-filling output FIFO with " + juce::String (numFrames) + " silent frames...");

    for (int i = 0; i < numFrames; ++i)
    {
        inferenceCore.inputFrame.fill (0.0f);
        inferenceCore.process();  // Process silence through model

        if (outputFifo.getFreeSpace() >= InferenceCore::MODEL_FRAME_SIZE)
        {
            const float* outPtr = inferenceCore.outputFrame.data();
            outputFifo.write (&outPtr, 1, InferenceCore::MODEL_FRAME_SIZE);
        }
    }

    juce::Logger::writeToLog ("[VOID] Output FIFO pre-fill complete. Ready: "
                              + juce::String (outputFifo.getNumReady()) + " samples");
}

// ==============================================================================
// Helper: Effective reduction mode
// ==============================================================================

int VoidAudioProcessor::getEffectiveReductionMode() const noexcept
{
    int mode = static_cast<int> (reductionModeParam->load() + 0.5f);

    // If AI mode is selected but no model is loaded, fall back to DSP
    if (mode == static_cast<int> (ReductionMode::AI) && !inferenceCore.isModelLoaded())
        return static_cast<int> (ReductionMode::DSP);

    // If Hybrid mode is selected but no model is loaded, fall back to DSP
    if (mode == static_cast<int> (ReductionMode::Hybrid) && !inferenceCore.isModelLoaded())
        return static_cast<int> (ReductionMode::DSP);

    return mode;
}

// ==============================================================================
// Metering
// ==============================================================================

float VoidAudioProcessor::getReductionDb() const noexcept
{
    float inDb  = inputLevelDb.load();
    float outDb = outputLevelDb.load();
    return std::max (0.0f, inDb - outDb);
}

bool VoidAudioProcessor::isCurrentlyReducing() const noexcept
{
    return getReductionDb() > 0.5f;
}

// ==============================================================================
// DSP Parameter Sync
// ==============================================================================

void VoidAudioProcessor::updateDSPParameters()
{
    spectralReducer.setReductionDb (juce::jlimit (0.0f, 60.0f, noiseThresholdParam->load() * -1.0f));
    spectralReducer.setNoiseFloorDb (noiseFloorParam->load());
    spectralReducer.setSpectralSmoothing (static_cast<int> (spectralSmoothParam->load()));
    spectralReducer.setAttackMs (attackTimeParam->load());
    spectralReducer.setReleaseMs (releaseTimeParam->load());
    spectralReducer.setLearnRate (0.01f);
    // Map Amount (0-100) to oversubtraction factor (1.0 - 3.0)
    const float amountVal = reductionAmountParam->load() / 100.0f;
    const float overSub = 1.0f + amountVal * 2.0f;  // 1.0 at 0%, 3.0 at 100%
    spectralReducer.setOverSubtractionFactor (overSub);
}

// ==============================================================================
// State persistence
// ==============================================================================

void VoidAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VoidAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// ==============================================================================
// Model loading
// ==============================================================================

bool VoidAudioProcessor::loadModel (const juce::String& modelPath)
{
    bool success = inferenceCore.loadModel (modelPath, getSampleRate());

    if (success)
    {
        inferenceThread.prepare();
        inputFifo.reset();
        dryFifo.reset();
        outputFifo.reset();

        const int windowSize = InferenceCore::MODEL_FRAME_SIZE;
        juce::AudioBuffer<float> silence (1, windowSize);
        silence.clear();
        dryFifo.write (silence, 0, windowSize);
        outputFifo.write (silence, 0, windowSize);

        // Pre-fill output FIFO with processed silence
        prefillOutputFifoWithSilentFrames (16);

        setLatencySamples (windowSize);
        juce::Logger::writeToLog ("[VOID] Model loaded successfully: " + modelPath);

        // Persist model path in APVTS for session restore
        apvts.state.setProperty (VoidParams::modelPath(), modelPath, nullptr);
    }

    return success;
}

void VoidAudioProcessor::unloadModel()
{
    inferenceCore.unloadModel();

    inferenceThread.prepare();
    inputFifo.reset();
    dryFifo.reset();
    outputFifo.reset();

    const int windowSize = InferenceCore::MODEL_FRAME_SIZE;
    juce::AudioBuffer<float> silence (1, windowSize);
    silence.clear();
    dryFifo.write (silence, 0, windowSize);
    outputFifo.write (silence, 0, windowSize);

    setLatencySamples (windowSize);
    juce::Logger::writeToLog ("[VOID] Model unloaded.");

    // Clear persisted model path
    apvts.state.removeProperty (VoidParams::modelPath(), nullptr);
}

bool VoidAudioProcessor::isModelLoaded() const noexcept
{
    return inferenceCore.isModelLoaded();
}

juce::String VoidAudioProcessor::getLoadedModelPath() const noexcept
{
    return inferenceCore.getLoadedModelPath();
}

// ==============================================================================
// Parameter layout
// ==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout VoidAudioProcessor::createParameterLayout()
{
    using namespace VoidParams;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ---- AI Parameters ----
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (bypass(), 1),
        "Bypass", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (vacuumIntensity(), 1),
        "Vacuum",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        100.0f));

    // ---- Mode ----
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID (reductionMode(), 1),
        "Mode",
        juce::StringArray { "AI", "DSP", "Hybrid" },
        1));  // Default: DSP

    // ---- DSP Parameters ----
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (noiseThreshold(), 1),
        "Threshold",
        juce::NormalisableRange<float> (Ranges::thresholdMin, Ranges::thresholdMax, Ranges::thresholdStep),
        Ranges::thresholdDefault));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (noiseFloor(), 1),
        "Floor",
        juce::NormalisableRange<float> (Ranges::floorMin, Ranges::floorMax, Ranges::floorStep),
        Ranges::floorDefault));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (reductionAmount(), 1),
        "Amount",
        juce::NormalisableRange<float> (Ranges::amountMin, Ranges::amountMax, Ranges::amountStep),
        Ranges::amountDefault));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (attackTime(), 1),
        "Attack",
        juce::NormalisableRange<float> (Ranges::attackMin, Ranges::attackMax, Ranges::attackStep),
        Ranges::attackDefault));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (releaseTime(), 1),
        "Release",
        juce::NormalisableRange<float> (Ranges::releaseMin, Ranges::releaseMax, Ranges::releaseStep),
        Ranges::releaseDefault));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID (spectralSmooth(), 1),
        "Smooth",
        juce::NormalisableRange<float> (Ranges::smoothMin, Ranges::smoothMax, Ranges::smoothStep),
        Ranges::smoothDefault));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID (learnNoise(), 1),
        "Learn Noise", false));

    return { params.begin(), params.end() };
}

// ==============================================================================
// User Presets — XML Save / Load
// ==============================================================================

juce::File VoidAudioProcessor::getUserPresetDirectory()
{
    auto presetDir = juce::File::getSpecialLocation (
        juce::File::userApplicationDataDirectory)
        .getChildFile ("ASTRA Audio").getChildFile ("VOID").getChildFile ("Presets");
    if (!presetDir.exists())
        presetDir.createDirectory();
    return presetDir;
}

bool VoidAudioProcessor::savePresetToFile (const juce::File& file)
{
    // Create the root XML element
    auto xml = std::make_unique<juce::XmlElement> ("VOID_USER_PRESET");

    // Store preset name from filename (without extension)
    xml->setAttribute ("name", file.getFileNameWithoutExtension());
    xml->setAttribute ("version", "1.0");

    // Copy current APVTS state into the XML
    auto state = apvts.copyState();
    auto stateXml = state.createXml();
    if (stateXml != nullptr)
        xml->addChildElement (stateXml.release());

    // Also persist the model path if a model is loaded
    if (isModelLoaded())
        xml->setAttribute ("modelPath", getLoadedModelPath());

    // Write to file
    return xml->writeTo (file);
}

bool VoidAudioProcessor::loadPresetFromFile (const juce::File& file)
{
    auto xml = juce::parseXML (file);
    if (xml == nullptr)
        return false;

    if (! xml->hasTagName ("VOID_USER_PRESET"))
        return false;

    auto* stateXml = xml->getChildByName (apvts.state.getType());
    if (stateXml == nullptr)
        return false;

    // Restore APVTS parameters
    apvts.replaceState (juce::ValueTree::fromXml (*stateXml));

    // Try to restore the model
    juce::String savedModelPath = xml->getStringAttribute ("modelPath");
    if (savedModelPath.isNotEmpty())
    {
        juce::File modelFile (savedModelPath);
        if (modelFile.existsAsFile())
        {
            loadModel (savedModelPath);
            juce::Logger::writeToLog ("[VOID] Model restored from preset: " + savedModelPath);
        }
    }

    juce::Logger::writeToLog ("[VOID] User preset loaded: " + file.getFileName());
    return true;
}

// ==============================================================================
// Editor
// ==============================================================================

bool VoidAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* VoidAudioProcessor::createEditor()
{
    return new VoidAudioProcessorEditor (*this);
}

// ==============================================================================
// Plugin entry point
// ==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoidAudioProcessor();
}
