#include "BiquadFilter.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>

// Helper structure to describe test parameters
struct FilterTestParams
{
    double sampleRate;
    float frequency;
    float gain;
    float q;
};

// Check if a float value is valid (not NaN and not Inf)
static inline void checkValue(float val, const std::string& context)
{
    if (std::isnan(val))
        throw std::runtime_error("NaN detected in " + context);
    if (std::isinf(val))
        throw std::runtime_error("Infinite detected in " + context);
}

void runBiquadCoefficientTests()
{
    // Define the test vectors
    const std::vector<double> sampleRates = { 44100.0, 48000.0, 96000.0 };
    const std::vector<float> frequencies = { 20.0f, 50.0f, 1000.0f, 10000.0f, 20000.0f };
    const std::vector<float> Qs = { 0.1f, 0.707f, 1.0f, 10.0f, 18.0f };
    const std::vector<float> gains = { -24.0f, 0.0f, 24.0f };

    BiquadFilter filter;

    // Loop through all configurations
    for (double sr : sampleRates)
    {
        for (float freq : frequencies)
        {
            for (float q : Qs)
            {
                for (float gain : gains)
                {
                    // 1. Prepare filter for current sample rate (stereo)
                    filter.prepare(sr, 2);

                    std::string configStr = "SR=" + std::to_string(sr) +
                                            " Freq=" + std::to_string(freq) +
                                            " Q=" + std::to_string(q) +
                                            " Gain=" + std::to_string(gain);

                    // ==========================================
                    // Test PEAKING / BELL filter
                    // ==========================================
                    filter.reset();
                    filter.setPeaking(freq, gain, q);
                    // Process impulse response (1.0f, then silence)
                    float output = filter.processSample(0, 1.0f);
                    checkValue(output, "Bell Peaking (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "Bell Peaking (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence input
                    filter.reset();
                    filter.setPeaking(freq, gain, q);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("Bell Peaking did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test LOW CUT / HIGH PASS filter
                    // ==========================================
                    filter.reset();
                    filter.setHighPass(freq, q);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "Low Cut HPF (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "Low Cut HPF (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setHighPass(freq, q);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("Low Cut HPF did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test HIGH CUT / LOW PASS filter
                    // ==========================================
                    filter.reset();
                    filter.setLowPass(freq, q);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "High Cut LPF (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "High Cut LPF (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setLowPass(freq, q);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("High Cut LPF did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test LOW SHELF filter
                    // ==========================================
                    filter.reset();
                    filter.setLowShelf(freq, gain);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "Low Shelf (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "Low Shelf (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setLowShelf(freq, gain);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("Low Shelf did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test HIGH SHELF filter
                    // ==========================================
                    filter.reset();
                    filter.setHighShelf(freq, gain);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "High Shelf (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "High Shelf (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setHighShelf(freq, gain);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("High Shelf did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test NOTCH filter
                    // ==========================================
                    filter.reset();
                    filter.setNotch(freq, q);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "Notch (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "Notch (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setNotch(freq, q);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("Notch did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }

                    // ==========================================
                    // Test BAND PASS filter
                    // ==========================================
                    filter.reset();
                    filter.setBandPass(freq, q);
                    output = filter.processSample(0, 1.0f);
                    checkValue(output, "Band Pass (Impulse sample 0) with " + configStr);
                    for (int i = 0; i < 20; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        checkValue(output, "Band Pass (Tail sample " + std::to_string(i) + ") with " + configStr);
                    }
                    // Test silence
                    filter.reset();
                    filter.setBandPass(freq, q);
                    for (int i = 0; i < 100; ++i)
                    {
                        output = filter.processSample(0, 0.0f);
                        if (std::abs(output) > 1e-4f)
                            throw std::runtime_error("Band Pass did not stay silent: " + std::to_string(output) + " at " + configStr);
                    }
                }
            }
        }
    }
}
