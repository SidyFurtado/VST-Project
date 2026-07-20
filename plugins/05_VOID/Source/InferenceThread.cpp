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
    while (!threadShouldExit())
    {
        // 1. Wait Event: Wait for a signal with a 10ms timeout
        signalEvent.wait (10);

        if (threadShouldExit())
            break;

        // 2. Queue Check: Loop while there are enough samples to fill a frame
        while (inputFifo.getNumReady() >= InferenceCore::MODEL_FRAME_SIZE)
        {
            if (threadShouldExit())
                break;

            // 3. Read: Fetch exactly MODEL_FRAME_SIZE samples from input FIFO
            float* inputPtrs[1] = { inferenceCore.inputFrame.data() };
            inputFifo.read (inputPtrs, 1, InferenceCore::MODEL_FRAME_SIZE);

            // 4. Neural Execution & 5. Extraction
            bool success = false;
            if (inferenceCore.isModelLoaded())
            {
                success = inferenceCore.process();
            }

            // If bypass or inference fails, copy input directly to output (clean bypass)
            if (!success)
            {
                juce::FloatVectorOperations::copy (inferenceCore.outputFrame.data(),
                                                   inferenceCore.inputFrame.data(),
                                                   InferenceCore::MODEL_FRAME_SIZE);
            }

            // 6. Write: Push the contents to output FIFO (drop frame if full)
            if (outputFifo.getFreeSpace() >= InferenceCore::MODEL_FRAME_SIZE)
            {
                const float* outputPtrs[1] = { inferenceCore.outputFrame.data() };
                outputFifo.write (outputPtrs, 1, InferenceCore::MODEL_FRAME_SIZE);
            }
            else
            {
                juce::Logger::writeToLog ("[VOID] InferenceThread output FIFO full. Dropping output frame.");
            }
        }
    }
}
