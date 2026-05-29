#include "EQBand.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <string>

// Helper to fill buffer with simple sine wave
static void fillWithSine(juce::AudioBuffer<float>& buffer, float freqHz, double sampleRate)
{
    const float radPerSample = 2.0f * juce::MathConstants<float>::pi * freqHz / static_cast<float>(sampleRate);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = std::sin(static_cast<float>(sample) * radPerSample);
        }
    }
}

// Compare two audio buffers, throw if differences found
static void assertBuffersEqual(const juce::AudioBuffer<float>& b1, const juce::AudioBuffer<float>& b2, const std::string& context)
{
    if (b1.getNumChannels() != b2.getNumChannels() || b1.getNumSamples() != b2.getNumSamples())
        throw std::runtime_error("Buffer dimensions mismatch in comparison: " + context);

    for (int ch = 0; ch < b1.getNumChannels(); ++ch)
    {
        const float* d1 = b1.getReadPointer(ch);
        const float* d2 = b2.getReadPointer(ch);
        for (int s = 0; s < b1.getNumSamples(); ++s)
        {
            if (std::abs(d1[s] - d2[s]) > 1e-7f)
            {
                throw std::runtime_error("Buffers differ at ch=" + std::to_string(ch) +
                                         " sample=" + std::to_string(s) +
                                         " diff=" + std::to_string(std::abs(d1[s] - d2[s])) +
                                         " context: " + context);
            }
        }
    }
}

// Verify that buffer contains no NaNs/Infs
static void assertBufferFinite(const juce::AudioBuffer<float>& buffer, const std::string& context)
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        const float* d = buffer.getReadPointer(ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            if (std::isnan(d[s]))
                throw std::runtime_error("NaN sample detected at ch=" + std::to_string(ch) + " sample=" + std::to_string(s) + " context: " + context);
            if (std::isinf(d[s]))
                throw std::runtime_error("Inf sample detected at ch=" + std::to_string(ch) + " sample=" + std::to_string(s) + " context: " + context);
        }
    }
}

