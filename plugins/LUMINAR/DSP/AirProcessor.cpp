#include "AirProcessor.h"
#include "AirWaveshaper.h"

#include <algorithm>
#include <cmath>

namespace LUMINAR::DSP
{
    void AirProcessor::HighPassBiquad::setHighPass (double sr, double frequencyHz, double q)
    {
        const auto safeSampleRate = std::max (sr, 1.0);
        const auto nyquist = safeSampleRate * 0.5;
        const auto frequency = std::clamp (frequencyHz, 20.0, nyquist * 0.45);
        const auto safeQ = std::max (q, 0.1);

        const auto omega = juce::MathConstants<double>::twoPi * frequency / safeSampleRate;
        const auto sinOmega = std::sin (omega);
        const auto cosOmega = std::cos (omega);
        const auto alpha = sinOmega / (2.0 * safeQ);

        const auto rawB0 =  (1.0 + cosOmega) * 0.5;
        const auto rawB1 = -(1.0 + cosOmega);
        const auto rawB2 =  (1.0 + cosOmega) * 0.5;
        const auto rawA0 =   1.0 + alpha;
        const auto rawA1 =  -2.0 * cosOmega;
        const auto rawA2 =   1.0 - alpha;

        b0 = static_cast<float> (rawB0 / rawA0);
        b1 = static_cast<float> (rawB1 / rawA0);
        b2 = static_cast<float> (rawB2 / rawA0);
        a1 = static_cast<float> (rawA1 / rawA0);
        a2 = static_cast<float> (rawA2 / rawA0);
    }

    void AirProcessor::HighPassBiquad::setBandPass (double sr, double frequencyHz, double q)
    {
        const auto safeSampleRate = std::max (sr, 1.0);
        const auto nyquist = safeSampleRate * 0.5;
        const auto frequency = std::clamp (frequencyHz, 20.0, nyquist * 0.45);
        const auto safeQ = std::max (q, 0.1);

        const auto omega = juce::MathConstants<double>::twoPi * frequency / safeSampleRate;
        const auto sinOmega = std::sin (omega);
        const auto cosOmega = std::cos (omega);
        const auto alpha = sinOmega / (2.0 * safeQ);

        const auto rawB0 =   alpha;
        const auto rawB1 =   0.0;
        const auto rawB2 =  -alpha;
        const auto rawA0 =   1.0 + alpha;
        const auto rawA1 =  -2.0 * cosOmega;
        const auto rawA2 =   1.0 - alpha;

        b0 = static_cast<float> (rawB0 / rawA0);
        b1 = static_cast<float> (rawB1 / rawA0);
        b2 = static_cast<float> (rawB2 / rawA0);
        a1 = static_cast<float> (rawA1 / rawA0);
        a2 = static_cast<float> (rawA2 / rawA0);
    }

    void AirProcessor::HighPassBiquad::reset() noexcept
    {
        z1 = 0.0f;
        z2 = 0.0f;
    }

    float AirProcessor::HighPassBiquad::processSample (float input) noexcept
    {
        if (! std::isfinite (input))
            input = 0.0f;

        const auto output = b0 * input + z1;
        z1 = b1 * input - a1 * output + z2;
        z2 = b2 * input - a2 * output;

        if (! std::isfinite (output))
        {
            reset();
            return 0.0f;
        }

        return output;
    }

    void AirProcessor::prepare (double newSampleRate, int maxBlockSize, int maxChannels)
    {
        sampleRate = std::max (newSampleRate, 1.0);
        preparedBlockSize = std::max (maxBlockSize, 1);
        preparedChannels = std::max (maxChannels, 1);

        airBuffer.setSize (preparedChannels, preparedBlockSize, false, false, true);
        airValues.assign (static_cast<size_t> (preparedBlockSize), 0.0f);
        presenceValues.assign (static_cast<size_t> (preparedBlockSize), 0.0f);

        presenceBandFilters.resize (static_cast<size_t> (preparedChannels));
        textureBandFilters.resize (static_cast<size_t> (preparedChannels));
        preHighPassFilters.resize (static_cast<size_t> (preparedChannels));
        postHighPassFilters.resize (static_cast<size_t> (preparedChannels));
        updateFilterCoefficients();

        airSmoother.reset (sampleRate, 0.020);
        airSmoother.setCurrentAndTargetValue (0.0f);
        presenceSmoother.reset (sampleRate, 0.020);
        presenceSmoother.setCurrentAndTargetValue (0.0f);

        reset();
    }

