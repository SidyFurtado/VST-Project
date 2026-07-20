#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <memory>
#include <vector>
#include <array>
#include <string>

// Include ONNX Runtime C++ API
#include <onnxruntime_cxx_api.h>

class LockFreeAudioFifo
{
public:
    LockFreeAudioFifo() = default;
    ~LockFreeAudioFifo() = default;

    void prepare (int numChannels, int capacity)
    {
        fifo.setTotalSize (capacity);
        buffer.setSize (numChannels, capacity);
        buffer.clear();
    }

    void reset()
    {
        fifo.reset();
        buffer.clear();
    }

    int getNumReady() const noexcept  { return fifo.getNumReady(); }
    int getFreeSpace() const noexcept { return fifo.getFreeSpace(); }
    int getCapacity() const noexcept  { return fifo.getTotalSize(); }

    // Write samples from JUCE AudioBuffer (Audio Thread)
    void write (const juce::AudioBuffer<float>& sourceBuffer, int startSample, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

        const int numChannels = std::min (buffer.getNumChannels(), sourceBuffer.getNumChannels());

        if (size1 > 0)
        {
            for (int ch = 0; ch < numChannels; ++ch)
                buffer.copyFrom (ch, start1, sourceBuffer, ch, startSample, size1);
        }

        if (size2 > 0)
        {
            for (int ch = 0; ch < numChannels; ++ch)
                buffer.copyFrom (ch, start2, sourceBuffer, ch, startSample + size1, size2);
        }

        fifo.finishedWrite (size1 + size2);
    }

    // Read samples to JUCE AudioBuffer (Audio Thread)
    void read (juce::AudioBuffer<float>& destBuffer, int startSample, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);

        const int numChannels = std::min (buffer.getNumChannels(), destBuffer.getNumChannels());

        if (size1 > 0)
        {
            for (int ch = 0; ch < numChannels; ++ch)
                destBuffer.copyFrom (ch, startSample, buffer, ch, start1, size1);
        }

        if (size2 > 0)
        {
            for (int ch = 0; ch < numChannels; ++ch)
                destBuffer.copyFrom (ch, startSample + size1, buffer, ch, start2, size2);
        }

        fifo.finishedRead (size1 + size2);
    }

    // Write samples from raw float arrays (Inference Thread)
    void write (const float* const* rawData, int numChannels, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

        const int channelsToCopy = std::min (buffer.getNumChannels(), numChannels);

        if (size1 > 0)
        {
            for (int ch = 0; ch < channelsToCopy; ++ch)
                buffer.copyFrom (ch, start1, rawData[ch], size1);
        }

        if (size2 > 0)
        {
            for (int ch = 0; ch < channelsToCopy; ++ch)
                buffer.copyFrom (ch, start2, rawData[ch] + size1, size2);
        }

        fifo.finishedWrite (size1 + size2);
    }

    // Read samples to raw float arrays (Inference Thread)
    void read (float** rawData, int numChannels, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);

        const int channelsToCopy = std::min (buffer.getNumChannels(), numChannels);

        if (size1 > 0)
        {
            for (int ch = 0; ch < channelsToCopy; ++ch)
                juce::FloatVectorOperations::copy (rawData[ch], buffer.getReadPointer (ch, start1), size1);
        }

        if (size2 > 0)
        {
            for (int ch = 0; ch < channelsToCopy; ++ch)
                juce::FloatVectorOperations::copy (rawData[ch] + size1, buffer.getReadPointer (ch, start2), size2);
        }

        fifo.finishedRead (size1 + size2);
    }

private:
    juce::AbstractFifo fifo { 1 };
    juce::AudioBuffer<float> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LockFreeAudioFifo)
};

class InferenceCore
{
public:
    InferenceCore();
    ~InferenceCore();

    // Constant size for standard frames (like RNNoise)
    static constexpr int MODEL_FRAME_SIZE = 480;

    // Pre-allocated static buffers (Backing Store for Tensors)
    std::array<float, MODEL_FRAME_SIZE> inputFrame;
    std::array<float, MODEL_FRAME_SIZE> outputFrame;

    // Load an ONNX model from file path.
    bool loadModel (const juce::String& modelPath);

    // Unload the current model.
    void unloadModel();

    // Check if the session is currently active and model is loaded.
    bool isModelLoaded() const noexcept;

    // Perform inference on the preallocated std::arrays.
    // Thread-safe: should be called from the Inference Thread.
    bool process();

    // Helper to get the loaded model path.
    juce::String getLoadedModelPath() const noexcept { return loadedModelPath; }

private:
    Ort::Env env;
    std::unique_ptr<Ort::Session> session;
    
    // Critical Section to protect session loading / unloading from concurrent inference
    juce::CriticalSection sessionLock;

    juce::String loadedModelPath;

    // Cached model info
    std::string inputName;
    std::string outputName;
    std::vector<int64_t> inputShape;
    std::vector<int64_t> outputShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InferenceCore)
};
