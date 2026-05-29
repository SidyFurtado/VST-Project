#include "EQProcessorCore.h"
#include "../Source/ParameterIDs.h"
#include <cmath>

EQProcessorCore::EQProcessorCore()
{
    for (int i = 0; i < 8; ++i)
    {
        bands[static_cast<size_t>(i)].initialize(i);
    }
}

void EQProcessorCore::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    currentNumChannels = numChannels;

    for (auto& band : bands)
    {
        band.prepare(sampleRate, numChannels);
    }
}

void EQProcessorCore::reset()
{
    for (auto& band : bands)
    {
        band.reset();
    }
}

void EQProcessorCore::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts)
{
    // 1. Atualizar Parâmetros Globais
    if (auto* pInputGain = apvts.getRawParameterValue(AUREQ::Params::inputGainID()))
    {
        inputGain = pInputGain->load();
    }
    
    if (auto* pOutputGain = apvts.getRawParameterValue(AUREQ::Params::outputGainID()))
    {
        outputGain = pOutputGain->load();
    }

    if (auto* pGlobalBypass = apvts.getRawParameterValue(AUREQ::Params::globalBypassID()))
    {
        globalBypass = pGlobalBypass->load() > 0.5f;
    }

    if (auto* pThemeMode = apvts.getRawParameterValue(AUREQ::Params::themeModeID()))
    {
        themeMode = static_cast<int>(std::round(pThemeMode->load()));
    }

    // 2. Atualizar Parâmetros por Banda (0 a 7)
    for (int i = 0; i < 8; ++i)
    {
        if (auto* pEnabled = apvts.getRawParameterValue(AUREQ::Params::bandEnabledID(i)))
        {
            bands[static_cast<size_t>(i)].setEnabled(pEnabled->load() > 0.5f);
        }

        if (auto* pType = apvts.getRawParameterValue(AUREQ::Params::bandTypeID(i)))
        {
            bands[static_cast<size_t>(i)].setType(static_cast<int>(std::round(pType->load())));
        }

        if (auto* pFreq = apvts.getRawParameterValue(AUREQ::Params::bandFrequencyID(i)))
        {
            bands[static_cast<size_t>(i)].setFrequency(pFreq->load());
        }

        if (auto* pGain = apvts.getRawParameterValue(AUREQ::Params::bandGainID(i)))
        {
            bands[static_cast<size_t>(i)].setGainDecibels(pGain->load());
        }

        if (auto* pQ = apvts.getRawParameterValue(AUREQ::Params::bandQID(i)))
        {
            bands[static_cast<size_t>(i)].setQ(pQ->load());
        }

        if (auto* pBypass = apvts.getRawParameterValue(AUREQ::Params::bandBypassID(i)))
        {
            bands[static_cast<size_t>(i)].setBypassed(pBypass->load() > 0.5f);
        }

        // Phase 5.19: Read slope from APVTS (choice index 0=12, 1=24, 2=48 dB/oct)
        if (auto* pSlope = apvts.getRawParameterValue(AUREQ::Params::bandSlopeID(i)))
        {
            int slopeIndex = static_cast<int>(std::round(pSlope->load()));
            int slopeDbPerOct = 12;
            if (slopeIndex == 1) slopeDbPerOct = 24;
            else if (slopeIndex == 2) slopeDbPerOct = 48;
            bands[static_cast<size_t>(i)].setSlope(slopeDbPerOct);
        }

        // Phase 6.0: Read Dynamic EQ parameters from APVTS and pass to bands
        if (auto* pDynEnabled = apvts.getRawParameterValue(AUREQ::Params::bandDynamicEnabledID(i)))
        {
            bands[static_cast<size_t>(i)].setDynamicEnabled(pDynEnabled->load() > 0.5f);
        }

        if (auto* pDynRange = apvts.getRawParameterValue(AUREQ::Params::bandDynamicRangeID(i)))
        {
            bands[static_cast<size_t>(i)].setDynamicRange(pDynRange->load());
        }

        if (auto* pDynThreshold = apvts.getRawParameterValue(AUREQ::Params::bandDynamicThresholdID(i)))
        {
            bands[static_cast<size_t>(i)].setDynamicThreshold(pDynThreshold->load());
        }

        if (auto* pDynAttack = apvts.getRawParameterValue(AUREQ::Params::bandDynamicAttackID(i)))
        {
            bands[static_cast<size_t>(i)].setDynamicAttack(pDynAttack->load());
        }

        if (auto* pDynRelease = apvts.getRawParameterValue(AUREQ::Params::bandDynamicReleaseID(i)))
        {
            bands[static_cast<size_t>(i)].setDynamicRelease(pDynRelease->load());
        }

        // Read Channel Mode (Stereo/Mid/Side/Left/Right) from APVTS (Phase 7.0)
        if (auto* pChannelMode = apvts.getRawParameterValue(AUREQ::Params::bandChannelModeID(i)))
        {
            bands[static_cast<size_t>(i)].setChannelMode(static_cast<int>(std::round(pChannelMode->load())));
        }
    }
}

void EQProcessorCore::processBlock(juce::AudioBuffer<float>& buffer)
{
    // 1. Global Bypass Check
    if (globalBypass)
    {
        return;
    }

    // 2. Input/Output Gain preparation
    float inGainDb = inputGain;
    float outGainDb = outputGain;

    // Sanitize DB values
    if (std::isnan(inGainDb) || std::isinf(inGainDb))
    {
        inGainDb = 0.0f;
    }
    if (std::isnan(outGainDb) || std::isinf(outGainDb))
    {
        outGainDb = 0.0f;
    }

    // Convert to linear gains
    float inputGainLinear = juce::Decibels::decibelsToGain(inGainDb);
    float outputGainLinear = juce::Decibels::decibelsToGain(outGainDb);

    // Sanitize linear gains
    if (std::isnan(inputGainLinear) || std::isinf(inputGainLinear) || inputGainLinear < 0.0f)
    {
        inputGainLinear = 1.0f;
    }
    if (std::isnan(outputGainLinear) || std::isinf(outputGainLinear) || outputGainLinear < 0.0f)
    {
        outputGainLinear = 1.0f;
    }

    // 3. Apply Input Gain
    buffer.applyGain(inputGainLinear);

    // 4. Process all 8 bands in cascade sequence
    // The EQBand::processBlock checks internally if enabled, type == Bell, etc.
    for (auto& band : bands)
    {
        band.processBlock(buffer);
    }

    // 5. Numerical safety guard: sanitize samples in buffer to prevent NaNs/Infs
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float s = channelData[sample];
            if (std::isnan(s) || std::isinf(s))
            {
                channelData[sample] = 0.0f;
            }
        }
    }

    // 6. Apply Output Gain
    buffer.applyGain(outputGainLinear);
}

float EQProcessorCore::getBandDynamicGainDb(int bandIndex) const
{
    if (bandIndex >= 0 && bandIndex < 8)
    {
        return bands[static_cast<size_t>(bandIndex)].getCurrentDynamicGainDb();
    }
    return 0.0f;
}

