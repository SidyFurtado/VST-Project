#include "EQBand.h"

void EQBand::initialize(int index)
{
    bandIndex = index;
    // Enabled por padrão apenas na banda 1 (índice 0)
    enabled = (bandIndex == 0);
}

void EQBand::prepare(double newSampleRate, int numChannels)
{
    sampleRate = newSampleRate;
    filter.prepare(newSampleRate, numChannels);
    for (auto& f : extraCutFilters)
        f.prepare(newSampleRate, numChannels);

    // Initialize smoothers: 30 ms for frequency, 20 ms for gain and Q
    freqSmoother.prepare(newSampleRate, 30.0f);
    gainSmoother.prepare(newSampleRate, 20.0f);
    qSmoother.prepare(newSampleRate, 20.0f);

    // Reset smoothers to current values immediately
    freqSmoother.reset(frequency);
    gainSmoother.reset(gainDecibels);
    qSmoother.reset(q);

    envelopeState = 0.0f;
    currentDynamicGainDb = 0.0f;
    updateEnvelopeCoefficients();

    updateFilterCoefficients();
}

void EQBand::reset()
{
    filter.reset();
    for (auto& f : extraCutFilters)
        f.reset();
    freqSmoother.reset(frequency);
    gainSmoother.reset(gainDecibels);
    qSmoother.reset(q);
    envelopeState = 0.0f;
    currentDynamicGainDb = 0.0f;
}

void EQBand::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
}

void EQBand::setBypassed(bool newBypassed)
{
    bypassed = newBypassed;
}

void EQBand::setType(int typeIndex)
{
    if (typeIndex >= 0 && typeIndex <= 6)
    {
        const auto newType = static_cast<FilterType>(typeIndex);
        if (type != newType)
        {
            type = newType;
            updateFilterCoefficients();
        }
    }
}

void EQBand::setFrequency(float freq)
{
    const float clampedFreq = std::max(20.0f, std::min(freq, 20000.0f));
    if (std::abs(frequency - clampedFreq) > 1e-5f)
    {
        frequency = clampedFreq;
        freqSmoother.setTarget(clampedFreq);

        if (sampleRate <= 0.0 || !freqSmoother.isSmoothing())
        {
            updateFilterCoefficients();
        }
    }
}

void EQBand::setGainDecibels(float gainDb)
{
    const float clampedGain = std::max(-24.0f, std::min(gainDb, 24.0f));
    if (std::abs(gainDecibels - clampedGain) > 1e-5f)
    {
        gainDecibels = clampedGain;
        gainSmoother.setTarget(clampedGain);

        if (sampleRate <= 0.0 || !gainSmoother.isSmoothing())
        {
            updateFilterCoefficients();
        }
    }
}

void EQBand::setQ(float qVal)
{
    const float clampedQ = std::max(0.1f, std::min(qVal, 18.0f));
    if (std::abs(q - clampedQ) > 1e-5f)
    {
        q = clampedQ;
        qSmoother.setTarget(clampedQ);

        if (sampleRate <= 0.0 || !qSmoother.isSmoothing())
        {
            updateFilterCoefficients();
        }
    }
}

void EQBand::setSlope(int slopeDbPerOct)
{
    int newSlope = slopeDbPerOct;
    if (newSlope != 12 && newSlope != 24 && newSlope != 48)
        newSlope = 12;

    if (slope != newSlope)
    {
        slope = newSlope;
        updateFilterCoefficients();
    }
}

