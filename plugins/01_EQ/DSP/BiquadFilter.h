#pragma once

#include <vector>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * A robust, numerically stable audio-rate Biquad IIR Filter.
 * Implements Direct Form II Transposed (DF2T) topology.
 */
class BiquadFilter
{
public:
    BiquadFilter() = default;
    ~BiquadFilter() = default;

    /**
     * Prepares/allocates state vectors for the given sample rate and channel count.
     */
    void prepare(double sampleRate, int numChannels);

    /**
     * Clears internal state buffers to zero.
     */
    void reset();

    /**
     * Computes the coefficients for a Peaking/Bell EQ filter using the RBJ cookbook formula.
     * Incorporates protective clamping and fallbacks for NaN/infinite cases.
     */
    void setPeaking(float frequencyHz, float gainDb, float q);

    /**
     * High-Pass filter (12 dB/oct, Butterworth-like). Used for Low Cut.
     * Q controls resonance at cutoff (0.707 = maximally flat / Butterworth).
     */
    void setHighPass(float frequencyHz, float q);

    /**
     * Low-Pass filter (12 dB/oct, Butterworth-like). Used for High Cut.
     * Q controls resonance at cutoff (0.707 = maximally flat / Butterworth).
     */
    void setLowPass(float frequencyHz, float q);

    /**
     * Low Shelf filter. Applies gain to frequencies below frequencyHz.
     * Uses RBJ cookbook low-shelf formula.
     */
    void setLowShelf(float frequencyHz, float gainDb);

    /**
     * High Shelf filter. Applies gain to frequencies above frequencyHz.
     * Uses RBJ cookbook high-shelf formula.
     */
    void setHighShelf(float frequencyHz, float gainDb);

    /**
     * Notch (band-reject) filter. Attenuates a narrow band centered at frequencyHz.
     * Q controls notch width (higher Q = narrower).
     */
    void setNotch(float frequencyHz, float q);

    /**
     * Band-Pass filter (constant peak gain). Passes a narrow band centered at frequencyHz.
     * Q controls bandwidth.
     */
    void setBandPass(float frequencyHz, float q);

    /**
     * High-Pass filter 1st-order (6 dB/oct).
     */
    void setHighPassFirstOrder(float frequencyHz);

    /**
     * Low-Pass filter 1st-order (6 dB/oct).
     */
    void setLowPassFirstOrder(float frequencyHz);

    /**
     * Low Shelf filter 1st-order (6 dB/oct).
     */
    void setLowShelfFirstOrder(float frequencyHz, float gainDb);

    /**
     * High Shelf filter 1st-order (6 dB/oct).
     */
    void setHighShelfFirstOrder(float frequencyHz, float gainDb);

    /**
     * Processes a single sample on a given channel.
     */
    float processSample(int channel, float inputSample) noexcept;

    /**
     * Processes an entire AudioBuffer in place.
     */
    void processBlock(juce::AudioBuffer<float>& buffer) noexcept;

private:
    double currentSampleRate = 44100.0;
    int currentNumChannels = 0;

    // Filter Coefficients (double precision for stability)
    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;

    // Per-channel state variables (z1, z2 registers for DF2T)
    std::vector<double> z1;
    std::vector<double> z2;

    // Safety fallback: identity filter
    void setBypassCoefficients() noexcept
    {
        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
    }
};
