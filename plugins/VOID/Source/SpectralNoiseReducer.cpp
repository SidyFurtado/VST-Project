#include "SpectralNoiseReducer.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// ==============================================================================
// Helpers
// ==============================================================================

static float dbToGain (float db)
{
    return std::pow (10.0f, db * 0.05f);
}

static float gainToDb (float gain)
{
    return 20.0f * std::log10 (std::max (gain, 1e-10f));
}

static float hannWindow (int i, int N)
{
    return 0.5f * (1.0f - std::cos (juce::MathConstants<float>::twoPi * (float) i / (float)(N - 1)));
}

// ==============================================================================
// Construction / Destruction
// ==============================================================================

SpectralNoiseReducer::SpectralNoiseReducer() {}

SpectralNoiseReducer::~SpectralNoiseReducer() {}

// ==============================================================================
// Lifecycle
// ==============================================================================

void SpectralNoiseReducer::prepare (double sampleRate_, int fftSize_, int hopSize_)
{
    sampleRate = sampleRate_;

    if (fftSize_ <= 0)
        fftSize_ = 2048;

    // Force power-of-two
    fftOrder = static_cast<int> (std::round (std::log2 (fftSize_)));
    fftSize  = 1 << fftOrder;
    numBins  = fftSize / 2;  // Number of complex bins (0 .. numBins-1, where numBins-1 = Nyquist)

    if (hopSize_ <= 0)
        hopSize_ = fftSize / 2; // 50% overlap for perfect reconstruction with Hann window
    hopSize = hopSize_;

    // Create FFT
    fft = std::make_unique<juce::dsp::FFT> (fftOrder);

    // Hann window (analysis & synthesis — satisfies COLA for 50% overlap)
    window.resize (fftSize);
    for (int i = 0; i < fftSize; ++i)
        window[i] = hannWindow (i, fftSize);

    // FFT buffers
    fftTimeData.resize (fftSize);
    fftFreqData.resize (fftSize);   // Packed format from JUCE real-only FFT
    magnitudeSpectrum.resize (numBins);
    noiseProfile.resize (numBins, 0.0f);
    noiseMinTracker.resize (numBins, std::numeric_limits<float>::max());
    rawGains.resize (numBins, 1.0f);
    smoothedGains.resize (numBins, 1.0f);
    prevGains.resize (numBins, 1.0f);

    // OLA output buffer (needs to hold at least 2 frames for 50% overlap)
    olaBuffer.resize (fftSize + hopSize * 2, 0.0f);
    olaWritePos = 0;

    // Pre-allocate smoothing scratch buffer (avoids heap allocation in audio thread)
    smoothScratchBuffer.resize (numBins);

    // Reset parameters
    reset();

    // Compute envelope coefficients
    attackCoeff  = std::exp (-1.0f / (static_cast<float> (sampleRate) * attackMs  / 1000.0f));
    releaseCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * releaseMs / 1000.0f));

    minTrackerResetInterval = static_cast<float> (fftSize) / static_cast<float> (sampleRate) * NOISE_MIN_FRAMES;
    minTrackerResetTimer = minTrackerResetInterval;

    prepared = true;
}

void SpectralNoiseReducer::reset()
{
    if (!prepared)
        return;

    std::fill (olaBuffer.begin(), olaBuffer.end(), 0.0f);
    olaWritePos = 0;

    std::fill (noiseProfile.begin(), noiseProfile.end(), 0.0f);
    std::fill (noiseMinTracker.begin(), noiseMinTracker.end(), std::numeric_limits<float>::max());
    noiseMinCounter = 0;
    learningRequested = false;
    profileFrozen = false;
    learnFrames = 0;

    std::fill (smoothedGains.begin(), smoothedGains.end(), 1.0f);
    std::fill (prevGains.begin(), prevGains.end(), 1.0f);

    inputRmsSum  = 0.0f;
    outputRmsSum = 0.0f;
    inputRmsDb   = -90.0f;
    outputRmsDb  = -90.0f;
    rmsCount     = 0;
    currentReductionDb.store (0.0f);
    smoothedMeterDb = 0.0f;
    processing = false;

    attackCoeff  = std::exp (-1.0f / (static_cast<float> (sampleRate) * attackMs  / 1000.0f));
    releaseCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * releaseMs / 1000.0f));
}

