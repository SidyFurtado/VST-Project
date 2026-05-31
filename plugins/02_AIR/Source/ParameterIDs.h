#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// ==============================================================================
// LUMINAR — ASTRA Audio Suite
// Parameter IDs for AudioProcessorValueTreeState (APVTS)
//
// All IDs are prefixed with "luminar_" to prevent any collision with AUREQ or
// any other plugin in the monorepo.
//
// APVTS State Identifier: "LUMINAR_PARAMS"
// ==============================================================================

namespace LUMINAR::Params
{
    // --------------------------------------------------------------------------
    // Global parameter IDs
    // --------------------------------------------------------------------------

    /** Presence enhancer intensity (0.0 – 1.0, default 0.40). */
    inline juce::String presenceID()   { return "luminar_presence";   }

    /** Air exciter mix level (0.0 – 1.0, default 0.30). */
    inline juce::String airID()        { return "luminar_air";        }

    /** Global output gain in dB (-12.0 – +6.0, default 0.0). */
    inline juce::String outputGainID() { return "luminar_outputGain"; }

} // namespace LUMINAR::Params