void EQBand::processBlock(juce::AudioBuffer<float>& buffer) noexcept
{
    // Bypass completely if band is disabled or bypassed
    if (!enabled || bypassed || sampleRate <= 0.0)
    {
        currentDynamicGainDb = 0.0f;
        return;
    }

    const int numSamples = buffer.getNumSamples();
    if (numSamples <= 0)
        return;

    float currentFreq = freqSmoother.getCurrentValue();
    float currentGain = gainSmoother.getCurrentValue();
    float currentQ    = qSmoother.getCurrentValue();

    // 1. Advance smoothers if currently interpolating
    if (freqSmoother.isSmoothing() || gainSmoother.isSmoothing() || qSmoother.isSmoothing())
    {
        currentFreq = freqSmoother.getNextBlockValue(numSamples);
        currentGain = gainSmoother.getNextBlockValue(numSamples);
        currentQ    = qSmoother.getNextBlockValue(numSamples);
    }

    // 2. Dynamic EQ Gain Modulation (Phase 6.0)
    // Dynamic EQ is only active for Bell, Low Shelf, and High Shelf, and when range is non-zero
    const bool isDynamicSupported = (type == FilterType::Bell || type == FilterType::LowShelf || type == FilterType::HighShelf);
    const bool isDynamicActive = (dynamicEnabled && isDynamicSupported && std::abs(dynamicRange) > 1e-4f);

    if (isDynamicActive)
    {
        // 2a. Envelope Follower: run sample-by-sample on INPUT buffer (before filtering)
        float currentEnvelope = envelopeState;
        const int numChans = buffer.getNumChannels();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSampleL = (numChans > 0) ? buffer.getSample(0, sample) : 0.0f;
            float inputSampleR = (numChans > 1) ? buffer.getSample(1, sample) : inputSampleL;
            float absSample = std::max(std::abs(inputSampleL), std::abs(inputSampleR));

            // Prevent subnormals
            if (absSample < 1e-15f) absSample = 0.0f;

            if (absSample > currentEnvelope)
                currentEnvelope = absSample + attackCoeff * (currentEnvelope - absSample);
            else
                currentEnvelope = absSample + releaseCoeff * (currentEnvelope - absSample);
        }

        // Safety clamp against denormal/NaN/Inf in persistent state
        if (std::isnan(currentEnvelope) || std::isinf(currentEnvelope) || currentEnvelope < 1e-15f)
            currentEnvelope = 0.0f;

        envelopeState = currentEnvelope;

        // 2b. Dynamic Gain calculation relative to threshold (convert to dBFS)
        float envelopeDb = juce::Decibels::gainToDecibels(envelopeState, -100.0f);
        float dynamicGainDb = 0.0f;

        if (envelopeDb > dynamicThreshold)
        {
            float overDb = envelopeDb - dynamicThreshold;
            // Asymptotic musical knee curve
            dynamicGainDb = dynamicRange * (1.0f - std::exp(-overDb / 12.0f));

            // Strict clamp against range limits
            if (dynamicRange > 0.0f)
                dynamicGainDb = std::max(0.0f, std::min(dynamicGainDb, dynamicRange));
            else
                dynamicGainDb = std::min(0.0f, std::max(dynamicGainDb, dynamicRange));
        }

        if (std::isnan(dynamicGainDb) || std::isinf(dynamicGainDb))
            dynamicGainDb = 0.0f;

        currentDynamicGainDb = dynamicGainDb;

        float effectiveGainDb = currentGain + dynamicGainDb;
        effectiveGainDb = std::max(-24.0f, std::min(effectiveGainDb, 24.0f));

        // Update biquad coefficients for the block incorporating modulated gain (max once per block)
        updateFilterCoefficients(currentFreq, effectiveGainDb, currentQ);
    }
    else
    {
        currentDynamicGainDb = 0.0f;
        // For non-dynamic processing, only update coefficients if smoothers are moving
        if (freqSmoother.isSmoothing() || gainSmoother.isSmoothing() || qSmoother.isSmoothing())
        {
            // Safety clamps
            currentFreq = std::max(20.0f, std::min(currentFreq, 20000.0f));
            currentGain = std::max(-24.0f, std::min(currentGain, 24.0f));
            currentQ    = std::max(0.1f,  std::min(currentQ, 18.0f));
            updateFilterCoefficients(currentFreq, currentGain, currentQ);
        }
    }

    // 3. Process through main filter stage and extra cascade filters depending on Channel Mode
    const int numChans = buffer.getNumChannels();

    if (numChans == 1)
    {
        // Mono buffer fallback: Stereo (0), Left (3), and Mid (1) process the single channel.
        // Right (4) and Side (2) bypass this band (since right/side channel does not exist).
        if (channelMode == 0 || channelMode == 1 || channelMode == 3)
        {
            filter.processBlock(buffer);
            if (type == FilterType::LowCut || type == FilterType::HighCut)
            {
                int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                for (int i = 0; i < numStages - 1; ++i)
                {
                    extraCutFilters[static_cast<size_t>(i)].processBlock(buffer);
                }
            }
        }
    }
    else if (numChans >= 2)
    {
        // Stereo buffer
        if (channelMode == 0) // Stereo
        {
            filter.processBlock(buffer);
            if (type == FilterType::LowCut || type == FilterType::HighCut)
            {
                int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                for (int i = 0; i < numStages - 1; ++i)
                {
                    extraCutFilters[static_cast<size_t>(i)].processBlock(buffer);
                }
            }
        }
        else if (channelMode == 3) // Left only
        {
            float* leftData = buffer.getWritePointer(0);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float x = leftData[sample];
                x = filter.processSample(0, x);
                if (type == FilterType::LowCut || type == FilterType::HighCut)
                {
                    int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                    for (int i = 0; i < numStages - 1; ++i)
                    {
                        x = extraCutFilters[static_cast<size_t>(i)].processSample(0, x);
                    }
                }
                leftData[sample] = x;
            }
        }
        else if (channelMode == 4) // Right only
        {
            float* rightData = buffer.getWritePointer(1);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float x = rightData[sample];
                x = filter.processSample(1, x);
                if (type == FilterType::LowCut || type == FilterType::HighCut)
                {
                    int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                    for (int i = 0; i < numStages - 1; ++i)
                    {
                        x = extraCutFilters[static_cast<size_t>(i)].processSample(1, x);
                    }
                }
                rightData[sample] = x;
            }
        }
        else if (channelMode == 1) // Mid
        {
            float* leftData = buffer.getWritePointer(0);
            float* rightData = buffer.getWritePointer(1);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float l = leftData[sample];
                float r = rightData[sample];
                float m = (l + r) * 0.5f;
                float s = (l - r) * 0.5f;

                // Process Mid using channel 0 of the filter state (isolated and consistent)
                m = filter.processSample(0, m);
                if (type == FilterType::LowCut || type == FilterType::HighCut)
                {
                    int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                    for (int i = 0; i < numStages - 1; ++i)
                    {
                        m = extraCutFilters[static_cast<size_t>(i)].processSample(0, m);
                    }
                }

                // Reconstruct L/R
                leftData[sample] = m + s;
                rightData[sample] = m - s;
            }
        }
        else if (channelMode == 2) // Side
        {
            float* leftData = buffer.getWritePointer(0);
            float* rightData = buffer.getWritePointer(1);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float l = leftData[sample];
                float r = rightData[sample];
                float m = (l + r) * 0.5f;
                float s = (l - r) * 0.5f;

                // Process Side using channel 1 of the filter state (isolated and consistent)
                s = filter.processSample(1, s);
                if (type == FilterType::LowCut || type == FilterType::HighCut)
                {
                    int numStages = (slope == 24) ? 2 : ((slope == 48) ? 4 : 1);
                    for (int i = 0; i < numStages - 1; ++i)
                    {
                        s = extraCutFilters[static_cast<size_t>(i)].processSample(1, s);
                    }
                }

                // Reconstruct L/R
                leftData[sample] = m + s;
                rightData[sample] = m - s;
            }
        }
    }
}