// ==============================================================================
// Parameter setters
// ==============================================================================

void SpectralNoiseReducer::setReductionDb (float db)
{
    maxReductionDb = juce::jlimit (0.0f, 60.0f, db);
}

void SpectralNoiseReducer::setNoiseFloorDb (float db)
{
    noiseFloorDb = juce::jlimit (-120.0f, 0.0f, db);
}

void SpectralNoiseReducer::setSpectralSmoothing (int bins)
{
    bins = juce::jlimit (1, 9, bins);
    if (bins % 2 == 0) ++bins;
    spectralSmoothBins = bins;
}

void SpectralNoiseReducer::setTemporalSmoothing (float factor)
{
    temporalSmoothFactor = juce::jlimit (0.0f, 1.0f, factor);
}

void SpectralNoiseReducer::setAttackMs (float ms)
{
    attackMs = std::max (1.0f, ms);
    attackCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * attackMs / 1000.0f));
}

void SpectralNoiseReducer::setReleaseMs (float ms)
{
    releaseMs = std::max (1.0f, ms);
    releaseCoeff = std::exp (-1.0f / (static_cast<float> (sampleRate) * releaseMs / 1000.0f));
}

void SpectralNoiseReducer::setLearnRate (float rate)
{
    learnRate = juce::jlimit (0.0f, 1.0f, rate);
}

void SpectralNoiseReducer::setOverSubtractionFactor (float factor)
{
    overSubFactor = juce::jlimit (0.5f, 5.0f, factor);
}

// ==============================================================================
// Noise profile management
// ==============================================================================

void SpectralNoiseReducer::startLearnProfile()
{
    learningRequested = true;
    learnFrames = 0;
}

void SpectralNoiseReducer::freezeProfile()
{
    profileFrozen = true;
}

void SpectralNoiseReducer::unfreezeProfile()
{
    profileFrozen = false;
}

float SpectralNoiseReducer::getNoiseBinDb (int bin) const noexcept
{
    if (bin < 0 || bin >= numBins)
        return -100.0f;
    return gainToDb (noiseProfile[bin]);
}

float SpectralNoiseReducer::getEstimatedNoiseFloorDb() const noexcept
{
    // Average the noise profile over all bins (skip DC)
    double sum = 0.0;
    int count = 0;
    for (int i = 1; i < numBins; ++i) // skip DC
    {
        sum += noiseProfile[i];
        ++count;
    }
    float avgMag = static_cast<float> (sum / std::max (1, count));
    return gainToDb (avgMag + 1e-10f);
}

// ==============================================================================
// Correct FFT bin extraction for JUCE real-only FFT
//
// JUCE's performRealOnlyForwardTransform packs the output as:
//   Index 0: real part of bin 0 (DC component)
//   Index 1: real part of bin N/2 (Nyquist frequency)
//   For k = 1 to N/2-1:
//     Index 2*k:   real part of bin k
//     Index 2*k+1: imaginary part of bin k
//
// numBins = fftSize / 2
// Bin 0 and bin (numBins-1) have only real components (imag = 0)
// Bins 1 to numBins-2 are complex
// ==============================================================================

static void extractMagnitudeFromFreqData (const float* freqData, int /*fftSize*/,
                                          float* magnitude, int numBins)
{
    // Bin 0 (DC): real at index 0, imag = 0
    magnitude[0] = std::abs (freqData[0]);

    // Bins 1 to numBins-2: complex pairs
    for (int b = 1; b < numBins - 1; ++b)
    {
        float re = freqData[2 * b];
        float im = freqData[2 * b + 1];
        magnitude[b] = std::sqrt (re * re + im * im);
    }

    // Bin numBins-1 (Nyquist): real at index 1, imag = 0
    if (numBins > 1)
        magnitude[numBins - 1] = std::abs (freqData[1]);
}

