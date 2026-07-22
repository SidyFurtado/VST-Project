#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include <vector>

namespace LUMINAR::DSP
{
    class AirProcessor
    {
    public:
        AirProcessor() = default;

        void prepare (double newSampleRate, int maxBlockSize, int maxChannels);
        void reset();

        void process (juce::AudioBuffer<float>& buffer,
                      float presenceAmount,
                      float airAmount);

    private:
        struct HighPassBiquad
        {
            void setHighPass (double sampleRate, double frequencyHz, double q);
            void setBandPass (double sampleRate, double frequencyHz, double q);
            void reset() noexcept;
            float processSample (float input) noexcept;

            float b0 = 1.0f;
            float b1 = 0.0f;
            float b2 = 0.0f;
            float a1 = 0.0f;
            float a2 = 0.0f;
            float z1 = 0.0f;
            float z2 = 0.0f;
        };

        void updateFilterCoefficients();
        void ensureCapacity (int numChannels, int numSamples);

        static float mapDrive (float airAmount) noexcept;
        static float mapMix (float airAmount) noexcept;
        static float mapPresenceDrive (float presenceAmount) noexcept;
        static float mapPresenceMix (float presenceAmount) noexcept;
        static float mapTextureDrive (float textureAmount) noexcept;
        static float mapTextureMix (float textureAmount) noexcept;
        static float mapTextureAmount (float presenceAmount, float airAmount) noexcept;
        static float mapCompensationGain (float presenceAmount, float airAmount) noexcept;

        double sampleRate = 44100.0;
        int preparedBlockSize = 0;
        int preparedChannels = 0;

        juce::AudioBuffer<float> airBuffer;
        std::vector<float> airValues;
        std::vector<float> presenceValues;
        juce::SmoothedValue<float> airSmoother;
        juce::SmoothedValue<float> presenceSmoother;

        std::vector<HighPassBiquad> presenceBandFilters;
        std::vector<HighPassBiquad> textureBandFilters;
        std::vector<HighPassBiquad> preHighPassFilters;
        std::vector<HighPassBiquad> postHighPassFilters;
    };
}
