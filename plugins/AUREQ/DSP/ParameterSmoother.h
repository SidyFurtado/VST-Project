#pragma once

/**
 * A lightweight, real-time safe parameter smoother implementing exponential smoothing.
 * Formula: current += coeff * (target - current)
 * Supports O(1) block-level updates via getNextBlockValue.
 */
class ParameterSmoother
{
public:
    ParameterSmoother() = default;
    ~ParameterSmoother() = default;

    /**
     * Prepares the smoother with the current sample rate and target smoothing time.
     */
    void prepare(double sampleRate, float smoothingTimeMs);

    /**
     * Resets the smoother to a starting value immediately, disabling smoothing.
     */
    void reset(float value);

    /**
     * Sets a new target value. If the target differs from current, smoothing is enabled.
     */
    void setTarget(float value);

    /**
     * Steps the smoother by one sample and returns the new value.
     */
    float getNextValue();

    /**
     * Steps the smoother by numSamples as if it had run for that entire block,
     * and returns the final value. Extremely efficient O(1) block update.
     */
    float getNextBlockValue(int numSamples);

    /**
     * Returns the current smoothed value.
     */
    float getCurrentValue() const { return current; }

    /**
     * Returns true if the smoother is currently interpolating towards a target.
     */
    bool isSmoothing() const { return smoothingActive; }

private:
    double currentSampleRate = 0.0;
    float currentSmoothingTimeMs = 0.0f;
    float current = 0.0f;
    float target = 0.0f;
    float coeff = 1.0f;
    bool smoothingActive = false;
};