// Apply per-bin gains: out[re] = in[re] * gain, out[im] = in[im] * gain
static void applyGainsToFreqData (float* freqData, int /*fftSize*/,
                                  const float* gains, int numBins)
{
    // Bin 0 (DC)
    freqData[0] *= gains[0];

    // Bins 1 to numBins-2: apply to both real and imag
    for (int b = 1; b < numBins - 1; ++b)
    {
        freqData[2 * b]     *= gains[b];
        freqData[2 * b + 1] *= gains[b];
    }

    // Bin numBins-1 (Nyquist)
    if (numBins > 1)
        freqData[1] *= gains[numBins - 1];
}

// ==============================================================================
// Core Processing
// ==============================================================================

int SpectralNoiseReducer::processBlock (const float* input, float* output, int numSamples,
                                         bool learnNoise)
{
    if (!prepared || processing)
    {
        if (output != input)
            std::memcpy (output, input, numSamples * sizeof (float));
        return numSamples;
    }

    // RAII guard: reset processing flag on scope exit (exception-safe)
    struct ProcessingGuard {
        bool& flag;
        ProcessingGuard (bool& f) : flag (f) { flag = true; }
        ~ProcessingGuard() { flag = false; }
    } guard (processing);

    // Zero output buffer
    std::memset (output, 0, numSamples * sizeof (float));

    // ---- Input RMS metering ----
    float sampleSquareSum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        sampleSquareSum += input[i] * input[i];
    float blockRms = std::sqrt (sampleSquareSum / std::max (1, numSamples));
    const float rmsAlpha = 0.01f;
    inputRmsDb = (1.0f - rmsAlpha) * inputRmsDb + rmsAlpha * gainToDb (blockRms + 1e-10f);

    // ---- Noise Profile Learning ----
    if (learnNoise || learningRequested)
    {
        for (int i = 0; i < numSamples; i += hopSize)
        {
            int frameStart = i;
            int frameSize = std::min (fftSize, numSamples - frameStart);

            // Copy to FFT buffer with zero-padding for partial frames
            std::fill (fftTimeData.begin(), fftTimeData.end(), 0.0f);
            std::memcpy (fftTimeData.data(), input + frameStart, frameSize * sizeof (float));

            // Apply analysis window
            for (int j = 0; j < fftSize; ++j)
                fftTimeData[j] *= window[j];

            // Forward FFT
            std::memcpy (fftFreqData.data(), fftTimeData.data(), fftSize * sizeof (float));
            fft->performRealOnlyForwardTransform (fftFreqData.data());

            // Extract magnitude spectrum
            extractMagnitudeFromFreqData (fftFreqData.data(), fftSize,
                                          magnitudeSpectrum.data(), numBins);

            // Update noise profile (running minimum tracker)
            updateNoiseProfile();
        }

        if (learningRequested)
        {
            learnFrames++;
            if (learnFrames >= MIN_LEARN_FRAMES)
            {
                // Apply learned profile
                for (int b = 0; b < numBins; ++b)
                {
                    if (noiseMinTracker[b] < std::numeric_limits<float>::max() * 0.5f)
                        noiseProfile[b] = noiseMinTracker[b];
                }
                learningRequested = false;
                juce::Logger::writeToLog ("[VOID] Noise profile learned (" + juce::String (learnFrames) + " frames)");
            }
        }
    }

    // ---- Main processing loop (50% overlap-add) ----
    int samplesProcessed = 0;
    while (samplesProcessed < numSamples)
    {
        // Copy input segment into FFT buffer (zero-pad partial frames)
        int segSize = std::min (fftSize, numSamples - samplesProcessed);
        std::fill (fftTimeData.begin(), fftTimeData.end(), 0.0f);
        std::memcpy (fftTimeData.data(), input + samplesProcessed, segSize * sizeof (float));

        // Apply analysis window
        for (int j = 0; j < fftSize; ++j)
            fftTimeData[j] *= window[j];

        // --- Forward FFT ---
        std::memcpy (fftFreqData.data(), fftTimeData.data(), fftSize * sizeof (float));
        fft->performRealOnlyForwardTransform (fftFreqData.data());

        // --- Extract magnitude and compute gains ---
        extractMagnitudeFromFreqData (fftFreqData.data(), fftSize,
                                      magnitudeSpectrum.data(), numBins);

        computeGains();

        // --- Apply gains to frequency data (preserves original phase) ---
        applyGainsToFreqData (fftFreqData.data(), fftSize,
                              smoothedGains.data(), numBins);

        // --- Inverse FFT ---
        fft->performRealOnlyInverseTransform (fftFreqData.data());

        // Copy time-domain output
        std::memcpy (fftTimeData.data(), fftFreqData.data(), fftSize * sizeof (float));

        // --- Overlap-Add into the OLA buffer ---
        // NOTE: No synthesis window multiplication!
        // Standard OLA with Hann window at 50% overlap gives perfect reconstruction
        // when the window is only applied at analysis (before FFT).
        // Applying the window a second time (WOLA) would introduce amplitude modulation
        // because Hann² doesn't satisfy the COLA condition at 50% overlap.

        for (int j = 0; j < fftSize; ++j)
        {
            int idx = (olaWritePos + j) % static_cast<int> (olaBuffer.size());
            olaBuffer[idx] += fftTimeData[j];
        }

        // --- Read hopSize samples from OLA buffer into output ---
        for (int j = 0; j < hopSize; ++j)
        {
            int outIdx = samplesProcessed + j;
            if (outIdx < numSamples)
            {
                output[outIdx] = olaBuffer[olaWritePos];
                olaBuffer[olaWritePos] = 0.0f;  // Clear consumed sample
                olaWritePos = (olaWritePos + 1) % static_cast<int> (olaBuffer.size());
            }
        }

        samplesProcessed += segSize;

        if (samplesProcessed >= numSamples)
            break;
    }

    // ---- Safety net: if OLA didn't produce enough output, copy remaining from input ----
    if (samplesProcessed < numSamples)
    {
        for (int i = samplesProcessed; i < numSamples; ++i)
            output[i] = input[i];
    }

    // ---- Output RMS metering ----
    float outSquareSum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        outSquareSum += output[i] * output[i];
    float outBlockRms = std::sqrt (outSquareSum / std::max (1, numSamples));
    outputRmsDb = (1.0f - rmsAlpha) * outputRmsDb + rmsAlpha * gainToDb (outBlockRms + 1e-10f);

    // ---- Update reduction metering ----
    float reduction = gainToDb (blockRms + 1e-10f) - gainToDb (outBlockRms + 1e-10f);
    smoothedMeterDb = 0.9f * smoothedMeterDb + 0.1f * std::max (0.0f, reduction);
    currentReductionDb.store (smoothedMeterDb);

    return numSamples;
}

