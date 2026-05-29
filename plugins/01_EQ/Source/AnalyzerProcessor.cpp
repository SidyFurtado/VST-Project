#include "AnalyzerProcessor.h"
#include <cmath>

AnalyzerProcessor::AnalyzerProcessor()
    : fft (FFT_ORDER)
{
    // Pre-calculate Hann window coefficients:
    // w[n] = 0.5 * (1 - cos(2π·n / (N-1)))  for n = 0..N-1
    // This prevents spectral leakage at the edges of each analysis frame.
    for (int n = 0; n < FFT_SIZE; ++n)
    {
        hannWindow[n] = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi
                                                  * (float) n / (float) (FFT_SIZE - 1)));
    }

    // Zero-initialize all working arrays
    fifoBuffer.fill (0.0f);
    fftData.fill (0.0f);
    spectrumBins.fill (FLOOR_DB);

    fifoFill = 0;
}

void AnalyzerProcessor::pushSamples (const float* data, int numSamples)
{
    int remaining = numSamples;
    int srcOffset = 0;

    while (remaining > 0)
    {
        int spaceInFifo = FFT_SIZE - fifoFill;
        int toCopy = std::min (remaining, spaceInFifo);

        std::memcpy (fifoBuffer.data() + fifoFill, data + srcOffset,
                     (size_t) toCopy * sizeof (float));

        fifoFill  += toCopy;
        srcOffset += toCopy;
        remaining -= toCopy;

        // When the accumulation buffer is full, process one FFT frame
        if (fifoFill >= FFT_SIZE)
            process();
    }
}

void AnalyzerProcessor::process()
{
    // 1. Copy the accumulated samples into the FFT working buffer and apply the Hann window.
    //    fftData has double size: first N slots = real signal, next N slots = imaginary (zeros).
    for (int n = 0; n < FFT_SIZE; ++n)
    {
        fftData[n]            = fifoBuffer[n] * hannWindow[n];
        fftData[n + FFT_SIZE] = 0.0f; // imaginary part = 0
    }

    // 2. Perform the magnitude-only forward FFT.
    //    After this call, fftData[0..FFT_SIZE-1] contains the magnitudes of each bin.
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    // 3. Convert magnitudes to dBFS and apply peak-hold ballistics (decay).
    //    Normalisation by FFT_SIZE converts from raw magnitude to a per-sample scale.
    for (int k = 0; k < NUM_BINS; ++k)
    {
        float magnitude = fftData[k] / (float) FFT_SIZE;

        // Guard against log of zero (silence → floor_db)
        float newDb = (magnitude > 0.0f)
                        ? 20.0f * std::log10 (magnitude)
                        : FLOOR_DB;

        // Clamp to a sensible floor
        newDb = std::max (newDb, FLOOR_DB);

        // Apply peak-hold with dB-domain decay (ballistics):
        // If the new value is higher → snap up immediately.
        // Otherwise → fall by a fixed dB amount per FFT frame.
        //
        // Important: spectrum values are negative dBFS. Multiplying a negative
        // dB value by a factor such as 0.78 moves it toward 0 dB, which makes
        // silence decay in the wrong direction. Subtracting in dB preserves the
        // expected visual falloff toward the floor.
        constexpr float decayDbPerFrame = 4.5f;
        const float decayedDb = std::max (FLOOR_DB, spectrumBins[k] - decayDbPerFrame);
        spectrumBins[k] = std::max (newDb, decayedDb);
    }

    // 4. 50% overlap-shift: keep the second half of the buffer as the new start.
    //    This preserves temporal continuity between frames.
    constexpr int HALF = FFT_SIZE / 2;
    std::memmove (fifoBuffer.data(), fifoBuffer.data() + HALF,
                  (size_t) HALF * sizeof (float));
    fifoFill = HALF;
}
