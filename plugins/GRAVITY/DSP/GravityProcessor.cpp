#include "GravityProcessor.h"

#include <algorithm>
#include <cmath>

// ==============================================================================
// GRAVITY::DSP::GravityProcessor — Implementation
//
// Buffer quality baseline: mirrors AUREQ / LUMINAR conventions:
//   — ScopedNoDenormals on every process boundary.
//   — All sample-level paths guarded with std::isfinite.
//   — Time coefficients use single-pole RC approximation (standard for VCA).
//   — SmoothedValue on makeup gain to eliminate zipper noise.
//
// Compressor topology (per block):
//   1. Detect input level  → dBFS (Peak or RMS depending on mode).
//   2. Feed level into GainComputer → target gain reduction in dB.
//   3. Apply attack/release envelope → smooth gain trajectory.
//   4. ARC: if transient detected, shorten release by 50%.
//   5. Convert gain-reduction dB → linear multiplier.
//   6. Apply multiplier + makeup gain to each sample.
//   7. Update metering atomics.
// ==============================================================================

namespace GRAVITY::DSP
{

// ==============================================================================
// ChannelState helpers
// ==============================================================================

void GravityProcessor::ChannelState::reset() noexcept
{
    peakEnv      = 0.0f;
    rmsSquareSum = 0.0f;
    rmsWindowPos = 0;
    gainEnv      = 1.0f;

    for (auto& v : rmsWindow)
        v = 0.0f;
}

void GravityProcessor::ChannelState::prepareRms (int windowSamples)
{
    const int safeSize = std::max (windowSamples, 1);

    rmsSquareSum = 0.0f;
    rmsWindowPos = 0;
    rmsWindow.assign (static_cast<size_t> (safeSize), 0.0f);
}

// ==============================================================================
// Lifecycle
// ==============================================================================

void GravityProcessor::prepare (double newSampleRate, int maxBlockSize, int maxChannels)
{
    sampleRate       = std::max (newSampleRate, 1.0);
    preparedBlockSize = std::max (maxBlockSize,   1);
    preparedChannels  = std::max (maxChannels,    1);

    const int rmsWindowSamples = static_cast<int> (
        std::round (kRmsWindowMs * 0.001 * sampleRate));

    channels.resize (static_cast<size_t> (preparedChannels));
    for (auto& ch : channels)
        ch.prepareRms (rmsWindowSamples);

    // Makeup gain: 20 ms linear ramp, initialised to unity.
    makeupGainSmoother.reset (sampleRate, 0.020);
    makeupGainSmoother.setCurrentAndTargetValue (1.0f);

    // Force coefficient recalculation on next process().
    cachedAttackMs  = -1.0f;
    cachedReleaseMs = -1.0f;

    gainReductionDb.store (0.0f,   std::memory_order_relaxed);
    inputLevelDb.store    (-96.0f, std::memory_order_relaxed);
    outputLevelDb.store   (-96.0f, std::memory_order_relaxed);

    reset();
}

void GravityProcessor::reset() noexcept
{
    for (auto& ch : channels)
        ch.reset();

    makeupGainSmoother.setCurrentAndTargetValue (makeupGainSmoother.getTargetValue());
}

// ==============================================================================
// Static helpers
// ==============================================================================

float GravityProcessor::timeConstant (float timeMs, double sr) noexcept
{
    // Standard single-pole RC: coeff = exp(-1 / (time_in_samples))
    const float timeSamples = std::max (timeMs * 0.001f * static_cast<float> (sr), 1.0f);
    return std::exp (-1.0f / timeSamples);
}

// ==============================================================================
// Gain Computer — logarithmic transfer curve with 6 dB soft-knee
// ==============================================================================
//
//  Soft-knee region: [threshold - 3, threshold + 3] dB
//
//  Below knee:  GR = 0 dB (unity)
//  In knee:     GR = (1/ratio - 1) * (inputDb - (threshold - halfKnee))^2
//                     / (2 * halfKnee)         [quadratic blend]
//  Above knee:  GR = threshold + (inputDb - threshold) / ratio - inputDb
//
float GravityProcessor::computeGainReductionDb (float inputDb,
                                                float thresholdDb,
                                                float ratio) noexcept
{
    constexpr float kHalfKneeDb = 3.0f;   // 6 dB total knee / 2
    const float safeRatio = std::max (ratio, 1.0f);

    const float kneeBottom = thresholdDb - kHalfKneeDb;
    const float kneeTop    = thresholdDb + kHalfKneeDb;

    if (inputDb <= kneeBottom)
        return 0.0f;   // Fully below threshold — no compression.

    if (inputDb >= kneeTop)
    {
        // Hard slope above the knee.
        // Output should be: threshold + (inputDb - threshold) / ratio
        // GR = output - input
        return thresholdDb + (inputDb - thresholdDb) / safeRatio - inputDb;
    }

    // Soft-knee quadratic blend:
    //   When inputDb == kneeBottom → GR approaches 0
    //   When inputDb == kneeTop    → GR approaches the hard slope value
    const float kneeWidth    = 2.0f * kHalfKneeDb;
    const float overshoot    = inputDb - kneeBottom;   // 0 at bottom, kneeWidth at top
    const float slopeFactor  = (1.0f / safeRatio) - 1.0f;

    return slopeFactor * (overshoot * overshoot) / (2.0f * kneeWidth);
}

// ==============================================================================
// Envelope follower with ARC
// ==============================================================================

float GravityProcessor::applyGainEnvelope (ChannelState& ch,
                                           float targetGainLinear,
                                           float attackCoeff_,
                                           float releaseCoeff_,
                                           float arcReleaseCoeff_,
                                           bool  arcEnabled) noexcept
{
    const float prevEnv = ch.gainEnv;

    // Determine effective release: ARC shortens it on fast transients.
    float effectiveRelease = releaseCoeff_;

    if (arcEnabled)
    {
        // A transient is detected when the gain-reduction target changes
        // quickly (the target is jumping further down = more compression).
        const float deltaGr = prevEnv - targetGainLinear;  // positive = more GR needed
        if (deltaGr > kArcTransientThresholdDb * 0.001f)   // scale to linear domain heuristic
            effectiveRelease = arcReleaseCoeff_;
    }

    float newEnv;
    if (targetGainLinear < prevEnv)
    {
        // Attack: compressing (target gain drops = more gain reduction).
        newEnv = attackCoeff_ * prevEnv + (1.0f - attackCoeff_) * targetGainLinear;
    }
    else
    {
        // Release: recovering (target gain rises = less gain reduction).
        newEnv = effectiveRelease * prevEnv + (1.0f - effectiveRelease) * targetGainLinear;
    }

    if (! std::isfinite (newEnv))
        newEnv = 1.0f;

    ch.gainEnv = std::clamp (newEnv, 0.0f, 1.0f);
    return ch.gainEnv;
}

// ==============================================================================
// Level detector
// ==============================================================================

float GravityProcessor::detectLevel (ChannelState& ch,
                                     const float*  samples,
                                     int           numSamples,
                                     DetectorMode  mode) noexcept
{
    if (numSamples <= 0 || samples == nullptr)
        return -96.0f;

    if (mode == DetectorMode::Peak)
    {
        // Peak follower: absolute peak of the entire block.
        float peak = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            const float s = std::isfinite (samples[i]) ? std::abs (samples[i]) : 0.0f;
            peak = std::max (peak, s);
        }

        // Smooth the peak envelope with a fast attack / slow release
        // (the main attack/release is applied at the gain stage; this is
        // just to keep the detector numerically stable).
        ch.peakEnv = std::max (peak, ch.peakEnv * 0.9999f);

        const float level = std::max (ch.peakEnv, 1e-8f);
        return 20.0f * std::log10 (level);
    }
    else  // RMS / Opto
    {
        // Sliding-window RMS.
        const size_t windowSize = ch.rmsWindow.size();

        if (windowSize == 0)
            return -96.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            const float s = std::isfinite (samples[i]) ? samples[i] : 0.0f;
            const float sq = s * s;

            // Subtract oldest sample, add newest.
            const size_t pos = static_cast<size_t> (ch.rmsWindowPos);
            ch.rmsSquareSum -= ch.rmsWindow[pos];
            ch.rmsWindow[pos] = sq;
            ch.rmsSquareSum += sq;

            ch.rmsWindowPos = static_cast<int> ((pos + 1) % windowSize);
        }