// ==============================================================================
// Compute Gains — Wiener Filter with Oversubtraction
// ==============================================================================

void SpectralNoiseReducer::computeGains()
{
    if (noiseProfile.empty())
        return;

    const float maxReductionGain = dbToGain (-maxReductionDb);
    const float floorGain = dbToGain (noiseFloorDb);

    for (int b = 0; b < numBins; ++b)
    {
        float signalMag = magnitudeSpectrum[b];
        float noiseMag  = noiseProfile[b];

        // Guard against silence / invalid noise
        if (noiseMag < 1e-10f || signalMag < 1e-10f)
        {
            rawGains[b] = 1.0f;
            continue;
        }

        // Signal power = |X|^2, noise power = |N|^2
        float signalPower = signalMag * signalMag;
        float noisePower  = noiseMag * noiseMag;

        // Oversubtraction: effective_noise = alpha * |N|^2
        float effectiveNoisePower = noisePower * overSubFactor;

        // Clean power estimate: |S|^2 = |X|^2 - alpha * |N|^2
        float cleanPower = signalPower - effectiveNoisePower;

        // Clamp to noise floor (prevents metallic artifacts)
        float floorPower = floorGain * floorGain;

        float gain;
        if (cleanPower < floorPower)
        {
            // Apply noise floor instead of full attenuation
            gain = std::sqrt (floorPower / std::max (signalPower, 1e-10f));
        }
        else
        {
            // Wiener gain: H = |S|^2 / |X|^2
            gain = cleanPower / std::max (signalPower, 1e-10f);
            gain = std::sqrt (std::max (gain, 1e-10f));
        }

        // Apply max reduction limit
        gain = std::max (gain, maxReductionGain);
        gain = std::min (gain, 1.0f);

        rawGains[b] = gain;
    }

    // --- Spectral smoothing ---
    applySpectralSmoothing();

    // --- Temporal smoothing ---
    applyTemporalSmoothing();
}

