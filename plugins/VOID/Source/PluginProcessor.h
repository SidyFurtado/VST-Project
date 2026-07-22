#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "InferenceCore.h"
#include "InferenceThread.h"
#include "SpectralNoiseReducer.h"

// ==============================================================================
// Noise Reduction Mode
// ==============================================================================

enum class ReductionMode
{
    AI     = 0,   // ONNX only
    DSP    = 1,   // Spectral subtraction + noise gate
    Hybrid = 2    // AI processed with DSP enhancement
};

// ==============================================================================
// Factory Preset
// ==============================================================================

struct Preset
{
    juce::String name;
    float vacuumIntensity;
    int   reductionMode;
    float noiseThreshold;
    float noiseFloor;
    float reductionAmount;
    float attackTime;
    float releaseTime;
    float spectralSmooth;
};

// ==============================================================================
// VoidAudioProcessor — ASTRA Audio Suite / VOID
// ==============================================================================

class VoidAudioProcessor : public juce::AudioProcessor
{
public:
    VoidAudioProcessor();
    ~VoidAudioProcessor() override;

    // ---- Audio lifecycle --------------------------------------------------------
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // ---- Editor -----------------------------------------------------------------
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // ---- Plugin metadata --------------------------------------------------------
    const juce::String getName() const override;
    bool acceptsMidi()  const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // ---- Program / Presets ------------------------------------------------------
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    /** Load a factory preset by index (0=Custom, 1-4=factory). */
    void loadPreset (int presetIndex);
    static const std::vector<Preset>& getFactoryPresets();

    // ---- User Presets (XML save/load) ------------------------------------------
    /** Save the current APVTS state to an XML file. */
    bool savePresetToFile (const juce::File& file);
    /** Load APVTS state from an XML file. */
    bool loadPresetFromFile (const juce::File& file);
    /** Get the standard user preset directory (creates it if missing). */
    static juce::File getUserPresetDirectory();

    // ---- State persistence ------------------------------------------------------
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // ---- Model loading ----------------------------------------------------------
    bool loadModel (const juce::String& modelPath);
    void unloadModel();
    bool isModelLoaded() const noexcept;
    juce::String getLoadedModelPath() const noexcept;

    // ---- APVTS ------------------------------------------------------------------
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    // ---- Metering (thread-safe read from UI thread) ------------------------------
    float getReductionDb() const noexcept;
    float getDspReductionDb() const noexcept { return spectralReducer.getCurrentReductionDb(); }
    float getEstimatedNoiseFloorDb() const noexcept { return spectralReducer.getEstimatedNoiseFloorDb(); }
    float getInputLevelDb() const noexcept { return inputLevelDb; }
    float getOutputLevelDb() const noexcept { return outputLevelDb; }
    bool isCurrentlyReducing() const noexcept;

private:
    // ---- AI Inference Engine -----------------------------------------------------
    InferenceCore inferenceCore;
    juce::WaitableEvent signalEvent { false }; // AutoReset mode
    
    // Fifo queues (for AI processing pipeline)
    LockFreeAudioFifo inputFifo;
    LockFreeAudioFifo outputFifo;
    LockFreeAudioFifo dryFifo;
    
    InferenceThread inferenceThread;

    // ---- DSP Noise Reduction Engine ---------------------------------------------
    SpectralNoiseReducer spectralReducer;

    // ---- Cached parameter pointers -----------------------------------------------
    std::atomic<float>* vacuumIntensityParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;
    std::atomic<float>* noiseThresholdParam = nullptr;
    std::atomic<float>* noiseFloorParam = nullptr;
    std::atomic<float>* reductionAmountParam = nullptr;
    std::atomic<float>* attackTimeParam = nullptr;
    std::atomic<float>* releaseTimeParam = nullptr;
    std::atomic<float>* spectralSmoothParam = nullptr;
    std::atomic<float>* reductionModeParam = nullptr;
    std::atomic<float>* learnNoiseParam = nullptr;

    // ---- Crossfade constants -----------------------------------------------------
    static constexpr int CROSSFADE_LEN = InferenceCore::CROSSFADE_LEN;

    // ---- Pre-allocated audio thread buffers (zero allocations in processBlock) ----
    juce::AudioBuffer<float> monoInputBuffer;
    juce::AudioBuffer<float> monoOutputBuffer;
    juce::AudioBuffer<float> monoDryBuffer;
    juce::AudioBuffer<float> dspProcessBuffer;
    juce::AudioBuffer<float> processedBuffer;   // Replaces stack-allocated buffer in processBlock
    juce::AudioBuffer<float> aiBuffer;          // Replaces stack-allocated buffer in Hybrid mode
    juce::AudioBuffer<float> pipelineDryBuffer; // Replaces dryDrainBuffer in processAIPipeline
    juce::AudioBuffer<float> pipelineWetBuffer; // Replaces wetBuffer in processAIPipeline

    // ---- Level metering (audio thread, read by UI thread) ------------------------
    std::atomic<float> inputLevelDb  { -90.0f };
    std::atomic<float> outputLevelDb { -90.0f };

    // ---- State -------------------------------------------------------------------
    bool dspPrepared = false;
    bool learnNoiseRequested = false;
    bool modelRestoreAttempted = false;  // Ensures model auto-restore runs only once

    // ---- Internal helpers --------------------------------------------------------
    void applyDSPNoiseReduction (const float* input, float* output, int numSamples);
    void processAIPipeline (const juce::AudioBuffer<float>& input,
                            juce::AudioBuffer<float>& output,
                            int numSamples);
    int getEffectiveReductionMode() const noexcept;
    void prefillOutputFifoWithSilentFrames (int numFrames);
    void updateDSPParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidAudioProcessor)
};
