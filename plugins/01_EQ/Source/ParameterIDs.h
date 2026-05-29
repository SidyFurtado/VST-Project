#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace AUREQ::Params
{
    inline constexpr int numBands = 8;

    // Helper para validação de índice
    bool isValidBandIndex(int bandIndex);

    // Identificadores de Parâmetros Globais
    juce::String inputGainID();
    juce::String outputGainID();
    juce::String globalBypassID();
    juce::String themeModeID();

    // Identificadores de Parâmetros de Banda (bandIndex: 0 a 7)
    juce::String bandEnabledID(int bandIndex);
    juce::String bandTypeID(int bandIndex);
    juce::String bandFrequencyID(int bandIndex);
    juce::String bandGainID(int bandIndex);
    juce::String bandQID(int bandIndex);
    juce::String bandBypassID(int bandIndex);

    // Phase 5.19: Advanced band parameters
    juce::String bandSlopeID(int bandIndex);           // Cut slope: 12, 24, 48 dB/oct
    juce::String bandDynamicEnabledID(int bandIndex);  // Dynamic EQ on/off (DSP reserved)
    juce::String bandDynamicRangeID(int bandIndex);    // Dynamic range in dB (DSP reserved)
    juce::String bandDynamicThresholdID(int bandIndex);// Dynamic threshold dBFS (DSP reserved)
    juce::String bandDynamicAttackID(int bandIndex);   // Dynamic attack ms (DSP reserved)
    juce::String bandDynamicReleaseID(int bandIndex);  // Dynamic release ms (DSP reserved)
    juce::String bandChannelModeID(int bandIndex);     // Channel mode: Stereo/Mid/Side/L/R (DSP reserved)

    // Helpers de metadados
    juce::String bandDisplayName(int bandIndex);
    float defaultBandFrequency(int bandIndex);
}
