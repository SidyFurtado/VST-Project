#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>
#include <atomic>
#include <cmath>

// ==============================================================================
// SpectralNoiseReducer — Next-Gen Spectral Noise Reduction Engine
//
// A hybrid Wiener-filter / spectral subtraction engine with:
//   - 75% overlap-add with Hann window for artifact-free reconstruction
//   - Adaptive noise floor estimation via running minimum per band
//   - Spectral smoothing (adjacent-bin averaging) to eliminate musical noise
//   - Temporal smoothing with fast-attack / slow-release envelope
//   - Voice-activity detection to freeze noise profile during speech
//   - Oversubtraction factor for aggressive yet transparent noise removal
//
// Designed to deliver "zero quality loss" noise reduction even without AI.
// ==============================================================================

class SpectralNoiseReducer
{
public:
    // ==============================================================================
    // Construction / Destruction
    // ==============================================================================
    SpectralNoiseReducer();
    ~SpectralNoiseReducer();

    // ==============================================================================
    // Lifecycle
    // ==============================================================================
    /** Prepare the engine for the given sample rate and block size.
     *  fftSize must be a power of two (default 2048).
     *  hopSize is typically fftSize / 4 (75% overlap).
     */
    void prepare (double sampleRate, int fftSize = 2048, int hopSize = 0);

    /** Reset all internal state – call on transport stop / prepareToPlay. */
    void reset();

    // ==============================================================================
    // Core Processing
    // ==============================================================================
    /** Process a block of audio samples.
     *  Input and output can point to the same buffer (in-place).
     *  If learnNoise is true, the noise profile is updated from this block.
     *  Returns the number of valid output samples written (always numSamples).
     */
    int processBlock (const float* input, float* output, int numSamples,
                      bool learnNoise = false);

    // ==============================================================================
    // Parameters
    // ==============================================================================
    /** Maximum noise reduction in dB (0 = none, 40 = very aggressive). */
    void setReductionDb (float db);
    float getReductionDb() const noexcept { return maxReductionDb; }

    /** Noise floor in dB below which we stop reducing.
     *  Higher values = more natural sound, lower values = more aggressive.
     *  Typical: -60 dB for clean, -80 dB for aggressive. */
    void setNoiseFloorDb (float db);
    float getNoiseFloorDb() const noexcept { return noiseFloorDb; }

    /** Spectral smoothing: number of adjacent bins to average (1-9, odd).
     *  1 = no smoothing (may produce musical noise), 3-5 = recommended. */
    void setSpectralSmoothing (int bins);
    int getSpectralSmoothing() const noexcept { return spectralSmoothBins; }

    /** Temporal smoothing factor (0 = no smoothing, 1 = max smoothing).
     *  Higher values = smoother but slower response. */
    void setTemporalSmoothing (float factor);
    float getTemporalSmoothing() const noexcept { return temporalSmoothFactor; }

    /** Time constant for gain attack in milliseconds. */
    void setAttackMs (float ms);
    float getAttackMs() const noexcept { return attackMs; }

    /** Time constant for gain release in milliseconds. */
    void setReleaseMs (float ms);
    float getReleaseMs() const noexcept { return releaseMs; }

    /** Learning rate for noise profile estimation (0 = frozen, 0.001 = slow, 1 = instant). */
    void setLearnRate (float rate);
    float getLearnRate() const noexcept { return learnRate; }

    /** Oversubtraction factor (1.0 = standard, 1.5-3.0 = aggressive but may distort).
     *  Higher values remove more noise but can affect speech quality. */
    void setOverSubtractionFactor (float factor);
    float getOverSubtractionFactor() const noexcept { return overSubFactor; }

    // ==============================================================================
    // Noise Profile Management
    // ==============================================================================
    /** Force the noise profile to be learned from the next block of audio. */
    void startLearnProfile();

    /** Freeze the current noise profile (stop learning). */
    void freezeProfile();

    /** Unfreeze the noise profile (enable adaptive learning). */
    void unfreezeProfile();

    bool isProfileFrozen() const noexcept { return profileFrozen; }

    /** Get a pointer to the current noise profile (magnitude spectrum).
     *  Only valid after prepare() has been called. */
    const float* getNoiseProfile() const noexcept { return noiseProfile.data(); }

