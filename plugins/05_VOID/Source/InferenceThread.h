#pragma once

#include <juce_core/juce_core.h>
#include "InferenceCore.h"

class InferenceThread : public juce::Thread
{
public:
    InferenceThread (InferenceCore& core,
                     LockFreeAudioFifo& inputFifo,
                     LockFreeAudioFifo& outputFifo,
                     juce::WaitableEvent& signalEvent);
    ~InferenceThread() override;

    // Called on prepareToPlay to restart thread and reset state
    void prepare();

    // juce::Thread implementation
    void run() override;

private:
    InferenceCore& inferenceCore;
    LockFreeAudioFifo& inputFifo;
    LockFreeAudioFifo& outputFifo;
    juce::WaitableEvent& signalEvent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InferenceThread)
};