void EQBand::updateFilterCoefficients()
{
    // Read current smoother values (already clamped by the set* methods)
    float f    = std::max(20.0f, std::min(freqSmoother.getCurrentValue(), 20000.0f));
    float g    = std::max(-24.0f, std::min(gainSmoother.getCurrentValue(), 24.0f));
    float qVal = std::max(0.1f,  std::min(qSmoother.getCurrentValue(), 18.0f));

    updateFilterCoefficients(f, g, qVal);
}

void EQBand::updateFilterCoefficients(float f, float g, float qVal)
{
    int numStages = 1;
    if (type == FilterType::LowCut || type == FilterType::HighCut)
    {
        if (slope == 24) numStages = 2;
        else if (slope == 48) numStages = 4;
    }

    switch (type)
    {
        case FilterType::Bell:
            filter.setPeaking(f, g, qVal);
            break;
        case FilterType::LowCut:
            filter.setHighPass(f, qVal);
            for (int i = 0; i < numStages - 1; ++i)
                extraCutFilters[static_cast<size_t>(i)].setHighPass(f, qVal);
            break;
        case FilterType::HighCut:
            filter.setLowPass(f, qVal);
            for (int i = 0; i < numStages - 1; ++i)
                extraCutFilters[static_cast<size_t>(i)].setLowPass(f, qVal);
            break;
        case FilterType::LowShelf:
            filter.setLowShelf(f, g);
            break;
        case FilterType::HighShelf:
            filter.setHighShelf(f, g);
            break;
        case FilterType::Notch:
            filter.setNotch(f, qVal);
            break;
        case FilterType::BandPass:
            filter.setBandPass(f, qVal);
            break;
        default:
            // Unknown type: identity filter (bypass)
            filter.setPeaking(f, 0.0f, qVal);
            break;
    }
}