    /** Get the noise estimate in dB at a specific FFT bin index. */
    float getNoiseBinDb (int bin) const noexcept;

    // ==============================================================================
    // Metering
    // ==============================================================================
    /** Get the current gain reduction in dB (smoothed for display). */
    float getCurrentReductionDb() const noexcept { return currentReductionDb.load(); }

    /** Get the current input RMS level in dB. */
    float getInputRmsDb() const noexcept { return inputRmsDb; }

    /** Get the current output RMS level in dB. */
    float getOutputRmsDb() const noexcept { return outputRmsDb; }

    /** Get the estimated noise floor in dB (overall broadband estimate). */
    float getEstimatedNoiseFloorDb() const noexcept;

private:
    // ==============================================================================
    // Internal processing
    // ==============================================================================
    void processFrame();
    void computeGains();
    void applySpectralSmoothing();
    void applyTemporalSmoothing();
    void updateNoiseProfile();
    void performOLA();

    // ==============================================================================
    // FFT / IFFT state
    // ==============================================================================
    int fftOrder  = 11;    // 2^11 = 2048
    int fftSize   = 2048;
    int hopSize   = 512;   // 25% hop = 75% overlap
    int numBins   = 1024;  // fftSize / 2

    std::unique_ptr<juce::dsp::FFT> fft;
    std::vector<float> window;           // Hann window
    std::vector<float> fftTimeData;      // Time-domain data for FFT
    std::vector<float> fftFreqData;      // Frequency-domain data (real interleaved)
    std::vector<float> magnitudeSpectrum; // |X| for each bin
    std::vector<float> phaseSpectrum;     // arg(X) for each bin

    // ==============================================================================
    // Noise profile
    // ==============================================================================
    std::vector<float> noiseProfile;     // Estimated noise magnitude per bin
    std::vector<float> noiseMinTracker;  // Running minimum tracker
    int noiseMinCounter = 0;
    static constexpr int NOISE_MIN_FRAMES = 32; // ~0.7s at 48kHz/2048
    float minTrackerResetTimer = 0.0f;
    float minTrackerResetInterval = 0.0f;

    bool learningRequested = false;
    bool profileFrozen = false;
    int learnFrames = 0;
    static constexpr int MIN_LEARN_FRAMES = 8;  // Require at least 8 frames of learning

    // ==============================================================================
    // Gain computation
    // ==============================================================================
    std::vector<float> rawGains;         // Per-bin gains before smoothing
    std::vector<float> smoothedGains;    // Gains after temporal smoothing
    std::vector<float> prevGains;        // Previous gains for temporal smoothing

    float maxReductionDb       = 30.0f;
    float noiseFloorDb         = -70.0f;
    int   spectralSmoothBins   = 3;      // Average 3 adjacent bins
    float temporalSmoothFactor = 0.7f;   // 0-1
    float overSubFactor        = 1.8f;   // Oversubtraction

    // ==============================================================================
    // Envelope detection
    // ==============================================================================
    float attackMs    = 10.0f;
    float releaseMs   = 150.0f;
    float attackCoeff  = 0.0f;
    float releaseCoeff = 0.0f;
    float learnRate   = 0.01f;

    // ==============================================================================
    // Overlap-Add ring buffer
    // ==============================================================================
    std::vector<float> olaBuffer;
    int olaWritePos = 0;

    // ==============================================================================
    // Spectral smoothing scratch buffer (pre-allocated, zero heap alloc in audio thread)
    // ==============================================================================
    std::vector<float> smoothScratchBuffer;

    // ==============================================================================
    // RMS metering
    // ==============================================================================
    float inputRmsSum   = 0.0f;
    float outputRmsSum  = 0.0f;
    float inputRmsDb    = -90.0f;
    float outputRmsDb   = -90.0f;
    int   rmsCount      = 0;
    static constexpr int RMS_WINDOW = 480; // ~10ms at 48kHz

    std::atomic<float> currentReductionDb { 0.0f };
    float smoothedMeterDb = 0.0f;

    // ==============================================================================
    // State
    // ==============================================================================
    double sampleRate = 48000.0;
    bool prepared     = false;
    bool processing   = false; // Re-entrancy guard

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralNoiseReducer)
};