        ch.rmsSquareSum = std::max (ch.rmsSquareSum, 0.0f);
        const float rms = std::sqrt (ch.rmsSquareSum / static_cast<float> (windowSize));
        const float level = std::max (rms, 1e-8f);
        return 20.0f * std::log10 (level);
    }
}

// ==============================================================================
// Time coefficient update
// ==============================================================================

void GravityProcessor::updateTimeCoefficients (float attackMs, float releaseMs)
{
    const float safeAttack  = std::clamp (attackMs,  0.1f,   500.0f);
    const float safeRelease = std::clamp (releaseMs, 5.0f,  3000.0f);

    if (std::abs (safeAttack  - cachedAttackMs)  > 0.01f ||
        std::abs (safeRelease - cachedReleaseMs) > 0.01f)
    {
        attackCoeff     = timeConstant (safeAttack,  sampleRate);
        releaseCoeff    = timeConstant (safeRelease, sampleRate);

        // ARC release: 50% of user release time → faster decay.
        arcReleaseCoeff = timeConstant (safeRelease * 0.50f, sampleRate);

        cachedAttackMs  = safeAttack;
        cachedReleaseMs = safeRelease;
    }
}

// ==============================================================================
// ensureCapacity — graceful handling of layout changes after prepare
// ==============================================================================

