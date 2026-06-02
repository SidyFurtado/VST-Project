#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include <vector>
#include <atomic>

// ==============================================================================
// GravityProcessor — ASTRA Audio Suite / GRAVITY
//
// High-performance dynamic compressor inspired by the Waves R-Comp lineage.
// Core algorithm pillars:
//   1. Detector Path    — Peak (Electro) and RMS (Opto) level detection.
//   2. Transfer Curve   — Logarithmic with a fixed 6 dB soft-knee zone.
//   3. ARC              — Auto Release Control: dynamic release adjustment
//                         that shortens release by up to 50% on fast transients.
//
// Designed to match the buffer quality standard established by AUREQ / LUMINAR:
//   — ScopedNoDenormals at every process boundary.
//   — isfinite guards on all sample-level operations.
//   — SmoothedValue on all time-variant coefficients.
//   — Per-channel state (no inter-channel bleed in gain computation).
// ==============================================================================

namespace GRAVITY::DSP
{
    // ============================= DetectorMode ================================

    enum class DetectorMode
    {
        Peak = 0,  ///< Electro mode — instant peak follower.
        RMS  = 1   ///< Opto mode   — square-mean RMS envelope.
    };

    // ============================= GravityProcessor ============================

    class GravityProcessor
    {
    public:
        GravityProcessor() = default;

        // ---- Lifecycle -------------------------------------------------------

        /** Call from PluginProcessor::prepareToPlay. */
        void prepare (double sampleRate, int maxBlockSize, int maxChannels);

        /** Reset all envelope followers and smoothers to zero. */
        void reset() noexcept;

        // ---- Main process call -----------------------------------------------

        /**
         * Compress `buffer` in-place.
         *
         * All parameter values are in their natural units:
         *   thresholdDb  — dB (negative)
         *   ratio        — linear ratio (≥ 1.0)
         *   attackMs     — milliseconds
         *   releaseMs    — milliseconds
         *   makeupGainDb — dB (positive)
         *   mode         — DetectorMode enum
         *   arcEnabled   — true = ARC on
         *
         * The caller (PluginProcessor) reads APVTS and passes values each block.
         */
        void process (juce::AudioBuffer<float>& buffer,
                      float thresholdDb,
                      float ratio,
                      float attackMs,
                      float releaseMs,
                      float makeupGainDb,
                      DetectorMode mode,
                      bool arcEnabled);

        // ---- Metering output (thread-safe reads by the UI timer) -------------

        /** Returns the current gain-reduction in dB (always ≤ 0). */
        float getGainReductionDb()  const noexcept;

        /** Returns the smoothed input level in dBFS. */
        float getInputLevelDb()     const noexcept;

        /** Returns the smoothed output level in dBFS. */
        float getOutputLevelDb()    const noexcept;

    private:
        // ---- Per-channel envelope state -------------------------------------

        struct ChannelState
        {
            // Peak follower envelope
            float peakEnv = 0.0f;

            // RMS accumulator (summed squares)
            float rmsSquareSum = 0.0f;
            int   rmsWindowPos = 0;
            std::vector<float> rmsWindow;

            // Gain-computer output (smoothed gain coefficient, linear)
            float gainEnv = 1.0f;

            void reset() noexcept;
            void prepareRms (int windowSamples);
        };

        // ---- Internal helpers -----------------------------------------------

        /**
         * Compute the desired gain reduction for a given input level in dBFS.
         *
         * Implements a logarithmic transfer curve with a 6 dB soft-knee:
         *   — Below (threshold - 3 dB): unity gain (0 dB GR).
         *   — Soft-knee zone ±3 dB around threshold: quadratic blend.
         *   — Above (threshold + 3 dB): hard ratio slope.
         *
         * Returns gain reduction in dB (≤ 0).
         */
        static float computeGainReductionDb (float inputDb,
                                             float thresholdDb,
                                             float ratio) noexcept;

        /**
         * Apply attack/release envelope to the gain-reduction signal.
         * ARC shortens the effective release by up to 50% when a fast
         * transient is detected (|ΔGR| > kArcTransientThresholdDb per sample).
         */
        float applyGainEnvelope (ChannelState& ch,
                                 float targetGainLinear,
                                 float attackCoeff,
                                 float releaseCoeff,
                                 float arcReleaseCoeff,
                                 bool  arcEnabled) noexcept;

        /** Detect input level: returns instantaneous dBFS for the given channel. */
        float detectLevel (ChannelState& ch,
                           const float* samples,
                           int           numSamples,
                           DetectorMode  mode) noexcept;

        /** Compute single-pole time constant from ms to sample-domain coefficient. */
        static float timeConstant (float timeMs, double sampleRate) noexcept;

        void ensureCapacity (int numChannels, int numSamples);
        void updateTimeCoefficients (float attackMs, float releaseMs);

        // ---- ARC constant ---------------------------------------------------
        // If the gain-reduction signal changes faster than this threshold per
        // sample (in dB), ARC activates the shorter release.
        static constexpr float kArcTransientThresholdDb = 0.5f;

        // ---- RMS window size in ms (Opto mode) ------------------------------
        static constexpr double kRmsWindowMs = 30.0;

        // ---- State ----------------------------------------------------------
        double sampleRate       = 44100.0;
        int    preparedChannels = 0;
        int    preparedBlockSize = 0;

        std::vector<ChannelState> channels;

        // Smoothed attack / release coefficients (updated each block)
        float attackCoeff      = 0.0f;
        float releaseCoeff     = 0.0f;
        float arcReleaseCoeff  = 0.0f;   ///< Release coeff at 50% of user release.

        // Cached parameter copies (to detect changes)
        float cachedAttackMs   = -1.0f;
        float cachedReleaseMs  = -1.0f;

        // Makeup gain smoother — prevents zipper noise on makeup gain changes.
        juce::SmoothedValue<float> makeupGainSmoother;

        // ---- Metering (atomic, safe for cross-thread reads) -----------------
        std::atomic<float> gainReductionDb  { 0.0f };
        std::atomic<float> inputLevelDb     { -96.0f };
        std::atomic<float> outputLevelDb    { -96.0f };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GravityProcessor)
    };

} // namespace GRAVITY::DSP
