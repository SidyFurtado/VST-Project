#include "BiquadFilter.h"
#include "EQBand.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <string>

// Simple deterministic pseudo-random number generator for white noise (LCG)
static float getDeterministicNoise()
{
    static unsigned int seed = 123456789;
    seed = seed * 1664525 + 1013904223;
    // Normalized to [-1.0, 1.0]
    return (static_cast<float>(seed) / 4294967295.0f) * 2.0f - 1.0f;
}

// Generate input signals in the buffer
static void generateSignal(juce::AudioBuffer<float>& buffer, int signalType, double sampleRate)
{
    const int numChans = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    buffer.clear();

    switch (signalType)
    {
        case 0: // Silence
            // Already cleared
            break;

        case 1: // Unit Impulse
            for (int ch = 0; ch < numChans; ++ch)
            {
                if (numSamples > 0)
                    buffer.setSample(ch, 0, 1.0f);
            }
            break;

        case 2: // 1 kHz Sine
            {
                const float radPerSample = 2.0f * juce::MathConstants<float>::pi * 1000.0f / static_cast<float>(sampleRate);
                for (int ch = 0; ch < numChans; ++ch)
                {
                    float* channelData = buffer.getWritePointer(ch);
                    for (int s = 0; s < numSamples; ++s)
                    {
                        channelData[s] = std::sin(static_cast<float>(s) * radPerSample);
                    }
                }
            }
            break;

        case 3: // Deterministic White Noise
            for (int ch = 0; ch < numChans; ++ch)
            {
                float* channelData = buffer.getWritePointer(ch);
                for (int s = 0; s < numSamples; ++s)
                {
                    channelData[s] = getDeterministicNoise();
                }
            }
            break;

        default:
            break;
    }
}

// Audit stability criteria
static void verifyBlockStability(const juce::AudioBuffer<float>& buffer, const std::string& context)
{
    const int numChans = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChans; ++ch)
    {
        const float* channelData = buffer.getReadPointer(ch);
        for (int s = 0; s < numSamples; ++s)
        {
            const float val = channelData[s];

            // 1. NaN checks
            if (std::isnan(val))
                throw std::runtime_error("NaN detected in output: " + context + " at sample " + std::to_string(s));

            // 2. Infinity checks
            if (std::isinf(val))
                throw std::runtime_error("Infinity detected in output: " + context + " at sample " + std::to_string(s));

            // 3. Prevent numerical explosion (absolute amplitude limit capped at 1000.0f)
            // Normal audio filters max gain is +24 dB (~16.0 gain multiplier). Output shouldn't blow up.
            if (std::abs(val) > 1000.0f)
                throw std::runtime_error("Numerical explosion detected in output (amplitude=" +
                                         std::to_string(val) + "): " + context + " at sample " + std::to_string(s));
        }
    }
}

void runFilterStabilityTests()
{
    const double sr = 48000.0;
    const int channels = 2;
    const int blockSize = 8192; // Large block size to check stability over time

    BiquadFilter filter;
    filter.prepare(sr, channels);

    juce::AudioBuffer<float> buffer(channels, blockSize);

    // List of signal names for clean exception reports
    const std::vector<std::string> signalNames = { "Silence", "Impulse", "1kHz Sine", "White Noise" };

    // Test extreme configurations
    // Extreme high resonance (Q=18.0), boost (+24 dB), and cutoffs (20 Hz, 20000 Hz)
    struct ExtremeConfig
    {
        float frequency;
        float gain;
        float q;
    };

    const std::vector<ExtremeConfig> configs = {
        { 20.0f,     24.0f,  18.0f },   // Extreme bass resonance
        { 1000.0f,   24.0f,  18.0f },   // Extreme mid resonance
        { 20000.0f,  24.0f,  18.0f },   // Extreme treble resonance
        { 1000.0f,  -24.0f,  18.0f },   // Extreme notch-like/attenuation Q
        { 50.0f,      0.0f,   0.707f }  // Butterworth-like baseline
    };

    // Loop through signals
    for (int signalType = 0; signalType < 4; ++signalType)
    {
        for (const auto& config : configs)
        {
            std::string configStr = "Signal=" + signalNames[static_cast<size_t>(signalType)] +
                                    " Freq=" + std::to_string(config.frequency) +
                                    " Q=" + std::to_string(config.q) +
                                    " Gain=" + std::to_string(config.gain);

            // ==========================================
            // Bell Filter Stability
            // ==========================================
            filter.reset();
            filter.setPeaking(config.frequency, config.gain, config.q);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "Bell - " + configStr);

            // ==========================================
            // Low Cut HPF Stability
            // ==========================================
            filter.reset();
            filter.setHighPass(config.frequency, config.q);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "Low Cut HPF - " + configStr);

            // ==========================================
            // High Cut LPF Stability
            // ==========================================
            filter.reset();
            filter.setLowPass(config.frequency, config.q);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "High Cut LPF - " + configStr);

            // ==========================================
            // Low Shelf Stability
            // ==========================================
            filter.reset();
            filter.setLowShelf(config.frequency, config.gain);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "Low Shelf - " + configStr);

            // ==========================================
            // High Shelf Stability
            // ==========================================
            filter.reset();
            filter.setHighShelf(config.frequency, config.gain);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "High Shelf - " + configStr);

            // ==========================================
            // Notch Stability
            // ==========================================
            filter.reset();
            filter.setNotch(config.frequency, config.q);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "Notch - " + configStr);

            // ==========================================
            // Band Pass Stability
            // ==========================================
            filter.reset();
            filter.setBandPass(config.frequency, config.q);
            generateSignal(buffer, signalType, sr);
            filter.processBlock(buffer);
            verifyBlockStability(buffer, "Band Pass - " + configStr);
        }
    }

    // Call EQBand Dynamic EQ stability checks
    std::cout << "[SUB-SUITE] Running EQBand Dynamic EQ stability sub-suite...\n";
    void runEQBandDynamicStabilityTests();
    runEQBandDynamicStabilityTests();
}