void GravityProcessor::ensureCapacity (int numChannels, int numSamples)
{
    bool needsReset = false;

    if (numChannels > preparedChannels)
    {
        preparedChannels = numChannels;
        const int rmsWindowSamples = static_cast<int> (
            std::round (kRmsWindowMs * 0.001 * sampleRate));

        channels.resize (static_cast<size_t> (preparedChannels));
        for (auto& ch : channels)
            ch.prepareRms (rmsWindowSamples);

        needsReset = true;
    }

    if (numSamples > preparedBlockSize)
    {
        preparedBlockSize = numSamples;
        needsReset = true;
    }

    if (needsReset)
        reset();
}

// ==============================================================================
// process — main DSP entry point
// ==============================================================================

void GravityProcessor::process (juce::AudioBuffer<float>& buffer,
                                float thresholdDb,
                                float ratio,
                                float attackMs,
                                float releaseMs,
                                float makeupGainDb,
                                DetectorMode mode,
                                bool  arcEnabled)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels <= 0 || numSamples <= 0)
        return;

    ensureCapacity (numChannels, numSamples);
    updateTimeCoefficients (attackMs, releaseMs);

    // Clamp parameters to valid ranges.
    const float safeThresh = std::clamp (thresholdDb, -60.0f, 0.0f);
    const float safeRatio  = std::clamp (ratio,         1.0f, 50.0f);
    const float safeGainDb = std::clamp (makeupGainDb,  0.0f, 30.0f);

    // Set makeup gain target on smoother.
    const float makeupLinear = juce::Decibels::decibelsToGain (safeGainDb);
    makeupGainSmoother.setTargetValue (makeupLinear);

    // ---- Per-block metering accumulators ------------------------------------
    float totalInputPower  = 0.0f;
    float totalOutputPower = 0.0f;
    float maxGrDb          = 0.0f;   // Most negative GR (highest reduction)

    // ---- Process each channel -----------------------------------------------
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto& state = channels[static_cast<size_t> (ch)];
        float* data = buffer.getWritePointer (ch);

        // 1. Detect input level for the whole block.
        const float inputLevelDbCh = detectLevel (state, data, numSamples, mode);

        // 2. Compute target gain reduction (same for all samples in block —
        //    the envelope smoother handles per-sample interpolation).
        const float targetGrDb = computeGainReductionDb (inputLevelDbCh,
                                                          safeThresh,
                                                          safeRatio);

        // Convert gain reduction to linear target for the envelope follower.
        const float targetGainLinear = juce::Decibels::decibelsToGain (targetGrDb);

        // 3. Apply envelope + ARC and process samples.
        for (int i = 0; i < numSamples; ++i)
        {
            // Advance gain envelope (one call per sample for smoothness).
            const float gain = applyGainEnvelope (state,
                                                  targetGainLinear,
                                                  attackCoeff,
                                                  releaseCoeff,
                                                  arcReleaseCoeff,
                                                  arcEnabled);

            // 4. Apply gain reduction + makeup gain (smoothed).
            const float makeup = makeupGainSmoother.getNextValue();
            float sample = data[i];

            if (! std::isfinite (sample))
                sample = 0.0f;

            const float compressed = sample * gain * makeup;

            // Hard clip at ±4 to prevent runaway (matches LUMINAR headroom).
            data[i] = std::clamp (compressed, -4.0f, 4.0f);

            // Metering accumulators.
            totalInputPower  += sample     * sample;
            totalOutputPower += data[i]    * data[i];
        }

        // Track maximum GR across channels for the GR meter.
        maxGrDb = std::min (maxGrDb, targetGrDb);
    }

    // ---- Update metering atomics (one write per block, not per sample) ------
    const float invSamplesChannels = 1.0f / static_cast<float> (numSamples * numChannels);

    const float rmsIn  = std::sqrt (totalInputPower  * invSamplesChannels);
    const float rmsOut = std::sqrt (totalOutputPower * invSamplesChannels);

    gainReductionDb.store (maxGrDb,
                           std::memory_order_relaxed);
    inputLevelDb.store    (rmsIn  > 1e-8f ? 20.0f * std::log10 (rmsIn)  : -96.0f,
                           std::memory_order_relaxed);
    outputLevelDb.store   (rmsOut > 1e-8f ? 20.0f * std::log10 (rmsOut) : -96.0f,
                           std::memory_order_relaxed);
}

// ==============================================================================
// Metering accessors
// ==============================================================================

float GravityProcessor::getGainReductionDb()  const noexcept
{
    return gainReductionDb.load (std::memory_order_relaxed);
}

float GravityProcessor::getInputLevelDb()     const noexcept
{
    return inputLevelDb.load (std::memory_order_relaxed);
}

float GravityProcessor::getOutputLevelDb()    const noexcept
{
    return outputLevelDb.load (std::memory_order_relaxed);
}

} // namespace GRAVITY::DSP
