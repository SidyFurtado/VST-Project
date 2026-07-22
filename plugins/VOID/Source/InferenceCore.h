#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
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

    // Crossfade length (Hann ramp at each edge of every output frame)
    static constexpr int CROSSFADE_LEN = 48;

    // Pre-allocated static buffers (Backing Store for Tensors)
    std::array<float, MODEL_FRAME_SIZE> inputFrame;
    std::array<float, MODEL_FRAME_SIZE> outputFrame;

    // Target sample rate for neural inference
    static constexpr double MODEL_SAMPLE_RATE = 48000.0;

    // Load an ONNX model from file path. Must be called from non-audio thread.
    bool loadModel (const juce::String& modelPath, double hostSampleRate);

    // Unload the current model.
    void unloadModel();

    // Check if the session is currently active and model is loaded.
    bool isModelLoaded() const noexcept;

    // Perform inference on the preallocated std::arrays.
    // Thread-safe: should be called from the Inference Thread.
    bool process();

    // Helper to get the loaded model path.
    juce::String getLoadedModelPath() const noexcept { return loadedModelPath; }

    // True if the loaded model has RNN hidden-state inputs/outputs
    bool hasRnnStates() const noexcept { return !stateInputNames.empty(); }

    // Resample a block of input audio from hostRate -> 48kHz, write to inputFrame.
    // Returns the number of 48kHz samples produced (should equal MODEL_FRAME_SIZE).
    int resampleToModelRate (const float* src, int numSrcSamples);

    // Resample the current outputFrame from 48kHz -> hostRate.
    // Returns number of host-rate samples written into dest.
    int resampleToHostRate (float* dest, int maxDestSamples);

    // Apply Hann crossfade ramp over CROSSFADE_LEN samples at boundaries of outputFrame,
    // blending with the history from the previous output frame.
    void applyCrossfade();

    // Reset resampler state (call on prepareToPlay)
    void resetResamplers (double hostSampleRate);

    // Is resampling needed?
    bool needsResampling() const noexcept { return resamplingEnabled; }

    // Expose current host sample rate so InferenceThread can calculate
    // how many host-rate input samples are needed per 48kHz frame.
    double getHostRate() const noexcept { return hostSampleRate; }

    // Reset crossfade state — call on prepareToPlay / transport restart
    void resetCrossfade();

    // Metering: get the current AI reduction in dB (input vs output RMS)
    float getAIReductionDb() const noexcept { return currentReductionDb.load(); }

    bool useDummyPassthrough { false };

private:
    Ort::Env env;
    std::unique_ptr<Ort::Session> session;
    
    // Critical Section to protect session loading / unloading from concurrent inference
    juce::CriticalSection sessionLock;

    juce::String loadedModelPath;

    // Cached primary input/output info
    std::string inputName;
    std::string outputName;
    std::vector<int64_t> inputShape;
    std::vector<int64_t> outputShape;

    // ---- RNN Hidden State Support -----------------------------------------------
    // Persistent state tensors (h/c for LSTM, h for GRU, etc.)
    struct StateNode
    {
        std::string inputName;   // e.g. "h_in"
        std::string outputName;  // e.g. "h_out"
        std::vector<int64_t>  shape;
        std::vector<float>    data;  // persistent between frames
    };
    std::vector<StateNode> stateNodes;
    std::vector<std::string> stateInputNames;
    std::vector<std::string> stateOutputNames;
    
    // Temporary ORT value vectors used during session.Run (allocated once per loadModel)
    std::vector<Ort::Value> allInputTensors;
    std::vector<Ort::Value> allOutputTensors;
    std::vector<const char*> allInputNamePtrs;
    std::vector<const char*> allOutputNamePtrs;

    // ---- Crossfade History -------------------------------------------------------
    std::array<float, CROSSFADE_LEN> prevOutputTail;  // tail of previous outputFrame
    bool firstFrame { true };

    // ---- Reduction Metering -------------------------------------------------------
    float inputFrameRms  = 0.0f;
    float outputFrameRms = 0.0f;
    std::atomic<float> currentReductionDb { 0.0f };
    float smoothedReductionDb = 0.0f;

    void updateReductionMeter();

    // ---- State Awareness ----------------------------------------------------------
    bool hasProcessedAnyFrames { false };
    // ---- Resampling -------------------------------------------------------------
    bool resamplingEnabled { false };
    double hostSampleRate  { 48000.0 };

    // Up/Down resamplers (host->model and model->host)
    juce::LagrangeInterpolator inputResampler;
    juce::LagrangeInterpolator outputResampler;

    // Staging buffers for resampling.
    // Must handle up to 192kHz host rate → ratio = 4.0 → need MODEL_FRAME_SIZE * 4 = 1920.
    static constexpr int MAX_RESAMPLE_BUFFER = MODEL_FRAME_SIZE * 4; // 1920
    std::array<float, MAX_RESAMPLE_BUFFER> resampleInStage;
    std::array<float, MAX_RESAMPLE_BUFFER> resampleOutStage;

    // Detects state nodes by inspecting model input/output node names
    void detectRnnStateNodes (Ort::AllocatorWithDefaultOptions& allocator);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InferenceCore)
};
