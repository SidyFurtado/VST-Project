#pragma once

#include <juce_core/juce_core.h>

namespace VoidParams
{
    // ---- AI / ONNX Parameters ----
    inline constexpr const char* bypass()          { return "bypass"; }
    inline constexpr const char* vacuumIntensity() { return "vacuum_intensity"; }

    // ---- DSP Noise Reduction Parameters ----
    inline constexpr const char* noiseThreshold() { return "noise_threshold"; }
    inline constexpr const char* noiseFloor()     { return "noise_floor"; }
    inline constexpr const char* reductionAmount() { return "reduction_amount"; }
    inline constexpr const char* attackTime()     { return "attack_time"; }
    inline constexpr const char* releaseTime()    { return "release_time"; }
    inline constexpr const char* spectralSmooth() { return "spectral_smooth"; }
    inline constexpr const char* learnNoise()     { return "learn_noise"; }

    // ---- Model path (persisted, not shown as automatable parameter) ----
    inline constexpr const char* modelPath()      { return "model_path"; }

    // ---- Preset selector ----
    // 0 = custom, 1-4 = factory presets
    inline constexpr const char* preset()         { return "preset"; }

    // ---- Mode ----
    // 0 = AI (ONNX only), 1 = DSP (algorithmic), 2 = Hybrid (both)
    inline constexpr const char* reductionMode()  { return "reduction_mode"; }

    // ---- Parameter ranges ----
    namespace Ranges
    {
        inline constexpr float thresholdMin  = -80.0f;
        inline constexpr float thresholdMax  = 0.0f;
        inline constexpr float thresholdStep = 0.5f;
        inline constexpr float thresholdDefault = -40.0f;

        inline constexpr float floorMin  = -100.0f;
        inline constexpr float floorMax  = -20.0f;
        inline constexpr float floorStep = 0.5f;
        inline constexpr float floorDefault = -70.0f;

        inline constexpr float amountMin  = 0.0f;
        inline constexpr float amountMax  = 100.0f;
        inline constexpr float amountStep = 0.5f;
        inline constexpr float amountDefault = 70.0f;

        inline constexpr float attackMin  = 1.0f;
        inline constexpr float attackMax  = 200.0f;
        inline constexpr float attackStep = 1.0f;
        inline constexpr float attackDefault = 10.0f;

        inline constexpr float releaseMin  = 10.0f;
        inline constexpr float releaseMax  = 2000.0f;
        inline constexpr float releaseStep = 10.0f;
        inline constexpr float releaseDefault = 200.0f;

        inline constexpr float smoothMin  = 1.0f;
        inline constexpr float smoothMax  = 9.0f;
        inline constexpr float smoothStep = 2.0f;
        inline constexpr float smoothDefault = 3.0f;
    }
}
