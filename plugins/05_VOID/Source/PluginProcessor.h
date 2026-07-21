#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "InferenceCore.h"
#include "InferenceThread.h"
#include <array>

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

    // ---- Program ----------------------------------------------------------------
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

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

private:
    // Core AI and threading
    InferenceCore inferenceCore;
    juce::WaitableEvent signalEvent;
    
    // Fifo queues (must be prepared in prepareToPlay)
    LockFreeAudioFifo inputFifo;
    LockFreeAudioFifo outputFifo;
    LockFreeAudioFifo dryFifo;
    
    InferenceThread inferenceThread;

    // Cached parameter pointers
    std::atomic<float>* vacuumIntensityParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    // Preallocated buffers for audio thread downmixing/upmixing/dry-path.
    // Absolutely no dynamic allocation must happen in processBlock.
    juce::AudioBuffer<float> monoInputBuffer;
    juce::AudioBuffer<float> monoOutputBuffer;
    juce::AudioBuffer<float> monoDryBuffer;

    // Staging buffer for resampled model output (host-rate samples from InferenceCore)
    static constexpr int RESAMPLE_OUT_MAX = InferenceCore::MODEL_FRAME_SIZE * 4;
    std::array<float, RESAMPLE_OUT_MAX> resampledOutputStage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoidAudioProcessor)
};
