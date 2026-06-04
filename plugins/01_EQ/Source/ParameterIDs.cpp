#include "ParameterIDs.h"

namespace AUREQ::Params
{
    bool isValidBandIndex(int bandIndex)
    {
        return bandIndex >= 0 && bandIndex < numBands;
    }

    juce::String inputGainID()      { return "inputGain"; }
    juce::String outputGainID()     { return "outputGain"; }
    juce::String globalBypassID()   { return "globalBypass"; }
    juce::String themeModeID()      { return "themeMode"; }

    juce::String bandEnabledID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Enabled";
    }

    juce::String bandTypeID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Type";
    }

    juce::String bandFrequencyID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Frequency";
    }

    juce::String bandGainID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Gain";
    }

    juce::String bandQID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Q";
    }

    juce::String bandBypassID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Bypass";
    }

    // Phase 5.19: Advanced band parameters

    juce::String bandSlopeID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "Slope";
    }

    juce::String bandDynamicEnabledID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "DynamicEnabled";
    }

    juce::String bandDynamicRangeID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "DynamicRange";
    }

    juce::String bandDynamicThresholdID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "DynamicThreshold";
    }

    juce::String bandDynamicAttackID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "DynamicAttack";
    }

    juce::String bandDynamicReleaseID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "DynamicRelease";
    }

    juce::String bandChannelModeID(int bandIndex)
    {
        jassert(isValidBandIndex(bandIndex));
        return "band" + juce::String(bandIndex + 1) + "ChannelMode";
    }

    juce::String bandDisplayName(int bandIndex)
    {
        if (!isValidBandIndex(bandIndex))
            return {};
        return "Band " + juce::String(bandIndex + 1);
    }

    float defaultBandFrequency(int bandIndex)
    {
        switch (bandIndex)
        {
            case 0: return 80.0f;
            case 1: return 160.0f;
            case 2: return 315.0f;
            case 3: return 630.0f;
            case 4: return 1250.0f;
            case 5: return 2500.0f;
            case 6: return 5000.0f;
            case 7: return 10000.0f;
            case 8: return 30.0f;     // Sub/Low
            case 9: return 400.0f;    // Low Mid
            case 10: return 800.0f;   // Mid
            case 11: return 15000.0f; // High Air
            default: break;
        }
        return 1000.0f;
    }
}
