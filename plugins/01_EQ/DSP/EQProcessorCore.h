#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include "EQBand.h"
#include "../Source/ParameterIDs.h"

class EQProcessorCore
{
public:
    EQProcessorCore();
    ~EQProcessorCore() = default;

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts);
    void processBlock(juce::AudioBuffer<float>& buffer);
    float getBandDynamicGainDb(int bandIndex) const;

    const std::array<EQBand, AUREQ::Params::numBands>& getBands() const { return bands; }
    
    float getInputGain() const { return inputGain; }
    float getOutputGain() const { return outputGain; }
    bool isGloballyBypassed() const { return globalBypass; }
    int getThemeMode() const { return themeMode; }

private:
    std::array<EQBand, AUREQ::Params::numBands> bands;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 0;
    int currentNumChannels = 0;

    // Parâmetros Globais
    float inputGain = 0.0f;
    float outputGain = 0.0f;
    bool globalBypass = false;
    int themeMode = 1; // Default: Dark (index 1)
};