    void AirProcessor::reset()
    {
        for (auto& filter : presenceBandFilters)
            filter.reset();

        for (auto& filter : textureBandFilters)
            filter.reset();

        for (auto& filter : preHighPassFilters)
            filter.reset();

        for (auto& filter : postHighPassFilters)
            filter.reset();
    }

    void AirProcessor::process (juce::AudioBuffer<float>& buffer,
                                float presenceAmount,
                                float airAmount)
    {
        juce::ScopedNoDenormals noDenormals;

        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (numChannels <= 0 || numSamples <= 0)
            return;

        const auto safePresence = std::clamp (std::isfinite (presenceAmount) ? presenceAmount : 0.0f,
                                              0.0f,
                                              1.0f);
        const auto safeAir = std::clamp (std::isfinite (airAmount) ? airAmount : 0.0f,
                                         0.0f,
                                         1.0f);

        presenceSmoother.setTargetValue (safePresence);
        airSmoother.setTargetValue (safeAir);

        const auto currentPresence = std::max (safePresence, presenceSmoother.getCurrentValue());
        const auto currentAir = std::max (safeAir, airSmoother.getCurrentValue());
        if (currentPresence <= 0.0001f && currentAir <= 0.0001f
            && ! presenceSmoother.isSmoothing() && ! airSmoother.isSmoothing())
            return;

        ensureCapacity (numChannels, numSamples);
        airBuffer.makeCopyOf (buffer, true);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            presenceValues[static_cast<size_t> (sample)] = std::clamp (presenceSmoother.getNextValue(), 0.0f, 1.0f);
            airValues[static_cast<size_t> (sample)] = std::clamp (airSmoother.getNextValue(), 0.0f, 1.0f);
        }

        constexpr float presenceWetGain = 2.6f;
        constexpr float textureWetGain = 1.8f;
        constexpr float airWetGain = 2.8f;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* dryWrite = buffer.getWritePointer (channel);
            const auto* airRead = airBuffer.getReadPointer (channel);

            auto& presenceBand = presenceBandFilters[static_cast<size_t> (channel)];
            auto& textureBand = textureBandFilters[static_cast<size_t> (channel)];
            auto& preHighPass = preHighPassFilters[static_cast<size_t> (channel)];
            auto& postHighPass = postHighPassFilters[static_cast<size_t> (channel)];

