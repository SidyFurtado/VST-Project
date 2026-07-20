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
    // Staging buffer for resampled host-rate output from InferenceCore
    static constexpr int OUT_STAGE_SIZE = InferenceCore::MODEL_FRAME_SIZE * 4;
    std::array<float, OUT_STAGE_SIZE> outStage;
    outStage.fill (0.0f);

    while (!threadShouldExit())
    {
        // 1. Wait for signal from audio thread (10ms timeout)
        signalEvent.wait (10);

        if (threadShouldExit())
            break;

        // 2. Loop while we have a full frame to consume
        while (inputFifo.getNumReady() >= InferenceCore::MODEL_FRAME_SIZE)
        {
            if (threadShouldExit())
                break;

            bool success = false;

            if (inferenceCore.isModelLoaded())
            {
                // 3a. If the host sample rate differs from 48kHz, resample into inputFrame
                if (inferenceCore.needsResampling())
                {
                    // Read raw samples into a temporary buffer
                    std::array<float, InferenceCore::MODEL_FRAME_SIZE> srcBuffer;
                    srcBuffer.fill (0.0f);
                    float* srcPtr = srcBuffer.data();
                    inputFifo.read (&srcPtr, 1, InferenceCore::MODEL_FRAME_SIZE);

                    // Resample host-rate -> 48kHz into inferenceCore.inputFrame
                    inferenceCore.resampleToModelRate (srcBuffer.data(), InferenceCore::MODEL_FRAME_SIZE);
                }
                else
                {
                    // 3b. Read samples directly into the inference backing store (zero-copy path)
                    float* inputPtr = inferenceCore.inputFrame.data();
                    inputFifo.read (&inputPtr, 1, InferenceCore::MODEL_FRAME_SIZE);
                }

                // 4. Run neural inference (also applies crossfade internally)
                success = inferenceCore.process();
            }
            else
            {
                // No model: drain queue and pass through
                float* inputPtr = inferenceCore.inputFrame.data();
                inputFifo.read (&inputPtr, 1, InferenceCore::MODEL_FRAME_SIZE);
                juce::FloatVectorOperations::copy (inferenceCore.outputFrame.data(),
                                                   inferenceCore.inputFrame.data(),
                                                   InferenceCore::MODEL_FRAME_SIZE);
            }

            // 5. Determine the number of host-rate output samples to push
            int outSamples = InferenceCore::MODEL_FRAME_SIZE;

            if (success && inferenceCore.needsResampling())
            {
                // Resample 48kHz output -> host rate
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