void runEQBandProcessingTests()
{
    const std::vector<double> sampleRates = { 44100.0, 96000.0 };
    const std::vector<int> channelCounts = { 1, 2 }; // Mono, Stereo
    const std::vector<int> blockSizes = { 64, 512, 2048 };
    const std::vector<int> slopes = { 12, 24, 48 };

    EQBand band;

    // Test 1: Initialize, prepare and reset sanity checks
    band.initialize(3); // Band index 3 (corresponds to Band 4)
    if (band.getBandIndex() != 3)
        throw std::runtime_error("EQBand failed to retain initialized index.");

    // Loop through sample rates, channels and block sizes
    for (double sr : sampleRates)
    {
        for (int channels : channelCounts)
        {
            for (int blockSize : blockSizes)
            {
                std::string contextStr = "SR=" + std::to_string(sr) +
                                         " Chans=" + std::to_string(channels) +
                                         " BlockSize=" + std::to_string(blockSize);

                // Prepare band
                band.prepare(sr, channels);
                band.setEnabled(true);
                band.setBypassed(false);

                // Create buffers
                juce::AudioBuffer<float> mainBuffer(channels, blockSize);
                juce::AudioBuffer<float> originalCopy(channels, blockSize);

                // ==================================================
                // Test Pass-through when Disabled
                // ==================================================
                fillWithSine(mainBuffer, 440.0f, sr);
                originalCopy.makeCopyOf(mainBuffer);

                band.setEnabled(false);
                band.processBlock(mainBuffer);
                assertBuffersEqual(mainBuffer, originalCopy, "Disabled band pass-through - " + contextStr);

                // ==================================================
                // Test Pass-through when Bypassed
                // ==================================================
                band.setEnabled(true);
                band.setBypassed(true);
                fillWithSine(mainBuffer, 440.0f, sr);
                originalCopy.makeCopyOf(mainBuffer);

                band.processBlock(mainBuffer);
                assertBuffersEqual(mainBuffer, originalCopy, "Bypassed band pass-through - " + contextStr);

                // Restore active state
                band.setBypassed(false);

                // ==================================================
                // Test all 7 filter types processing
                // ==================================================
                for (int typeIdx = 0; typeIdx < 7; ++typeIdx)
                {
                    band.setType(typeIdx);
                    band.setFrequency(1000.0f);
                    band.setGainDecibels(6.0f);
                    band.setQ(1.0f);

                    fillWithSine(mainBuffer, 440.0f, sr);
                    band.processBlock(mainBuffer);
                    assertBufferFinite(mainBuffer, "FilterType " + std::to_string(typeIdx) + " processing - " + contextStr);
                }

                // ==================================================
                // Test Low Cut & High Cut slopes (12 / 24 / 48 dB/oct)
                // ==================================================
                const std::vector<int> cutTypes = { 1, 2 }; // LowCut = 1, HighCut = 2
                for (int cutType : cutTypes)
                {
                    band.setType(cutType);
                    band.setFrequency(200.0f);
                    band.setQ(0.707f);

                    for (int slopeVal : slopes)
                    {
                        band.setSlope(slopeVal);

                        fillWithSine(mainBuffer, 100.0f, sr);
                        band.processBlock(mainBuffer);

                        assertBufferFinite(mainBuffer, "CutType " + std::to_string(cutType) +
                                                      " Slope " + std::to_string(slopeVal) +
                                                      " processing - " + contextStr);
                    }
                }
                // ==================================================
                // Test Dynamic EQ Core (Phase 6.0)
                // ==================================================
                // Test on a Bell band
                band.setEnabled(true);
                band.setBypassed(false);
                band.setType(0); // Bell
                band.setFrequency(1000.0f);
                band.setGainDecibels(6.0f);
                band.setQ(1.0f);
                band.prepare(sr, channels); // Force smoothers to target values immediately (no ramps)

                // Scenario A: dynamicEnabled = false (control/baseline)
                band.setDynamicEnabled(false);
                band.reset(); // Reset filter delay states to clean zero
                fillWithSine(mainBuffer, 1000.0f, sr); // Signal on center frequency
                juce::AudioBuffer<float> controlBuffer(channels, blockSize);
                controlBuffer.makeCopyOf(mainBuffer);
                band.processBlock(controlBuffer);
                assertBufferFinite(controlBuffer, "Dynamic EQ Control (Off) - " + contextStr);

                // Scenario B: dynamicEnabled = true, but range = 0.0 dB (must be identical to baseline)
                band.setDynamicEnabled(true);
                band.setDynamicRange(0.0f);
                band.setDynamicThreshold(-24.0f);
                band.setDynamicAttack(10.0f);
                band.setDynamicRelease(120.0f);
                band.reset(); // Reset filter delay states to clean zero

                juce::AudioBuffer<float> rangeZeroBuffer(channels, blockSize);
                rangeZeroBuffer.makeCopyOf(mainBuffer);
                band.processBlock(rangeZeroBuffer);
                assertBuffersEqual(rangeZeroBuffer, controlBuffer, "Dynamic EQ Range=0 (Neutro) - " + contextStr);

                // Scenario C: dynamicEnabled = true, range positive (boosting)
                band.setDynamicRange(12.0f);
                band.setDynamicThreshold(-50.0f); // very sensitive to trigger easily
                band.setDynamicAttack(0.1f);      // extreme fast attack
                band.setDynamicRelease(2000.0f);   // extreme slow release
                band.reset(); // Reset filter delay states to clean zero

                juce::AudioBuffer<float> boostBuffer(channels, blockSize);
                boostBuffer.makeCopyOf(mainBuffer);
                band.processBlock(boostBuffer);
                assertBufferFinite(boostBuffer, "Dynamic EQ Boost - " + contextStr);

                // Scenario D: dynamicEnabled = true, range negative (attenuation)
                band.setDynamicRange(-12.0f);
                band.setDynamicThreshold(-50.0f);
                band.reset(); // Reset filter delay states to clean zero

                juce::AudioBuffer<float> cutBuffer(channels, blockSize);
                cutBuffer.makeCopyOf(mainBuffer);
                band.processBlock(cutBuffer);
                assertBufferFinite(cutBuffer, "Dynamic EQ Attenuate - " + contextStr);

                // Scenario E: check extreme thresholds and time constants
                band.setDynamicRange(6.0f);
                band.setDynamicThreshold(0.0f); // high threshold
                band.setDynamicAttack(200.0f);  // extreme slow attack
                band.setDynamicRelease(10.0f);  // extreme fast release
                band.reset(); // Reset filter delay states to clean zero

                juce::AudioBuffer<float> extremeBuffer(channels, blockSize);
                extremeBuffer.makeCopyOf(mainBuffer);
                band.processBlock(extremeBuffer);
                assertBufferFinite(extremeBuffer, "Dynamic EQ Extreme Params - " + contextStr);

                // Scenario F: Support validation (Bell, LowShelf, HighShelf are dynamic)
                // Notch/LowCut/HighCut/BandPass must ignore dynamic EQ (output with dyn active = output with dyn inactive)
                const std::vector<int> unsupportedTypes = { 1, 2, 5, 6 }; // LowCut, HighCut, Notch, BandPass
                for (int unType : unsupportedTypes)
                {
                    band.setType(unType);
                    band.setFrequency(1000.0f);
                    band.setQ(0.707f);
                    band.prepare(sr, channels); // Settle smoothers

                    // Run baseline (dyn off)
                    band.setDynamicEnabled(false);
                    band.reset(); // Reset filter delay states to clean zero
                    fillWithSine(mainBuffer, 1000.0f, sr);
                    juce::AudioBuffer<float> staticOut(channels, blockSize);
                    staticOut.makeCopyOf(mainBuffer);
                    band.processBlock(staticOut);

                    // Run with active dynamic EQ parameters (must ignore it)
                    band.setDynamicEnabled(true);
                    band.setDynamicRange(-12.0f);
                    band.setDynamicThreshold(-50.0f);
                    band.reset(); // Reset filter delay states to clean zero

                    juce::AudioBuffer<float> dynamicOut(channels, blockSize);
                    dynamicOut.makeCopyOf(mainBuffer);
                    band.processBlock(dynamicOut);

                    assertBuffersEqual(dynamicOut, staticOut, "Unsupported filter type " + std::to_string(unType) + " ignored dynamic EQ - " + contextStr);
                }
            }
        }
    }

    // ==================================================
    // Test getCurrentDynamicGainDb() Behavior (Phase 6.2)
    // ==================================================
    {
        band.initialize(0);
        band.prepare(44100.0, 2);
        band.setEnabled(true);
        band.setBypassed(false);
        band.setType(0); // Bell (supported)
        band.setFrequency(1000.0f);
        band.setGainDecibels(0.0f);
        band.setQ(1.0f);
        
        // Disabled dynamic EQ should return 0.0f
        band.setDynamicEnabled(false);
        band.setDynamicRange(6.0f);
        band.setDynamicThreshold(-50.0f);
        juce::AudioBuffer<float> testBuf(2, 512);
        fillWithSine(testBuf, 1000.0f, 44100.0);
        band.processBlock(testBuf);
        if (std::abs(band.getCurrentDynamicGainDb()) > 1e-5f)
            throw std::runtime_error("getCurrentDynamicGainDb() not 0.0f when dynamicEnabled is false");

        // Range = 0.0f should return 0.0f
        band.setDynamicEnabled(true);
        band.setDynamicRange(0.0f);
        band.reset();
        fillWithSine(testBuf, 1000.0f, 44100.0);
        band.processBlock(testBuf);
        if (std::abs(band.getCurrentDynamicGainDb()) > 1e-5f)
            throw std::runtime_error("getCurrentDynamicGainDb() not 0.0f when dynamicRange is 0.0f");

        // Unsupported type (e.g. LowCut = 1) should return 0.0f
        band.setType(1); // LowCut
        band.setDynamicRange(6.0f);
        band.reset();
        fillWithSine(testBuf, 1000.0f, 44100.0);
        band.processBlock(testBuf);
        if (std::abs(band.getCurrentDynamicGainDb()) > 1e-5f)
            throw std::runtime_error("getCurrentDynamicGainDb() not 0.0f for unsupported filter type");

        // Positive range with signal exceeding threshold should return >= 0.0f
        band.setType(0); // Bell
        band.setDynamicRange(6.0f);
        band.setDynamicThreshold(-50.0f);
        band.setDynamicAttack(0.1f);
        band.setDynamicRelease(1000.0f);
        band.reset();
        fillWithSine(testBuf, 1000.0f, 44100.0); // Sine amplitude is 1.0 (0 dBFS), way above threshold (-50 dBFS)
        band.processBlock(testBuf);
        float gainVal = band.getCurrentDynamicGainDb();
        if (std::isnan(gainVal) || std::isinf(gainVal))
            throw std::runtime_error("getCurrentDynamicGainDb() returned NaN or Inf");
        if (gainVal <= 0.0f)
            throw std::runtime_error("getCurrentDynamicGainDb() did not return positive value for positive range under boost condition");

        // Negative range with signal exceeding threshold should return <= 0.0f
        band.setDynamicRange(-6.0f);
        band.reset();
        fillWithSine(testBuf, 1000.0f, 44100.0);
        band.processBlock(testBuf);
        gainVal = band.getCurrentDynamicGainDb();
        if (std::isnan(gainVal) || std::isinf(gainVal))
            throw std::runtime_error("getCurrentDynamicGainDb() returned NaN or Inf");
        if (gainVal >= 0.0f)
            throw std::runtime_error("getCurrentDynamicGainDb() did not return negative value for negative range under cut condition");
    }

    // ==================================================
    // Test Channel Mode Behavior (Phase 7.0)
    // ==================================================
    {
        band.initialize(0);
        band.prepare(44100.0, 2);
        band.setEnabled(true);
        band.setBypassed(false);
        band.setType(0); // Bell
        band.setFrequency(1000.0f);
        band.setGainDecibels(12.0f); // High gain so difference is obvious
        band.setQ(1.0f);
        band.setDynamicEnabled(false);

        // 1. Stereo (0) - processes both channels
        {
            band.setChannelMode(0);
            juce::AudioBuffer<float> testBuf(2, 512);
            fillWithSine(testBuf, 1000.0f, 44100.0);
            juce::AudioBuffer<float> originalCopy(2, 512);
            originalCopy.makeCopyOf(testBuf);

            band.processBlock(testBuf);
            assertBufferFinite(testBuf, "Stereo channel mode");
            
            // Both channels should be modified (should not be equal to original)
            bool ch0Changed = false;
            bool ch1Changed = false;
            for (int s = 0; s < 512; ++s)
            {
                if (std::abs(testBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) ch0Changed = true;
                if (std::abs(testBuf.getSample(1, s) - originalCopy.getSample(1, s)) > 1e-4f) ch1Changed = true;
            }
            if (!ch0Changed || !ch1Changed)
                throw std::runtime_error("Stereo mode did not process both channels");
        }

        // 2. Left (3) - processes left only
        {
            band.setChannelMode(3);
            juce::AudioBuffer<float> testBuf(2, 512);
            fillWithSine(testBuf, 1000.0f, 44100.0);
            juce::AudioBuffer<float> originalCopy(2, 512);
            originalCopy.makeCopyOf(testBuf);

            band.processBlock(testBuf);
            assertBufferFinite(testBuf, "Left channel mode");

            // Left channel (0) must change, Right (1) must remain identical
            bool ch0Changed = false;
            bool ch1Changed = false;
            for (int s = 0; s < 512; ++s)
            {
                if (std::abs(testBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) ch0Changed = true;
                if (std::abs(testBuf.getSample(1, s) - originalCopy.getSample(1, s)) > 1e-7f) ch1Changed = true;
            }
            if (!ch0Changed)
                throw std::runtime_error("Left mode did not alter Left channel");
            if (ch1Changed)
                throw std::runtime_error("Left mode altered Right channel");
        }

        // 3. Right (4) - processes right only
        {
            band.setChannelMode(4);
            juce::AudioBuffer<float> testBuf(2, 512);
            fillWithSine(testBuf, 1000.0f, 44100.0);
            juce::AudioBuffer<float> originalCopy(2, 512);
            originalCopy.makeCopyOf(testBuf);

            band.processBlock(testBuf);
            assertBufferFinite(testBuf, "Right channel mode");

            // Right channel (1) must change, Left (0) must remain identical
            bool ch0Changed = false;
            bool ch1Changed = false;
            for (int s = 0; s < 512; ++s)
            {
                if (std::abs(testBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-7f) ch0Changed = true;
                if (std::abs(testBuf.getSample(1, s) - originalCopy.getSample(1, s)) > 1e-4f) ch1Changed = true;
            }
            if (ch0Changed)
                throw std::runtime_error("Right mode altered Left channel");
            if (!ch1Changed)
                throw std::runtime_error("Right mode did not alter Right channel");
        }

        // 4. Mid (1) - L = R input (so S = 0).
        // Since input is pure mono (L = R), Mid = L, Side = 0.
        // Mid mode filters Mid, so L_new = M_filtered, R_new = M_filtered.
        // Therefore, output should be L_new = R_new, both filtered.
        {
            band.setChannelMode(1);
            juce::AudioBuffer<float> testBuf(2, 512);
            fillWithSine(testBuf, 1000.0f, 44100.0);
            // Verify L = R
            for (int s = 0; s < 512; ++s)
                testBuf.setSample(1, s, testBuf.getSample(0, s));
            
            juce::AudioBuffer<float> originalCopy(2, 512);
            originalCopy.makeCopyOf(testBuf);

            band.processBlock(testBuf);
            assertBufferFinite(testBuf, "Mid channel mode");

            // Check that both channels changed and are identical (L = R)
            bool ch0Changed = false;
            bool channelsDiffer = false;
            for (int s = 0; s < 512; ++s)
            {
                if (std::abs(testBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) ch0Changed = true;
                if (std::abs(testBuf.getSample(0, s) - testBuf.getSample(1, s)) > 1e-6f) channelsDiffer = true;
            }
            if (!ch0Changed)
                throw std::runtime_error("Mid mode did not filter mono signal");
            if (channelsDiffer)
                throw std::runtime_error("Mid mode caused mono signal to become stereo");
        }

        // 5. Side (2) - L = -R input (so M = 0).
        // Since input is pure out-of-phase (L = -R), Mid = 0, Side = L.
        // Side mode filters Side, so L_new = S_filtered, R_new = -S_filtered.
        // Therefore, output should be L_new = -R_new, both filtered.
        {
            band.setChannelMode(2);
            juce::AudioBuffer<float> testBuf(2, 512);
            fillWithSine(testBuf, 1000.0f, 44100.0);
            // Verify L = -R
            for (int s = 0; s < 512; ++s)
                testBuf.setSample(1, s, -testBuf.getSample(0, s));
            
            juce::AudioBuffer<float> originalCopy(2, 512);
            originalCopy.makeCopyOf(testBuf);

            band.processBlock(testBuf);
            assertBufferFinite(testBuf, "Side channel mode");

            // Check that both channels changed and remain out-of-phase (L = -R)
            bool ch0Changed = false;
            bool phaseDiffer = false;
            for (int s = 0; s < 512; ++s)
            {
                if (std::abs(testBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) ch0Changed = true;
                if (std::abs(testBuf.getSample(0, s) + testBuf.getSample(1, s)) > 1e-6f) phaseDiffer = true;
            }
            if (!ch0Changed)
                throw std::runtime_error("Side mode did not filter out-of-phase signal");
            if (phaseDiffer)
                throw std::runtime_error("Side mode altered phase relationship L = -R");
        }

        // 6. Mono Fallback
        {
            juce::AudioBuffer<float> monoBuf(1, 512);
            fillWithSine(monoBuf, 1000.0f, 44100.0);
            juce::AudioBuffer<float> originalCopy(1, 512);
            originalCopy.makeCopyOf(monoBuf);

            // Stereo (0) processes mono
            band.setChannelMode(0);
            band.reset();
            monoBuf.makeCopyOf(originalCopy);
            band.processBlock(monoBuf);
            assertBufferFinite(monoBuf, "Mono fallback Stereo");
            bool changed = false;
            for (int s = 0; s < 512; ++s)
                if (std::abs(monoBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) changed = true;
            if (!changed)
                throw std::runtime_error("Mono fallback Stereo did not process");

            // Left (3) processes mono
            band.setChannelMode(3);
            band.reset();
            monoBuf.makeCopyOf(originalCopy);
            band.processBlock(monoBuf);
            assertBufferFinite(monoBuf, "Mono fallback Left");
            changed = false;
            for (int s = 0; s < 512; ++s)
                if (std::abs(monoBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) changed = true;
            if (!changed)
                throw std::runtime_error("Mono fallback Left did not process");

            // Mid (1) processes mono (Stereo fallback)
            band.setChannelMode(1);
            band.reset();
            monoBuf.makeCopyOf(originalCopy);
            band.processBlock(monoBuf);
            assertBufferFinite(monoBuf, "Mono fallback Mid");
            changed = false;
            for (int s = 0; s < 512; ++s)
                if (std::abs(monoBuf.getSample(0, s) - originalCopy.getSample(0, s)) > 1e-4f) changed = true;
            if (!changed)
                throw std::runtime_error("Mono fallback Mid did not process");

            // Right (4) bypasses mono (fails safely, no modification)
            band.setChannelMode(4);
            band.reset();
            monoBuf.makeCopyOf(originalCopy);
            band.processBlock(monoBuf);
            assertBufferFinite(monoBuf, "Mono fallback Right");
            assertBuffersEqual(monoBuf, originalCopy, "Mono fallback Right bypass check");

            // Side (2) bypasses mono (fails safely, no modification)
            band.setChannelMode(2);
            band.reset();
            monoBuf.makeCopyOf(originalCopy);
            band.processBlock(monoBuf);
            assertBufferFinite(monoBuf, "Mono fallback Side");
            assertBuffersEqual(monoBuf, originalCopy, "Mono fallback Side bypass check");
        }

        // 7. Dynamic EQ + Channel Mode combination (Bell/LowShelf/HighShelf)
        {
            band.initialize(0);
            band.prepare(44100.0, 2);
            band.setEnabled(true);
            band.setBypassed(false);
            band.setType(0); // Bell
            band.setFrequency(1000.0f);
            band.setGainDecibels(0.0f);
            band.setQ(1.0f);

            band.setDynamicEnabled(true);
            band.setDynamicRange(-6.0f);
            band.setDynamicThreshold(-50.0f);
            band.setDynamicAttack(0.1f);
            band.setDynamicRelease(100.0f);

            const std::vector<int> modes = { 0, 1, 2, 3, 4 };
            juce::AudioBuffer<float> testBuf(2, 512);
            for (int mode : modes)
            {
                band.setChannelMode(mode);
                band.reset();
                fillWithSine(testBuf, 1000.0f, 44100.0);
                band.processBlock(testBuf);
                assertBufferFinite(testBuf, "Dynamic EQ + Channel Mode " + std::to_string(mode));
                float dynGain = band.getCurrentDynamicGainDb();
                if (std::isnan(dynGain) || std::isinf(dynGain))
                    throw std::runtime_error("getCurrentDynamicGainDb() returned NaN/Inf under dynamic channel mode test");
            }
        }
    }
}
