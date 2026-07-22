#include "InferenceThread.h"

InferenceThread::InferenceThread (InferenceCore& core,
                                 LockFreeAudioFifo& inFifo,
                                 LockFreeAudioFifo& outFifo,
                                 juce::WaitableEvent& sigEvent)
    : Thread ("VoidInferenceThread"),
      inferenceCore (core),
      inputFifo (inFifo),
      outputFifo (outFifo),
      signalEvent (sigEvent)
{
}

InferenceThread::~InferenceThread()
{
    stopThread (2000);
}

void InferenceThread::prepare()
{
    stopThread (2000);
    startThread (juce::Thread::Priority::high);
}

void InferenceThread::run()
{
    // Staging buffer for resampled host-rate output from InferenceCore.
    // Must be large enough to hold output at any sample rate up to 192kHz.
    // At 192kHz: 480 model samples → ceil(480 / (48000/192000)) = ceil(480 / 0.25) = 1920 host samples.
    static constexpr int OUT_STAGE_SIZE = InferenceCore::MODEL_FRAME_SIZE * 4; // 1920
    std::array<float, OUT_STAGE_SIZE> outStage;
    outStage.fill (0.0f);

    // Staging buffer for input resampling (host → 48kHz).
    // At 192kHz: 1920 host samples → 480 model samples.
    static constexpr int IN_STAGE_SIZE = InferenceCore::MODEL_FRAME_SIZE * 4; // 1920
    std::array<float, IN_STAGE_SIZE> inStage;
    inStage.fill (0.0f);

    while (!threadShouldExit())
    {
        // 1. Wait for signal from audio thread with AutoReset event.
        //    After wait returns, reset the event so the next wait() actually
        //    blocks instead of spinning at 100% CPU (ManualReset default bug).
        signalEvent.wait (10);

        if (threadShouldExit())
            break;

        // 2. Loop while we have a full frame to consume
        while (inputFifo.getNumReady() >= InferenceCore::MODEL_FRAME_SIZE)
        {
            if (threadShouldExit())
                break;

            bool success = false;
            int outSamples = InferenceCore::MODEL_FRAME_SIZE;

            if (inferenceCore.isModelLoaded())
            {
                if (inferenceCore.needsResampling())
                {
                    // ---- Resampling path (host rate ≠ 48kHz) ----
                    // Calculate how many host-rate input samples we need to produce
                    // one 48kHz output frame of MODEL_FRAME_SIZE samples.
                    const double hostToModelRatio = inferenceCore.getHostRate() / InferenceCore::MODEL_SAMPLE_RATE;
                    const int inputSamplesNeeded = static_cast<int> (std::ceil (hostToModelRatio * (double) InferenceCore::MODEL_FRAME_SIZE));

                    // Read host-rate samples from the input FIFO into the staging buffer
                    inStage.fill (0.0f);
                    float* inPtr = inStage.data();
                    inputFifo.read (&inPtr, 1, inputSamplesNeeded);

                    // Resample host-rate samples → 48kHz into inferenceCore.inputFrame
                    inferenceCore.resampleToModelRate (inStage.data(), inputSamplesNeeded);
                }
                else
                {
                    // ---- Direct path (host rate == 48kHz) ----
                    // Zero-copy: read directly into the inference backing store
                    float* inputPtr = inferenceCore.inputFrame.data();
                    inputFifo.read (&inputPtr, 1, InferenceCore::MODEL_FRAME_SIZE);
                }

                // 3. Run neural inference (also applies crossfade internally via process())
                success = inferenceCore.process();
            }
            else
            {
                // No model loaded: drain queue and pass through
                float* inputPtr = inferenceCore.inputFrame.data();
                inputFifo.read (&inputPtr, 1, InferenceCore::MODEL_FRAME_SIZE);
                juce::FloatVectorOperations::copy (inferenceCore.outputFrame.data(),
                                                   inferenceCore.inputFrame.data(),
                                                   InferenceCore::MODEL_FRAME_SIZE);
                success = true;
            }

            if (!success)
                continue;

            // 4. Determine the number of host-rate output samples to push
            if (inferenceCore.needsResampling())
            {
                // Resample 48kHz output → host rate
                // The resampleToHostRate now clamps internally to prevent buffer overflow
                outSamples = inferenceCore.resampleToHostRate (outStage.data(), OUT_STAGE_SIZE);

                if (outSamples > 0 && outputFifo.getFreeSpace() >= outSamples)
                {
                    const float* outPtr = outStage.data();
                    outputFifo.write (&outPtr, 1, outSamples);
                }
                else
                {
                    juce::Logger::writeToLog ("[VOID] Output FIFO full or resample produced 0 samples. Dropping.");
                }
            }
            else
            {
                // No resampling: push outputFrame directly
                if (outputFifo.getFreeSpace() >= outSamples)
                {
                    const float* outPtr = inferenceCore.outputFrame.data();
                    outputFifo.write (&outPtr, 1, outSamples);
                }
                else
                {
                    juce::Logger::writeToLog ("[VOID] InferenceThread output FIFO full. Dropping output frame.");
                }
            }
        }
    }
}
