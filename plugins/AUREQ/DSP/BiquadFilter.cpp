#include "BiquadFilter.h"
#include <cmath>
#include <algorithm>

void BiquadFilter::prepare(double sampleRate, int numChannels)
{
    currentSampleRate = sampleRate;
    currentNumChannels = numChannels;

    // Allocate channel states (doing it during prepare prevents audio-thread allocations)
    const size_t numChans = static_cast<size_t>(std::max(0, numChannels));
    z1.assign(numChans, 0.0);
    z2.assign(numChans, 0.0);
}

void BiquadFilter::reset()
{
    std::fill(z1.begin(), z1.end(), 0.0);
    std::fill(z2.begin(), z2.end(), 0.0);
}

void BiquadFilter::setPeaking(float frequencyHz, float gainDb, float q)
{
    if (currentSampleRate <= 0.0)
    {
        setBypassCoefficients();
        return;
    }

    // 1. Clamping inputs according to safety rules
    // Frequency between 20 Hz and Nyquist * 0.95, capped at 20000 Hz
    const double nyquist = currentSampleRate * 0.5;
    const double maxFreq = std::min(20000.0, nyquist * 0.95);
    const double minFreq = 20.0;
    const double f0 = std::max(minFreq, std::min(static_cast<double>(frequencyHz), maxFreq));

    // Q between 0.1 and 18.0
    const double Q = std::max(0.1, std::min(static_cast<double>(q), 18.0));

    // 2. Intermediate variables for RBJ cookbook Peaking EQ
    const double A = std::pow(10.0, static_cast<double>(gainDb) / 40.0);
    const double omega0 = 2.0 * juce::MathConstants<double>::pi * f0 / currentSampleRate;
    const double sinOmega0 = std::sin(omega0);
    const double cosOmega0 = std::cos(omega0);
    const double alpha = sinOmega0 / (2.0 * Q);

    // 3. Compute coefficients
    const double a0 = 1.0 + alpha / A;

    // Safety guard: division by zero prevention
    if (std::abs(a0) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double raw_a1 = -2.0 * cosOmega0;
    const double raw_a2 = 1.0 - alpha / A;
    const double raw_b0 = 1.0 + alpha * A;
    const double raw_b1 = -2.0 * cosOmega0;
    const double raw_b2 = 1.0 - alpha * A;

    // 4. Normalization
    const double norm_b0 = raw_b0 / a0;
    const double norm_b1 = raw_b1 / a0;
    const double norm_b2 = raw_b2 / a0;
    const double norm_a1 = raw_a1 / a0;
    const double norm_a2 = raw_a2 / a0;

    // 5. Numerical safety checks (reject NaN/Inf)
    if (std::isnan(norm_b0) || std::isinf(norm_b0) ||
        std::isnan(norm_b1) || std::isinf(norm_b1) ||
        std::isnan(norm_b2) || std::isinf(norm_b2) ||
        std::isnan(norm_a1) || std::isinf(norm_a1) ||
        std::isnan(norm_a2) || std::isinf(norm_a2))
    {
        setBypassCoefficients();
        return;
    }

    // Apply coefficients
    b0 = norm_b0;
    b1 = norm_b1;
    b2 = norm_b2;
    a1 = norm_a1;
    a2 = norm_a2;
}

// ---------------------------------------------------------------------------
// Helper: shared input sanitisation used by all filter type methods.
// Returns false if inputs are fundamentally invalid (signals caller to bypass).
// ---------------------------------------------------------------------------
static bool sanitizeCommonInputs(double sampleRate,
                                  float frequencyHz,
                                  double& f0Out,
                                  double& omega0Out,
                                  double& sinOOut,
                                  double& cosOOut)
{
    if (sampleRate <= 0.0)
        return false;

    const double nyquist  = sampleRate * 0.5;
    const double maxFreq  = std::min(20000.0, nyquist * 0.95);
    f0Out    = std::max(20.0, std::min(static_cast<double>(frequencyHz), maxFreq));
    omega0Out = 2.0 * juce::MathConstants<double>::pi * f0Out / sampleRate;
    sinOOut  = std::sin(omega0Out);
    cosOOut  = std::cos(omega0Out);
    return true;
}

// ---------------------------------------------------------------------------
// Helper: normalise five raw biquad coefficients by a0, then validate.
// Returns false if a0 ≈ 0 or any result is NaN/Inf.
// ---------------------------------------------------------------------------
static bool normaliseCoeffs(double raw_b0, double raw_b1, double raw_b2,
                             double raw_a1, double raw_a2, double a0,
                             double& nb0, double& nb1, double& nb2,
                             double& na1, double& na2)
{
    if (std::abs(a0) < 1e-12)
        return false;

    nb0 = raw_b0 / a0;
    nb1 = raw_b1 / a0;
    nb2 = raw_b2 / a0;
    na1 = raw_a1 / a0;
    na2 = raw_a2 / a0;

    return !(std::isnan(nb0) || std::isinf(nb0) ||
             std::isnan(nb1) || std::isinf(nb1) ||
             std::isnan(nb2) || std::isinf(nb2) ||
             std::isnan(na1) || std::isinf(na1) ||
             std::isnan(na2) || std::isinf(na2));
}

// ---------------------------------------------------------------------------
void BiquadFilter::setHighPass(float frequencyHz, float q)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double Q     = std::max(0.1, std::min(static_cast<double>(q), 18.0));
    const double alpha = sinO / (2.0 * Q);

    // RBJ HPF coefficients (12 dB/oct single-pole biquad)
    const double raw_b0 =  (1.0 + cosO) / 2.0;
    const double raw_b1 = -(1.0 + cosO);
    const double raw_b2 =  (1.0 + cosO) / 2.0;
    const double a0     =   1.0 + alpha;
    const double raw_a1 =  -2.0 * cosO;
    const double raw_a2 =   1.0 - alpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setLowPass(float frequencyHz, float q)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double Q     = std::max(0.1, std::min(static_cast<double>(q), 18.0));
    const double alpha = sinO / (2.0 * Q);

    // RBJ LPF coefficients (12 dB/oct single-pole biquad)
    const double raw_b0 =  (1.0 - cosO) / 2.0;
    const double raw_b1 =   1.0 - cosO;
    const double raw_b2 =  (1.0 - cosO) / 2.0;
    const double a0     =   1.0 + alpha;
    const double raw_a1 =  -2.0 * cosO;
    const double raw_a2 =   1.0 - alpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setLowShelf(float frequencyHz, float gainDb)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    // Clamp gain to ±24 dB to prevent coefficient explosion
    const double dBclamped = std::max(-24.0, std::min(static_cast<double>(gainDb), 24.0));
    const double A  = std::pow(10.0, dBclamped / 40.0);
    const double sqA = std::sqrt(A);

    // Fixed shelf slope Q = 0.707 (Butterworth, maximally flat transition)
    // Equivalent to S=1 in the RBJ cookbook: alpha = sin/2 * sqrt((A+1/A)*(1/S-1)+2)
    // Simplified here to the classic S=1 formula:
    // alpha_S = sin/2 * sqrt((A+1/A)*(1+1)-2)  → simplifies to sin*sqrt(A)/sqrt(2) for S=1
    // We use the direct RBJ S=1 form with the sAQ notation.
    const double twoSqAAlpha = 2.0 * sqA * sinO / std::sqrt(2.0); // = 2*sqrt(A)*alpha_S=1

    const double raw_b0 =    A * ((A + 1.0) - (A - 1.0) * cosO + twoSqAAlpha);
    const double raw_b1 =  2.0 * A * ((A - 1.0) - (A + 1.0) * cosO);
    const double raw_b2 =    A * ((A + 1.0) - (A - 1.0) * cosO - twoSqAAlpha);
    const double a0     =       (A + 1.0) + (A - 1.0) * cosO + twoSqAAlpha;
    const double raw_a1 =  -2.0 * ((A - 1.0) + (A + 1.0) * cosO);
    const double raw_a2 =        (A + 1.0) + (A - 1.0) * cosO - twoSqAAlpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setHighShelf(float frequencyHz, float gainDb)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double dBclamped = std::max(-24.0, std::min(static_cast<double>(gainDb), 24.0));
    const double A  = std::pow(10.0, dBclamped / 40.0);
    const double sqA = std::sqrt(A);
    const double twoSqAAlpha = 2.0 * sqA * sinO / std::sqrt(2.0);

    const double raw_b0 =    A * ((A + 1.0) + (A - 1.0) * cosO + twoSqAAlpha);
    const double raw_b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosO);
    const double raw_b2 =    A * ((A + 1.0) + (A - 1.0) * cosO - twoSqAAlpha);
    const double a0     =       (A + 1.0) - (A - 1.0) * cosO + twoSqAAlpha;
    const double raw_a1 =   2.0 * ((A - 1.0) - (A + 1.0) * cosO);
    const double raw_a2 =        (A + 1.0) - (A - 1.0) * cosO - twoSqAAlpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setNotch(float frequencyHz, float q)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double Q     = std::max(0.1, std::min(static_cast<double>(q), 18.0));
    const double alpha = sinO / (2.0 * Q);

    // RBJ Notch: passes all frequencies but nulls exactly at f0
    const double raw_b0 =  1.0;
    const double raw_b1 = -2.0 * cosO;
    const double raw_b2 =  1.0;
    const double a0     =  1.0 + alpha;
    const double raw_a1 = -2.0 * cosO;
    const double raw_a2 =  1.0 - alpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setBandPass(float frequencyHz, float q)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double Q     = std::max(0.1, std::min(static_cast<double>(q), 18.0));
    const double alpha = sinO / (2.0 * Q);

    // RBJ BPF (constant peak gain = Q variant)
    // b0 = sin/2 = Q*alpha, b1 = 0, b2 = -sin/2
    const double raw_b0 =  sinO / 2.0;
    const double raw_b1 =  0.0;
    const double raw_b2 = -sinO / 2.0;
    const double a0     =  1.0 + alpha;
    const double raw_a1 = -2.0 * cosO;
    const double raw_a2 =  1.0 - alpha;

    double nb0, nb1, nb2, na1, na2;
    if (!normaliseCoeffs(raw_b0, raw_b1, raw_b2, raw_a1, raw_a2, a0,
                         nb0, nb1, nb2, na1, na2))
    {
        setBypassCoefficients();
        return;
    }

    b0 = nb0; b1 = nb1; b2 = nb2; a1 = na1; a2 = na2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setHighPassFirstOrder(float frequencyHz)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double tanHalf = std::tan(omega0 / 2.0);
    if (std::abs(tanHalf) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double k = 1.0 / tanHalf;
    const double a0 = k + 1.0;

    if (std::abs(a0) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double raw_b0 = k / a0;
    const double raw_b1 = -k / a0;
    const double raw_b2 = 0.0;
    const double raw_a1 = (1.0 - k) / a0;
    const double raw_a2 = 0.0;

    // Safety checks
    if (std::isnan(raw_b0) || std::isinf(raw_b0) ||
        std::isnan(raw_b1) || std::isinf(raw_b1) ||
        std::isnan(raw_a1) || std::isinf(raw_a1))
    {
        setBypassCoefficients();
        return;
    }

    b0 = raw_b0; b1 = raw_b1; b2 = raw_b2; a1 = raw_a1; a2 = raw_a2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setLowPassFirstOrder(float frequencyHz)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double tanHalf = std::tan(omega0 / 2.0);
    if (std::abs(tanHalf) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double k = 1.0 / tanHalf;
    const double a0 = k + 1.0;

    if (std::abs(a0) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double raw_b0 = 1.0 / a0;
    const double raw_b1 = 1.0 / a0;
    const double raw_b2 = 0.0;
    const double raw_a1 = (1.0 - k) / a0;
    const double raw_a2 = 0.0;

    // Safety checks
    if (std::isnan(raw_b0) || std::isinf(raw_b0) ||
        std::isnan(raw_b1) || std::isinf(raw_b1) ||
        std::isnan(raw_a1) || std::isinf(raw_a1))
    {
        setBypassCoefficients();
        return;
    }

    b0 = raw_b0; b1 = raw_b1; b2 = raw_b2; a1 = raw_a1; a2 = raw_a2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setLowShelfFirstOrder(float frequencyHz, float gainDb)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double dBclamped = std::max(-24.0, std::min(static_cast<double>(gainDb), 24.0));
    const double A = std::pow(10.0, dBclamped / 40.0);

    const double tanHalf = std::tan(omega0 / 2.0);
    if (std::abs(tanHalf) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double k = 1.0 / tanHalf;
    const double denom = A * k + 1.0;

    if (std::abs(denom) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double raw_b0 = A * (k + A) / denom;
    const double raw_b1 = A * (A - k) / denom;
    const double raw_b2 = 0.0;
    const double raw_a1 = (1.0 - A * k) / denom;
    const double raw_a2 = 0.0;

    // Safety checks
    if (std::isnan(raw_b0) || std::isinf(raw_b0) ||
        std::isnan(raw_b1) || std::isinf(raw_b1) ||
        std::isnan(raw_a1) || std::isinf(raw_a1))
    {
        setBypassCoefficients();
        return;
    }

    b0 = raw_b0; b1 = raw_b1; b2 = raw_b2; a1 = raw_a1; a2 = raw_a2;
}

// ---------------------------------------------------------------------------
void BiquadFilter::setHighShelfFirstOrder(float frequencyHz, float gainDb)
{
    double f0, omega0, sinO, cosO;
    if (!sanitizeCommonInputs(currentSampleRate, frequencyHz, f0, omega0, sinO, cosO))
    {
        setBypassCoefficients();
        return;
    }

    const double dBclamped = std::max(-24.0, std::min(static_cast<double>(gainDb), 24.0));
    const double A = std::pow(10.0, dBclamped / 40.0);

    const double tanHalf = std::tan(omega0 / 2.0);
    if (std::abs(tanHalf) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double k = 1.0 / tanHalf;
    const double denom = k + A;

    if (std::abs(denom) < 1e-12)
    {
        setBypassCoefficients();
        return;
    }

    const double raw_b0 = A * (A * k + 1.0) / denom;
    const double raw_b1 = A * (1.0 - A * k) / denom;
    const double raw_b2 = 0.0;
    const double raw_a1 = (A - k) / denom;
    const double raw_a2 = 0.0;

    // Safety checks
    if (std::isnan(raw_b0) || std::isinf(raw_b0) ||
        std::isnan(raw_b1) || std::isinf(raw_b1) ||
        std::isnan(raw_a1) || std::isinf(raw_a1))
    {
        setBypassCoefficients();
        return;
    }

    b0 = raw_b0; b1 = raw_b1; b2 = raw_b2; a1 = raw_a1; a2 = raw_a2;
}

float BiquadFilter::processSample(int channel, float inputSample) noexcept
{
    const size_t chanIdx = static_cast<size_t>(channel);
    
    // Bounds check to ensure safety
    if (channel < 0 || chanIdx >= z1.size())
        return inputSample;

    // Direct Form II Transposed:
    // y[n]  = b0 * x[n] + z1[n-1]
    // z1[n] = b1 * x[n] - a1 * y[n] + z2[n-1]
    // z2[n] = b2 * x[n] - a2 * y[n]
    
    const double x = static_cast<double>(inputSample);
    const double y = b0 * x + z1[chanIdx];

    z1[chanIdx] = b1 * x - a1 * y + z2[chanIdx];
    z2[chanIdx] = b2 * x - a2 * y;

    return static_cast<float>(y);
}

void BiquadFilter::processBlock(juce::AudioBuffer<float>& buffer) noexcept
{
    const int numSamples = buffer.getNumSamples();
    const int numChans = buffer.getNumChannels();

    // Verify state configuration matches channel count to prevent out of bounds
    if (static_cast<int>(z1.size()) < numChans)
    {
        // Re-allocate safety if channel count somehow mismatched dynamically (rare but safe)
        prepare(currentSampleRate, numChans);
    }

    for (int channel = 0; channel < numChans; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSample(channel, channelData[sample]);
        }
    }
}