// ---------------------------------------------------------------------------
// Dynamic EQ Stability Tests (Phase 6.0)
// ---------------------------------------------------------------------------
void runEQBandDynamicStabilityTests()
{
    const double sr = 48000.0;
    const int channels = 2;
    const int blockSize = 8192;

    EQBand band;
    juce::AudioBuffer<float> buffer(channels, blockSize);
    const std::vector<std::string> signalNames = { "Silence", "Impulse", "1kHz Sine", "White Noise" };

    struct ExtremeDynConfig
    {
        int type;
        float frequency;
        float gain;
        float q;
        float dynRange;
        float dynThreshold;
        float dynAttack;
        float dynRelease;
    };

    const std::vector<ExtremeDynConfig> configs = {
        // Bell dynamic EQ (compression & expansion)
        { 0, 1000.0f,  6.0f, 1.0f, -12.0f, -50.0f,   0.1f,  2000.0f },
        { 0, 1000.0f,  0.0f, 1.0f,  12.0f, -50.0f, 200.0f,    10.0f },
        // Low Shelf dynamic EQ
        { 3,  100.0f,  3.0f, 0.707f, -6.0f, -30.0f,  10.0f,   100.0f },
        // High Shelf dynamic EQ
        { 4, 8000.0f,  0.0f, 0.707f,  6.0f, -40.0f,   5.0f,   500.0f },
        // Unsupported types (must bypass/ignore dynamic processing and stay safe)
        { 1,  200.0f,  0.0f, 0.707f, -12.0f, -50.0f,  1.0f,   100.0f }, // Low Cut HPF
        { 2, 5000.0f,  0.0f, 0.707f,  12.0f, -50.0f,  1.0f,   100.0f }, // High Cut LPF
        { 5, 1000.0f,  0.0f, 2.0f,   -12.0f, -50.0f,  1.0f,   100.0f }  // Notch
    };

    for (int signalType = 0; signalType < 4; ++signalType)
    {
        for (const auto& config : configs)
        {
            for (int modeVal = 0; modeVal < 5; ++modeVal)
            {
                band.initialize(0);
                band.prepare(sr, channels);
                band.setEnabled(true);
                band.setBypassed(false);
                band.setType(config.type);
                band.setFrequency(config.frequency);
                band.setGainDecibels(config.gain);
                band.setQ(config.q);
                band.setChannelMode(modeVal);
                
                // Set dynamic EQ active parameters
                band.setDynamicEnabled(true);
                band.setDynamicRange(config.dynRange);
                band.setDynamicThreshold(config.dynThreshold);
                band.setDynamicAttack(config.dynAttack);
                band.setDynamicRelease(config.dynRelease);

                std::string configStr = "DynEQ - Signal=" + signalNames[static_cast<size_t>(signalType)] +
                                        " Type=" + std::to_string(config.type) +
                                        " Freq=" + std::to_string(config.frequency) +
                                        " Range=" + std::to_string(config.dynRange) +
                                        " Threshold=" + std::to_string(config.dynThreshold) +
                                        " Mode=" + std::to_string(modeVal);

                generateSignal(buffer, signalType, sr);
                band.processBlock(buffer);
                verifyBlockStability(buffer, configStr);

                // Verify dynamicGainDb stability and boundary checks (Phase 6.2)
                float dynGain = band.getCurrentDynamicGainDb();
                if (std::isnan(dynGain) || std::isinf(dynGain))
                    throw std::runtime_error("getCurrentDynamicGainDb() returned NaN or Inf in stability test: " + configStr);
                if (std::abs(dynGain) > 24.05f)
                    throw std::runtime_error("getCurrentDynamicGainDb() exceeded range safety limits: " + std::to_string(dynGain) + " " + configStr);
            }
        }
    }
}
