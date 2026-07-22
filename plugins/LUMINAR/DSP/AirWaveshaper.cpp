#include "AirWaveshaper.h"

#include <algorithm>
#include <cmath>

namespace LUMINAR::DSP
{
    float AirWaveshaper::processSample (float input, float drive) noexcept
    {
        if (! std::isfinite (input))
            input = 0.0f;

        const float safeInput = std::clamp (input, -inputLimit, inputLimit);
        const float safeDrive = std::clamp (std::isfinite (drive) ? drive : minDrive,
                                            minDrive,
                                            maxDrive);

        const float denominator = std::tanh (safeDrive);

        if (std::abs (denominator) < 1.0e-6f || ! std::isfinite (denominator))
            return safeInput;

        const float shaped = std::tanh (safeDrive * safeInput) / denominator;

        if (! std::isfinite (shaped))
            return 0.0f;

        return std::clamp (shaped, -inputLimit, inputLimit);
    }
}
