#include "ParameterSmoother.h"
#include <cmath>
#include <algorithm>

void ParameterSmoother::prepare(double sampleRate, float smoothingTimeMs)
{
    currentSampleRate = sampleRate;
    currentSmoothingTimeMs = smoothingTimeMs;

    if (sampleRate > 0.0 && smoothingTimeMs > 0.0f)
    {
        // Tau represents the time constant to reach ~63.2% of the target
        double tau = static_cast<double>(smoothingTimeMs) * 0.001;
        coeff = static_cast<float>(1.0 - std::exp(-1.0 / (sampleRate * tau)));
        coeff = std::max(0.0f, std::min(coeff, 1.0f));
    }
    else
    {
        coeff = 1.0f;
    }
}

void ParameterSmoother::reset(float value)
{
    current = value;
    target = value;
    smoothingActive = false;
}

void ParameterSmoother::setTarget(float value)
{
    target = value;
    if (std::abs(target - current) > 1e-6f)
    {
        smoothingActive = true;
    }
    else
    {
        current = target;
        smoothingActive = false;
    }
}

float ParameterSmoother::getNextValue()
{
    if (!smoothingActive)
    {
        return current;
    }

    if (coeff >= 1.0f)
    {
        current = target;
        smoothingActive = false;
        return current;
    }

    current += coeff * (target - current);

    // Safety checks against NaNs/Infs
    if (std::isnan(current) || std::isinf(current))
    {
        current = target;
        smoothingActive = false;
    }

    // Snap to target if very close to avoid denormal numbers or endless trailing
    if (std::abs(target - current) < 1e-6f)
    {
        current = target;
        smoothingActive = false;
    }

    return current;
}

float ParameterSmoother::getNextBlockValue(int numSamples)
{
    if (numSamples <= 0 || !smoothingActive)
    {
        return current;
    }

    if (coeff >= 1.0f)
    {
        current = target;
        smoothingActive = false;
        return current;
    }

    // Advance as if processed for numSamples samples
    // Formula: current = target - (target - current) * (1 - coeff)^numSamples
    float ratio = std::pow(1.0f - coeff, static_cast<float>(numSamples));

    if (std::isnan(ratio) || std::isinf(ratio))
    {
        ratio = 0.0f; // Snaps directly to target
    }

    current = target - (target - current) * ratio;

    // Safety checks against NaNs/Infs
    if (std::isnan(current) || std::isinf(current))
    {
        current = target;
        smoothingActive = false;
    }

    // Snap to target if very close to avoid denormal numbers or endless trailing
    if (std::abs(target - current) < 1e-6f)
    {
        current = target;
        smoothingActive = false;
    }

    return current;
}