// ==============================================================================
// Spectral Smoothing — Average adjacent bins
// ==============================================================================

void SpectralNoiseReducer::applySpectralSmoothing()
{
    if (spectralSmoothBins <= 1)
    {
        std::memcpy (smoothedGains.data(), rawGains.data(), numBins * sizeof (float));
        return;
    }

    int half = spectralSmoothBins / 2;

    // Use pre-allocated scratch buffer (zero heap allocation in audio thread)
    float* temp = smoothScratchBuffer.data();

    for (int b = 0; b < numBins; ++b)
    {
        float sum = 0.0f;
        int count = 0;

        for (int j = -half; j <= half; ++j)
        {
            int idx = b + j;
            if (idx >= 0 && idx < numBins)
            {
                sum += rawGains[idx];
                ++count;
            }
        }

        temp[b] = count > 0 ? sum / count : 1.0f;
    }

    std::memcpy (smoothedGains.data(), temp, numBins * sizeof (float));
}

// ==============================================================================
// Temporal Smoothing — Fast attack, slow release
// ==============================================================================

void SpectralNoiseReducer::applyTemporalSmoothing()
{
    if (temporalSmoothFactor <= 0.0f)
    {
        std::memcpy (prevGains.data(), smoothedGains.data(), numBins * sizeof (float));
        return;
    }

    for (int b = 0; b < numBins; ++b)
    {
        float currentGain = smoothedGains[b];
        float prevGain    = prevGains[b];

        // Different smoothing direction: attack = gain decrease (more noise reduction)
        // Release = gain increase (less noise reduction)
        float alpha = (currentGain < prevGain) ? attackCoeff : releaseCoeff;

        // One-pole smoothing
        float smoothed = alpha * prevGain + (1.0f - alpha) * currentGain;

        // Blend between raw and smoothed
        smoothedGains[b] = (1.0f - temporalSmoothFactor) * currentGain
                         + temporalSmoothFactor * smoothed;

        prevGains[b] = smoothedGains[b];
    }
}

// ==============================================================================
// Noise Profile Update — Running minimum tracker per bin
// ==============================================================================

void SpectralNoiseReducer::updateNoiseProfile()
{
    if (profileFrozen)
        return;

    for (int b = 0; b < numBins; ++b)
    {
        float mag = magnitudeSpectrum[b];

        // Track running minimum
        if (mag < noiseMinTracker[b])
        {
            noiseMinTracker[b] = mag;
            noiseMinCounter = 0;
        }
    }

    noiseMinCounter++;

    // After NOISE_MIN_FRAMES frames, update the profile
    if (noiseMinCounter >= NOISE_MIN_FRAMES)
    {
        for (int b = 0; b < numBins; ++b)
        {
            float estimate = noiseMinTracker[b];
            if (estimate < std::numeric_limits<float>::max() * 0.5f)
            {
                // Smooth update with learning rate
                noiseProfile[b] = (1.0f - learnRate) * noiseProfile[b] + learnRate * estimate;
            }
            noiseMinTracker[b] = std::numeric_limits<float>::max();
        }
        noiseMinCounter = 0;
    }
}