// ---------------------------------------------------------------------------
// Dynamic EQ Setters & Helpers (Phase 6.0)
// ---------------------------------------------------------------------------
void EQBand::setDynamicEnabled(bool enabledVal)
{
    if (dynamicEnabled != enabledVal)
    {
        dynamicEnabled = enabledVal;
        if (!dynamicEnabled)
        {
            envelopeState = 0.0f;
            updateFilterCoefficients(); // Reset to static gain coefficients immediately
        }
    }
}

void EQBand::setDynamicRange(float rangeDb)
{
    dynamicRange = std::max(-24.0f, std::min(rangeDb, 24.0f));
}

void EQBand::setDynamicThreshold(float thresholdDb)
{
    dynamicThreshold = std::max(-60.0f, std::min(thresholdDb, 0.0f));
}

void EQBand::setDynamicAttack(float attackMs)
{
    float newAttack = std::max(0.1f, std::min(attackMs, 200.0f));
    if (std::abs(dynamicAttack - newAttack) > 1e-5f)
    {
        dynamicAttack = newAttack;
        updateEnvelopeCoefficients();
    }
}

void EQBand::setDynamicRelease(float releaseMs)
{
    float newRelease = std::max(10.0f, std::min(releaseMs, 2000.0f));
    if (std::abs(dynamicRelease - newRelease) > 1e-5f)
    {
        dynamicRelease = newRelease;
        updateEnvelopeCoefficients();
    }
}

float EQBand::getCurrentDynamicGainDb() const
{
    return currentDynamicGainDb.load();
}

void EQBand::updateEnvelopeCoefficients()
{
    if (sampleRate > 0.0)
    {
        const float attackSec = std::max(0.1f, dynamicAttack) / 1000.0f;
        const float releaseSec = std::max(10.0f, dynamicRelease) / 1000.0f;

        attackCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate) * attackSec));
        releaseCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate) * releaseSec));
    }
    else
    {
        attackCoeff = 0.99f;
        releaseCoeff = 0.99f;
    }
}

void EQBand::setChannelMode(int mode)
{
    if (mode >= 0 && mode <= 4)
    {
        channelMode = mode;
    }
}