            for (int sample = 0; sample < numSamples; ++sample)
            {
                const auto presence = presenceValues[static_cast<size_t> (sample)];
                const auto air = airValues[static_cast<size_t> (sample)];
                const auto texture = mapTextureAmount (presence, air);
                float presenceWet = 0.0f;
                float textureWet = 0.0f;
                float airWet = 0.0f;

                if (presence > 0.0001f)
                {
                    const auto presenceBandSample = presenceBand.processSample (airRead[sample]);
                    const auto presenceTexture = AirWaveshaper::processSample (presenceBandSample, mapPresenceDrive (presence));
                    presenceWet = (presenceBandSample * 0.55f + presenceTexture * 0.45f)
                                * presenceWetGain
                                * mapPresenceMix (presence);
                }

                if (texture > 0.0001f)
                {
                    const auto textureBandSample = textureBand.processSample (airRead[sample]);
                    const auto textureShaped = AirWaveshaper::processSample (textureBandSample, mapTextureDrive (texture));
                    textureWet = (textureShaped - textureBandSample * 0.18f)
                               * textureWetGain
                               * mapTextureMix (texture);
                }

                if (air > 0.0001f)
                {
                    const auto filteredHighs = preHighPass.processSample (airRead[sample]);
                    const auto shaped = AirWaveshaper::processSample (filteredHighs, mapDrive (air));
                    const auto airTexture = postHighPass.processSample (shaped + filteredHighs * 0.35f);
                    airWet = airTexture * airWetGain * mapMix (air);
                }

                float wet = presenceWet + textureWet + airWet;

                if (! std::isfinite (wet))
                    wet = 0.0f;

                const auto compensated = (dryWrite[sample] + std::clamp (wet, -2.5f, 2.5f))
                                       * mapCompensationGain (presence, air);

                dryWrite[sample] = std::clamp (compensated, -4.0f, 4.0f);
            }
        }
    }

    void AirProcessor::updateFilterCoefficients()
    {
        for (auto& filter : presenceBandFilters)
            filter.setBandPass (sampleRate, 3900.0, 0.72);

        for (auto& filter : textureBandFilters)
            filter.setBandPass (sampleRate, 7200.0, 0.58);

        for (auto& filter : preHighPassFilters)
            filter.setHighPass (sampleRate, 6400.0, 0.70710678);

        for (auto& filter : postHighPassFilters)
            filter.setHighPass (sampleRate, 5900.0, 0.70710678);
    }

    void AirProcessor::ensureCapacity (int numChannels, int numSamples)
    {
        if (numChannels > preparedChannels || numSamples > preparedBlockSize)
        {
            preparedChannels = std::max (preparedChannels, numChannels);
            preparedBlockSize = std::max (preparedBlockSize, numSamples);

            airBuffer.setSize (preparedChannels, preparedBlockSize, false, false, true);
            airValues.assign (static_cast<size_t> (preparedBlockSize), 0.0f);
            presenceValues.assign (static_cast<size_t> (preparedBlockSize), 0.0f);
            presenceBandFilters.resize (static_cast<size_t> (preparedChannels));
            textureBandFilters.resize (static_cast<size_t> (preparedChannels));
            preHighPassFilters.resize (static_cast<size_t> (preparedChannels));
            postHighPassFilters.resize (static_cast<size_t> (preparedChannels));
            updateFilterCoefficients();
            reset();
        }
    }

    float AirProcessor::mapDrive (float airAmount) noexcept
    {
        const auto air = std::clamp (airAmount, 0.0f, 1.0f);
        return 1.45f + air * 3.30f;
    }

    float AirProcessor::mapMix (float airAmount) noexcept
    {
        const auto air = std::clamp (airAmount, 0.0f, 1.0f);
        return 0.70f * std::pow (air, 0.72f);
    }

    float AirProcessor::mapPresenceDrive (float presenceAmount) noexcept
    {
        const auto presence = std::clamp (presenceAmount, 0.0f, 1.0f);
        return 1.20f + presence * 2.30f;
    }

    float AirProcessor::mapPresenceMix (float presenceAmount) noexcept
    {
        const auto presence = std::clamp (presenceAmount, 0.0f, 1.0f);
        return 0.42f * std::pow (presence, 0.78f);
    }

    float AirProcessor::mapTextureDrive (float textureAmount) noexcept
    {
        const auto texture = std::clamp (textureAmount, 0.0f, 1.0f);
        return 1.25f + texture * 2.60f;
    }

    float AirProcessor::mapTextureMix (float textureAmount) noexcept
    {
        const auto texture = std::clamp (textureAmount, 0.0f, 1.0f);
        return 0.16f * std::pow (texture, 0.70f);
    }

    float AirProcessor::mapTextureAmount (float presenceAmount, float airAmount) noexcept
    {
        const auto presence = std::clamp (presenceAmount, 0.0f, 1.0f);
        const auto air = std::clamp (airAmount, 0.0f, 1.0f);
        return std::clamp (presence * 0.45f + air * 0.75f, 0.0f, 1.0f);
    }

    float AirProcessor::mapCompensationGain (float presenceAmount, float airAmount) noexcept
    {
        const auto presence = std::clamp (presenceAmount, 0.0f, 1.0f);
        const auto air = std::clamp (airAmount, 0.0f, 1.0f);
        const auto compensationDb = -0.9f * std::max (presence * 0.65f, air);
        return std::pow (10.0f, compensationDb / 20.0f);
    }
}
