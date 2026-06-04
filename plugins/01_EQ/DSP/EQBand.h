#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "BiquadFilter.h"
#include "ParameterSmoother.h"
#include <array>

class EQBand
{
public:
    enum class FilterType
    {
        Bell = 0,
        LowCut,
        HighCut,
        LowShelf,
        HighShelf,
        Notch,
        BandPass
    };

    EQBand() = default;
    ~EQBand() = default;

    void initialize(int index);
    void prepare(double sampleRate, int numChannels);
    void reset();

    void setEnabled(bool enabled);
    void setBypassed(bool bypassed);
    void setType(int typeIndex);
    void setFrequency(float freq);
    void setGainDecibels(float gainDb);
    void setQ(float qVal);
    void setSlope(int slopeDbPerOct);

    // Dynamic EQ Setters (Phase 6.0)
    void setDynamicEnabled(bool enabled);
    void setDynamicRange(float rangeDb);
    void setDynamicThreshold(float thresholdDb);
    void setDynamicAttack(float attackMs);
    void setDynamicRelease(float releaseMs);

    float getCurrentDynamicGainDb() const;

    void setChannelMode(int mode);
    int getChannelMode() const { return channelMode; }

    int getBandIndex() const { return bandIndex; }
    bool isEnabled() const { return enabled; }
    bool isBypassed() const { return bypassed; }
    FilterType getType() const { return type; }
    float getFrequency() const { return frequency; }
    float getGainDecibels() const { return gainDecibels; }
    float getQ() const { return q; }
    int getSlope() const { return slope; }

    void processBlock(juce::AudioBuffer<float>& buffer) noexcept;

private:
    void updateFilterCoefficients();
    void updateFilterCoefficients(float f, float g, float qVal);
    void updateEnvelopeCoefficients();

    int bandIndex = 0;
    double sampleRate = 44100.0;
    bool enabled = false;
    bool bypassed = false;
    FilterType type = FilterType::Bell;
    float frequency = 1000.0f;
    float gainDecibels = 0.0f;
    float q = 1.0f;
    int slope = 12;
    int channelMode = 0; // 0 = Stereo, 1 = Mid, 2 = Side, 3 = Left, 4 = Right

    // Dynamic EQ configuration and envelope state (Phase 6.0)
    bool dynamicEnabled = false;
    float dynamicRange = 0.0f;
    float dynamicThreshold = -24.0f;
    float dynamicAttack = 10.0f;
    float dynamicRelease = 120.0f;
    std::atomic<float> currentDynamicGainDb { 0.0f };

    float envelopeState = 0.0f;
    float attackCoeff = 0.99f;
    float releaseCoeff = 0.99f;

    BiquadFilter filter;
    std::array<BiquadFilter, 1> extraFilters;
    ParameterSmoother freqSmoother;
    ParameterSmoother gainSmoother;
    ParameterSmoother qSmoother;
};
