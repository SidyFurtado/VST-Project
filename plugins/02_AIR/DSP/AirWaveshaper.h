#pragma once

#include <juce_core/juce_core.h>

namespace LUMINAR::DSP
{
    class AirWaveshaper
    {
    public:
        static float processSample (float input, float drive) noexcept;

    private:
        static constexpr float minDrive = 1.0f;
        static constexpr float maxDrive = 5.0f;
        static constexpr float inputLimit = 2.0f;
    };
}
