#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// ==============================================================================
// GRAVITY — ASTRA Audio Suite
// Parameter IDs for AudioProcessorValueTreeState (APVTS)
//
// All IDs are prefixed with "gravity_" to prevent any collision with AUREQ,
// LUMINAR, or any other plugin in the monorepo.
//
// APVTS State Identifier: "GRAVITY_PARAMS"
// ==============================================================================

namespace GRAVITY::Params
{
    // --------------------------------------------------------------------------
    // Compressor parameters
    // --------------------------------------------------------------------------

    /** Threshold in dB (-60.0 – 0.0, default -18.0). */
    inline juce::String threshID()   { return "gravity_thresh";   }

    /** Compression ratio (1.0 – 50.0, default 4.0). */
    inline juce::String ratioID()    { return "gravity_ratio";    }

    /** Attack time in ms (0.1 – 500.0, default 10.0). */
    inline juce::String attackID()   { return "gravity_attack";   }

    /** Release time in ms (5.0 – 3000.0, default 150.0). */
    inline juce::String releaseID()  { return "gravity_release";  }

    /** Output makeup gain in dB (0.0 – 30.0, default 0.0). */
    inline juce::String gainID()     { return "gravity_gain";     }

    /** Detection mode: 0 = Electro (Peak), 1 = Opto (RMS). */
    inline juce::String modeID()     { return "gravity_mode";     }

    /** Auto Release Control: 0 = Off, 1 = On. */
    inline juce::String arcID()      { return "gravity_arc";      }

} // namespace GRAVITY::Params
