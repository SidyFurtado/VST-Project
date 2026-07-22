#include "PresetManager.h"
#include "ParameterIDs.h"
#include <initializer_list>

namespace AUREQ
{
    namespace
    {
        struct BandSpec
        {
            int band = 0;
            float type = 0.0f;
            float frequency = 1000.0f;
            float gain = 0.0f;
            float q = 1.0f;
            float slope = 0.0f;
            float channelMode = 0.0f;
            float dynamicRange = 0.0f;
            float dynamicThreshold = -20.0f;
            float dynamicAttack = 10.0f;
            float dynamicRelease = 120.0f;
            bool usesSlope = false;
            bool usesChannelMode = false;
            bool usesDynamic = false;
        };

        constexpr float bellType = 0.0f;
        constexpr float lowCutType = 1.0f;
        constexpr float highCutType = 2.0f;
        constexpr float lowShelfType = 3.0f;
        constexpr float highShelfType = 4.0f;
        constexpr float slope6  = 0.0f;
        constexpr float slope12 = 1.0f;
        constexpr float slope18 = 2.0f;
        constexpr float slope24 = 3.0f;
        constexpr float slope48 = 3.0f; // Map legacy 48 dB/oct to 24 dB/oct
        constexpr float midMode = 1.0f;
        constexpr float sideMode = 2.0f;

        BandSpec bell (int band, float frequency, float gain, float q = 1.0f, float channelMode = 0.0f)
        {
            BandSpec spec { band, bellType, frequency, gain, q };
            spec.usesChannelMode = channelMode != 0.0f;
            spec.channelMode = channelMode;
            return spec;
        }

        BandSpec lowShelf (int band, float frequency, float gain, float q = 0.8f, float channelMode = 0.0f)
        {
            BandSpec spec { band, lowShelfType, frequency, gain, q };
            spec.usesChannelMode = channelMode != 0.0f;
            spec.channelMode = channelMode;
            return spec;
        }

        BandSpec highShelf (int band, float frequency, float gain, float q = 0.7f, float channelMode = 0.0f)
        {
            BandSpec spec { band, highShelfType, frequency, gain, q };
            spec.usesChannelMode = channelMode != 0.0f;
            spec.channelMode = channelMode;
            return spec;
        }

        BandSpec lowCut (int band, float frequency, float slope = slope24, float channelMode = 0.0f)
        {
            BandSpec spec { band, lowCutType, frequency };
            spec.usesSlope = true;
            spec.slope = slope;
            spec.usesChannelMode = channelMode != 0.0f;
            spec.channelMode = channelMode;
            return spec;
        }

        BandSpec highCut (int band, float frequency, float slope = slope12, float channelMode = 0.0f)
        {
            BandSpec spec { band, highCutType, frequency };
            spec.usesSlope = true;
            spec.slope = slope;
            spec.usesChannelMode = channelMode != 0.0f;
            spec.channelMode = channelMode;
            return spec;
        }

        BandSpec dynamicBell (int band, float frequency, float range, float q,
                              float threshold = -20.0f, float attack = 10.0f,
                              float release = 120.0f, float staticGain = 0.0f,
                              float channelMode = 0.0f)
        {
            auto spec = bell (band, frequency, staticGain, q, channelMode);
            spec.usesDynamic = true;
            spec.dynamicRange = range;
            spec.dynamicThreshold = threshold;
            spec.dynamicAttack = attack;
            spec.dynamicRelease = release;
            return spec;
        }

        Preset makePresetFromBands (const char* name,
                                    const char* category,
                                    const char* subcategory,
                                    std::initializer_list<const char*> tags,
                                    std::initializer_list<BandSpec> bands)
        {
            Preset preset;
            preset.name = name;
            preset.category = category;
            preset.subcategory = subcategory;
            for (auto* tag : tags)
                preset.tags.add (tag);

            for (const auto& band : bands)
            {
                preset.parameters.push_back ({ Params::bandEnabledID (band.band), 1.0f });
                preset.parameters.push_back ({ Params::bandTypeID (band.band), band.type });
                preset.parameters.push_back ({ Params::bandFrequencyID (band.band), band.frequency });

                if (band.type == lowCutType || band.type == highCutType)
                {
                    if (band.usesSlope)
                        preset.parameters.push_back ({ Params::bandSlopeID (band.band), band.slope });
                }
                else
                {
                    preset.parameters.push_back ({ Params::bandGainID (band.band), band.gain });
                    preset.parameters.push_back ({ Params::bandQID (band.band), band.q });
                }

                if (band.usesChannelMode)
                    preset.parameters.push_back ({ Params::bandChannelModeID (band.band), band.channelMode });

                if (band.usesDynamic)
                {
                    preset.parameters.push_back ({ Params::bandDynamicEnabledID (band.band), 1.0f });
                    preset.parameters.push_back ({ Params::bandDynamicRangeID (band.band), band.dynamicRange });
                    preset.parameters.push_back ({ Params::bandDynamicThresholdID (band.band), band.dynamicThreshold });
                    preset.parameters.push_back ({ Params::bandDynamicAttackID (band.band), band.dynamicAttack });
                    preset.parameters.push_back ({ Params::bandDynamicReleaseID (band.band), band.dynamicRelease });
                }
            }

            return preset;
        }
    }

    PresetManager::PresetManager (juce::AudioProcessorValueTreeState& apvtsToUse)
        : apvts (apvtsToUse)
    {
        initializePresets();
        currentPresetName = getPresetName (currentPresetIndex);
        refreshUserPresets();
    }

    juce::String PresetManager::getPresetName (int index) const
    {
        if (index >= 0 && index < getNumPresets())
            return presets[static_cast<size_t> (index)].name;
        return {};
    }

    juce::String PresetManager::getPresetCategory (int index) const
    {
        if (index >= 0 && index < getNumPresets())
            return presets[static_cast<size_t> (index)].category;
        return {};
    }

    juce::String PresetManager::getPresetSubcategory (int index) const
    {
        if (index >= 0 && index < getNumPresets())
            return presets[static_cast<size_t> (index)].subcategory;
        return {};
    }

    juce::StringArray PresetManager::getPresetTags (int index) const
    {
        if (index >= 0 && index < getNumPresets())
            return presets[static_cast<size_t> (index)].tags;
        return {};
    }

    juce::String PresetManager::getCurrentPresetName() const
    {
        return currentPresetName.isNotEmpty() ? currentPresetName : getPresetName (currentPresetIndex);
    }

    void PresetManager::loadPreset (int index)
    {
        if (index < 0 || index >= getNumPresets())
            return;

        currentPresetIndex = index;
        currentUserPresetIndex = -1;
        const auto& preset = presets[static_cast<size_t> (index)];
        currentPresetName = preset.name;

        // Begin transaction gesture for all parameters to notify host correctly
        for (int b = 0; b < Params::numBands; ++b)
        {
            // Reset core band parameters
            if (auto* param = apvts.getParameter (Params::bandEnabledID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (b == 0 ? 1.0f : 0.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandTypeID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f)); // Bell
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandFrequencyID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (Params::defaultBandFrequency (b)));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandGainID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandQID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (1.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandBypassID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f)); // False
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandSlopeID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f)); // 12 dB/oct
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandDynamicEnabledID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f)); // False
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandDynamicRangeID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandDynamicThresholdID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (-24.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandDynamicAttackID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (10.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandDynamicReleaseID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (120.0f));
                param->endChangeGesture();
            }

            if (auto* param = apvts.getParameter (Params::bandChannelModeID (b)))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (0.0f)); // Stereo
                param->endChangeGesture();
            }
        }

        // Apply preset specific parameters
        for (const auto& pParam : preset.parameters)
        {
            if (auto* param = apvts.getParameter (pParam.id))
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (param->convertTo0to1 (pParam.value));
                param->endChangeGesture();
            }
        }
    }

    void PresetManager::refreshUserPresets()
    {
        userPresets.clear();

        auto presetDirectory = getUserPresetDirectory();
        if (! presetDirectory.exists())
            return;

        juce::Array<juce::File> files;
        presetDirectory.findChildFiles (files, juce::File::findFiles, false, "*.aureqpreset");
        files.sort();

        for (const auto& file : files)
        {
            UserPreset preset;
            preset.name = file.getFileNameWithoutExtension();
            preset.file = file;
            userPresets.push_back (preset);
        }
    }

    juce::String PresetManager::getUserPresetName (int index) const
    {
        if (index >= 0 && index < getNumUserPresets())
            return userPresets[static_cast<size_t> (index)].name;
        return {};
    }

    bool PresetManager::loadUserPreset (int index)
    {
        if (index < 0 || index >= getNumUserPresets())
            return false;

        const auto& userPreset = userPresets[static_cast<size_t> (index)];
        if (! userPreset.file.existsAsFile())
            return false;

        std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (userPreset.file));
        if (xml == nullptr || ! xml->hasTagName (apvts.state.getType()))
            return false;

        auto state = juce::ValueTree::fromXml (*xml);
        if (! state.isValid())
            return false;

        apvts.replaceState (state);
        currentUserPresetIndex = index;
        currentPresetName = userPreset.name;
        return true;
    }

    bool PresetManager::saveCurrentUserPreset (const juce::String& name)
    {
        auto sanitizedName = sanitizePresetName (name);
        if (sanitizedName.isEmpty())
            return false;

        auto presetDirectory = getUserPresetDirectory();
        if (! presetDirectory.exists() && ! presetDirectory.createDirectory())
            return false;

        auto targetFile = getAvailableUserPresetFile (sanitizedName);
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml (state.createXml());

        if (xml == nullptr || ! xml->writeTo (targetFile))
            return false;

        refreshUserPresets();
        currentUserPresetIndex = -1;

        for (int i = 0; i < getNumUserPresets(); ++i)
        {
            if (userPresets[static_cast<size_t> (i)].file == targetFile)
            {
                currentUserPresetIndex = i;
                break;
            }
        }

        currentPresetName = targetFile.getFileNameWithoutExtension();
        return true;
    }

    juce::File PresetManager::getUserPresetFile (int index) const
    {
        if (index >= 0 && index < getNumUserPresets())
            return userPresets[static_cast<size_t> (index)].file;
        return {};
    }

    juce::File PresetManager::getUserPresetDirectoryForUser() const
    {
        return getUserPresetDirectory();
    }

    bool PresetManager::revealUserPresetDirectory() const
    {
        auto presetDirectory = getUserPresetDirectory();

        if (! presetDirectory.exists() && ! presetDirectory.createDirectory())
            return false;

        presetDirectory.revealToUser();
        return true;
    }

    bool PresetManager::deleteUserPreset (int index)
    {
        if (index < 0 || index >= getNumUserPresets())
            return false;

        auto targetFile = userPresets[static_cast<size_t> (index)].file;
        auto presetDirectory = getUserPresetDirectory();

        if (! targetFile.existsAsFile())
            return false;

        if (targetFile.getFileExtension() != ".aureqpreset")
            return false;

        if (targetFile.getParentDirectory() != presetDirectory)
            return false;

        if (! targetFile.moveToTrash())
            return false;

        refreshUserPresets();

        if (currentUserPresetIndex == index)
            currentUserPresetIndex = -1;
        else if (currentUserPresetIndex > index)
            --currentUserPresetIndex;

        return true;
    }

    juce::File PresetManager::getUserPresetDirectory()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
            .getChildFile ("AUREQ")
            .getChildFile ("Presets");
    }

    juce::String PresetManager::sanitizePresetName (const juce::String& name)
    {
        auto sanitized = name.trim();
        const juce::String invalidCharacters { "/\\:*?\"<>|" };

        for (auto character : invalidCharacters)
            sanitized = sanitized.replaceCharacter (character, '-');

        while (sanitized.contains ("  "))
            sanitized = sanitized.replace ("  ", " ");

        while (sanitized.contains ("--"))
            sanitized = sanitized.replace ("--", "-");

        return sanitized.trim().trimCharactersAtStart ("-. ").trimCharactersAtEnd ("-. ");
    }

    juce::File PresetManager::getAvailableUserPresetFile (const juce::String& sanitizedName)
    {
        auto presetDirectory = getUserPresetDirectory();
        auto candidate = presetDirectory.getChildFile (sanitizedName).withFileExtension (".aureqpreset");

        if (! candidate.existsAsFile())
            return candidate;

        for (int suffix = 2; suffix < 10000; ++suffix)
        {
            auto numberedName = sanitizedName + " " + juce::String (suffix);
            candidate = presetDirectory.getChildFile (numberedName).withFileExtension (".aureqpreset");

            if (! candidate.existsAsFile())
                return candidate;
        }

        return presetDirectory.getNonexistentChildFile (sanitizedName, ".aureqpreset", false);
    }

    void PresetManager::initializePresets()
    {
        presets.clear();
        presets.reserve (148);

        auto add = [this] (const Preset& preset)
        {
            presets.push_back (preset);
        };

        add (makePresetFromBands ("Default Flat", "Utility / Cleanup", "Default", { "flat", "neutral" }, {}));

        add (makePresetFromBands ("Vocal Presence Safe", "Vocals", "Lead Vocal", { "vocal", "presence", "toolbox" }, { lowCut (0, 80.0f), bell (3, 2800.0f, 1.6f), highShelf (6, 11000.0f, 0.8f) }));
        add (makePresetFromBands ("Vocal De-Esser Dynamic", "Vocals", "Lead Vocal", { "vocal", "de-esser", "dynamic", "toolbox" }, { lowCut (0, 85.0f), dynamicBell (5, 6600.0f, -5.0f, 1.7f, -20.0f, 5.0f, 90.0f) }));
        add (makePresetFromBands ("Vocal Low-Mid Cleanup", "Vocals", "Lead Vocal", { "vocal", "cleanup", "toolbox" }, { lowCut (0, 75.0f), bell (2, 280.0f, -2.5f), bell (3, 520.0f, -1.4f, 1.1f) }));
        add (makePresetFromBands ("Backing Vocal Tuck", "Vocals", "Backing Vocal", { "backing", "vocal", "toolbox" }, { lowCut (0, 125.0f), bell (2, 450.0f, -1.8f), bell (4, 2800.0f, -1.0f), highShelf (6, 10000.0f, 0.6f, 0.7f, sideMode) }));
        add (makePresetFromBands ("Kick Punch Toolkit", "Drums", "Kick", { "kick", "punch", "toolbox" }, { lowCut (0, 28.0f), bell (1, 75.0f, 1.8f), bell (2, 260.0f, -2.2f), bell (4, 3300.0f, 1.1f) }));
        add (makePresetFromBands ("Snare Body Toolkit", "Drums", "Snare", { "snare", "body", "toolbox" }, { lowCut (0, 85.0f, slope12), bell (2, 210.0f, 1.6f), bell (3, 620.0f, -1.6f, 1.1f), dynamicBell (5, 5000.0f, -2.0f, 1.3f) }));
        add (makePresetFromBands ("Drum Bus Punch Safe", "Drums", "Drum Bus", { "drums", "bus", "toolbox" }, { lowCut (0, 30.0f, slope12), lowShelf (1, 80.0f, 0.9f), bell (2, 320.0f, -1.0f, 0.9f), highShelf (6, 11000.0f, 0.6f) }));
        add (makePresetFromBands ("Bass Tight Low", "Bass / 808", "Bass", { "bass", "tight", "toolbox" }, { lowCut (0, 28.0f, slope12), bell (1, 90.0f, 1.2f, 0.9f), bell (2, 250.0f, -2.0f) }));
        add (makePresetFromBands ("Bass Side Cleanup", "Bass / 808", "Bass", { "bass", "side", "cleanup", "toolbox" }, { lowCut (0, 35.0f, slope12), lowCut (1, 140.0f, slope24, sideMode) }));
        add (makePresetFromBands ("808 Note Tamer Dynamic", "Bass / 808", "808", { "808", "dynamic", "toolbox" }, { lowCut (0, 24.0f, slope12), dynamicBell (1, 58.0f, -3.0f, 1.0f, -18.0f, 18.0f, 160.0f), bell (3, 220.0f, -1.2f) }));
        add (makePresetFromBands ("Acoustic Guitar Clarity", "Instruments", "Acoustic Guitar", { "acoustic", "guitar", "toolbox" }, { lowCut (0, 95.0f), bell (2, 260.0f, -1.8f), bell (4, 4200.0f, 1.2f), dynamicBell (5, 5200.0f, -2.0f, 1.3f) }));
        add (makePresetFromBands ("Electric Guitar Fizz Control", "Instruments", "Electric Guitar", { "electric", "guitar", "toolbox" }, { lowCut (0, 90.0f), bell (2, 320.0f, -1.4f), dynamicBell (4, 3800.0f, -2.5f, 1.3f), highCut (6, 11000.0f) }));
        add (makePresetFromBands ("Piano Keys Clarity", "Instruments", "Piano / Keys", { "piano", "keys", "toolbox" }, { lowCut (0, 55.0f, slope12), bell (2, 320.0f, -1.5f, 0.9f), bell (4, 2800.0f, 1.1f), highShelf (6, 10000.0f, 0.6f) }));
        add (makePresetFromBands ("Master Air Safe", "Mix Bus / Master", "Master", { "master", "air", "toolbox" }, { lowCut (0, 24.0f, slope12), bell (2, 320.0f, -0.5f, 0.8f), highShelf (6, 12500.0f, 0.7f) }));
        add (makePresetFromBands ("Mix Bus Gentle Lift", "Mix Bus / Master", "Mix Bus", { "mix bus", "lift", "toolbox" }, { lowCut (0, 25.0f, slope12), bell (2, 300.0f, -0.7f, 0.8f), bell (4, 2800.0f, 0.5f), highShelf (6, 11500.0f, 0.6f) }));
        add (makePresetFromBands ("Stereo Side Low Cleanup", "Mix Bus / Master", "Stereo", { "stereo", "side", "cleanup", "toolbox" }, { lowCut (1, 125.0f, slope24, sideMode), highShelf (6, 11000.0f, 0.5f, 0.7f, sideMode) }));
        add (makePresetFromBands ("Dynamic De-Harsh 3k", "Dynamic EQ", "Harshness", { "dynamic", "de-harsh", "toolbox" }, { dynamicBell (4, 3200.0f, -4.0f, 1.3f, -19.0f, 12.0f, 150.0f) }));
        add (makePresetFromBands ("Dynamic Sibilance 6k", "Dynamic EQ", "Sibilance", { "dynamic", "sibilance", "toolbox" }, { dynamicBell (5, 6500.0f, -5.0f, 1.7f, -20.0f, 4.0f, 90.0f) }));
        add (makePresetFromBands ("Dynamic Low-Mid Control", "Dynamic EQ", "Low-Mid", { "dynamic", "low-mid", "toolbox" }, { dynamicBell (2, 320.0f, -3.5f, 1.0f, -22.0f, 20.0f, 180.0f) }));
        add (makePresetFromBands ("Low Cut Cleanup", "Utility / Cleanup", "Cleanup", { "cleanup", "low cut", "toolbox" }, { lowCut (0, 35.0f, slope48), lowCut (1, 120.0f, slope24, sideMode) }));
        add (makePresetFromBands ("Sub Rumble Clean", "Utility / Cleanup", "Cleanup", { "sub", "rumble", "toolbox" }, { lowCut (0, 30.0f, slope48) }));

        const auto addGenre = [&add] (const char* name, const char* genre, const char* source, std::initializer_list<const char*> tags, std::initializer_list<BandSpec> bands)
        {
            juce::StringArray fullTags;
            fullTags.add (juce::String (genre).toLowerCase());
            fullTags.add (juce::String ("source:") + source);
            for (auto* tag : tags)
                fullTags.add (tag);

            Preset preset;
            preset.name = name;
            preset.category = "Genres";
            preset.subcategory = genre;
            preset.tags = fullTags;
            for (const auto& band : bands)
            {
                auto bandPreset = makePresetFromBands ("", "", "", {}, { band });
                preset.parameters.insert (preset.parameters.end(), bandPreset.parameters.begin(), bandPreset.parameters.end());
            }
            add (preset);
        };

        // Forro
        addGenre ("Forro Vocal Front Clarity", "Forro", "vocal", { "presence" }, { lowCut (0, 75.0f), bell (2, 320.0f, -1.2f), bell (4, 2600.0f, 1.8f) });
        addGenre ("Forro Vocal De-Harsh Safe", "Forro", "vocal", { "de-harsh" }, { lowCut (0, 75.0f), dynamicBell (4, 3200.0f, -3.0f, 1.3f), dynamicBell (5, 6500.0f, -3.5f, 1.6f) });
        addGenre ("Forro Vocal Boxy Cleanup", "Forro", "vocal", { "cleanup" }, { lowCut (0, 80.0f), bell (2, 430.0f, -2.4f, 1.1f), bell (3, 900.0f, -1.1f, 1.1f) });
        addGenre ("Forro Backing Vocal Pocket", "Forro", "backing vocal", { "cleanup" }, { lowCut (0, 130.0f), bell (2, 500.0f, -1.8f), bell (4, 2800.0f, -0.8f) });
        addGenre ("Forro Backing Vocal Air Tuck", "Forro", "backing vocal", { "air" }, { lowCut (0, 140.0f), bell (3, 2600.0f, -0.8f), highShelf (6, 10500.0f, 0.7f, 0.7f, sideMode) });
        addGenre ("Forro Accordion Presence", "Forro", "accordion", { "presence" }, { lowCut (0, 90.0f), bell (3, 1400.0f, 1.2f), bell (5, 3500.0f, 0.9f) });
        addGenre ("Forro Accordion Nasal Control", "Forro", "accordion", { "nasal" }, { lowCut (0, 95.0f), bell (3, 950.0f, -2.2f, 1.4f), dynamicBell (5, 3300.0f, -1.8f, 1.2f) });
        addGenre ("Forro Accordion Harsh Reed Tame", "Forro", "accordion", { "de-harsh" }, { lowCut (0, 100.0f), dynamicBell (5, 4200.0f, -2.8f, 1.4f), highShelf (6, 12000.0f, -0.4f) });
        addGenre ("Forro Zabumba Low Control", "Forro", "zabumba", { "low control" }, { lowCut (0, 32.0f), bell (1, 80.0f, 1.3f), bell (2, 280.0f, -1.8f) });
        addGenre ("Forro Zabumba Mud Cleanup", "Forro", "zabumba", { "cleanup" }, { lowCut (0, 35.0f), bell (2, 240.0f, -2.4f, 1.1f), bell (4, 2500.0f, 0.8f) });
        addGenre ("Forro Percussion Bright Detail", "Forro", "percussion", { "detail" }, { lowCut (0, 150.0f), dynamicBell (5, 6500.0f, -2.2f, 1.4f), highShelf (6, 10500.0f, 0.8f) });
        addGenre ("Forro Bass Warm Lock", "Forro", "bass", { "low control" }, { lowCut (0, 30.0f, slope12), bell (1, 95.0f, 1.0f, 0.9f), bell (2, 260.0f, -1.6f) });
        addGenre ("Forro Mix Dance Cleanup", "Forro", "mix bus", { "mix bus" }, { lowCut (0, 30.0f, slope12), bell (2, 300.0f, -0.9f, 0.8f), highShelf (6, 11000.0f, 0.6f) });
        addGenre ("Forro Mix Gentle Air", "Forro", "mix bus", { "air" }, { bell (2, 350.0f, -0.6f, 0.8f), highShelf (6, 12000.0f, 0.6f), highShelf (7, 11000.0f, 0.4f, 0.7f, sideMode) });

        // Trap
        addGenre ("Trap Vocal Cut Through", "Trap", "vocal", { "presence" }, { lowCut (0, 100.0f), bell (2, 280.0f, -1.3f), bell (4, 2700.0f, 1.8f), dynamicBell (5, 7000.0f, -2.5f, 1.6f) });
        addGenre ("Trap Vocal De-Esser", "Trap", "vocal", { "de-esser" }, { lowCut (0, 100.0f), dynamicBell (5, 6800.0f, -5.0f, 1.8f, -20.0f, 4.0f, 85.0f), highShelf (6, 11500.0f, 0.5f) });
        addGenre ("Trap Vocal Harsh 3k Control", "Trap", "vocal", { "de-harsh" }, { lowCut (0, 95.0f), dynamicBell (4, 3300.0f, -3.5f, 1.3f), bell (2, 300.0f, -1.1f) });
        addGenre ("Trap Vocal Boxy Cleanup", "Trap", "vocal", { "cleanup" }, { lowCut (0, 100.0f), bell (2, 380.0f, -2.4f, 1.1f), bell (3, 760.0f, -1.2f, 1.1f) });
        addGenre ("Trap Adlibs Side Air", "Trap", "adlibs", { "width" }, { lowCut (0, 140.0f), bell (4, 2500.0f, -0.8f, 1.0f, midMode), highShelf (6, 10500.0f, 1.1f, 0.7f, sideMode) });
        addGenre ("Trap Adlibs Sibilance Tuck", "Trap", "adlibs", { "de-esser" }, { lowCut (0, 140.0f), dynamicBell (5, 6500.0f, -5.0f, 1.8f), highShelf (6, 12000.0f, -0.4f) });
        addGenre ("Trap 808 Tight Mono Focus", "Trap", "808", { "low control" }, { lowCut (0, 24.0f), bell (2, 190.0f, -1.5f), lowCut (3, 140.0f, slope24, sideMode) });
        addGenre ("Trap 808 Sub Rumble Control", "Trap", "808", { "rumble" }, { lowCut (0, 22.0f, slope24), dynamicBell (1, 48.0f, -2.5f, 0.9f, -18.0f, 20.0f, 180.0f), bell (3, 240.0f, -1.3f) });
        addGenre ("Trap 808 Note Tamer Dynamic", "Trap", "808", { "dynamic" }, { lowCut (0, 24.0f, slope12), dynamicBell (1, 58.0f, -3.2f, 1.0f, -18.0f, 18.0f, 160.0f), bell (3, 220.0f, -1.2f) });
        addGenre ("Trap Kick Sub Boundary", "Trap", "kick", { "punch" }, { lowCut (0, 28.0f), bell (1, 72.0f, 1.3f), bell (2, 180.0f, -1.5f), bell (4, 3000.0f, 0.8f) });
        addGenre ("Trap Clap Snap Tame", "Trap", "snare / clap", { "de-harsh" }, { lowCut (0, 160.0f), bell (4, 2200.0f, 1.0f), dynamicBell (5, 5500.0f, -3.0f, 1.4f) });
        addGenre ("Trap Hi-Hat Softener", "Trap", "hi-hat", { "de-harsh" }, { lowCut (0, 350.0f), bell (5, 7500.0f, -2.0f, 1.4f), highShelf (6, 13000.0f, -0.6f) });
        addGenre ("Trap Synth Lead De-Harsh", "Trap", "synth", { "de-harsh" }, { lowCut (0, 120.0f), dynamicBell (4, 3500.0f, -3.0f, 1.3f), highCut (6, 14000.0f) });
        addGenre ("Trap Mix Gloss Guard", "Trap", "mix bus", { "mix bus" }, { lowCut (0, 24.0f, slope12), dynamicBell (5, 7000.0f, -1.5f, 1.2f, -18.0f, 12.0f, 180.0f), highShelf (6, 12000.0f, 0.6f) });

        // Hip-Hop
        addGenre ("Hip-Hop Rap Vocal Forward", "Hip-Hop", "rap vocal", { "presence" }, { lowCut (0, 90.0f), bell (2, 280.0f, -1.2f), bell (4, 2200.0f, 1.8f) });
        addGenre ("Hip-Hop Rap Vocal Low Cleanup", "Hip-Hop", "rap vocal", { "cleanup" }, { lowCut (0, 95.0f), bell (2, 240.0f, -1.8f), bell (3, 420.0f, -1.2f) });
        addGenre ("Hip-Hop Rap Vocal Dynamic Bite", "Hip-Hop", "rap vocal", { "dynamic" }, { bell (4, 2400.0f, 1.0f), dynamicBell (5, 4000.0f, -3.0f, 1.3f), dynamicBell (6, 6500.0f, -4.0f, 1.6f) });
        addGenre ("Hip-Hop Beat Bus Glue", "Hip-Hop", "beat bus", { "mix bus" }, { lowCut (0, 30.0f, slope12), bell (2, 320.0f, -1.3f, 0.9f), highShelf (6, 10500.0f, 0.5f) });
        addGenre ("Hip-Hop Beat Bus Low-Mid Clean", "Hip-Hop", "beat bus", { "cleanup" }, { lowCut (0, 32.0f, slope12), bell (2, 320.0f, -1.7f), bell (3, 700.0f, -0.8f) });
        addGenre ("Hip-Hop Kick Knock Control", "Hip-Hop", "kick", { "punch" }, { lowCut (0, 28.0f), bell (1, 85.0f, 1.5f), bell (2, 300.0f, -1.5f), bell (4, 2800.0f, 0.9f) });
        addGenre ("Hip-Hop Snare Crack", "Hip-Hop", "snare", { "punch" }, { lowCut (0, 90.0f, slope12), bell (2, 210.0f, 1.2f), bell (4, 2500.0f, 1.2f), dynamicBell (5, 5000.0f, -2.0f, 1.3f) });
        addGenre ("Hip-Hop Snare Body Crack", "Hip-Hop", "snare", { "body" }, { lowCut (0, 90.0f, slope12), bell (2, 200.0f, 1.5f), bell (3, 700.0f, -1.2f), bell (5, 4200.0f, 0.8f) });
        addGenre ("Hip-Hop Bass Groove Definition", "Hip-Hop", "bass", { "low control" }, { lowCut (0, 30.0f, slope12), bell (1, 90.0f, 1.1f), bell (2, 260.0f, -1.3f), bell (4, 900.0f, 0.8f) });
        addGenre ("Hip-Hop Bass Note Leveler", "Hip-Hop", "bass", { "dynamic" }, { dynamicBell (1, 80.0f, -2.5f, 1.0f, -18.0f, 18.0f, 170.0f), bell (2, 250.0f, -1.0f) });
        addGenre ("Hip-Hop Sample Dust Cleanup", "Hip-Hop", "sample", { "cleanup" }, { lowCut (0, 70.0f, slope12), bell (2, 350.0f, -1.5f), highCut (6, 14000.0f) });
        addGenre ("Hip-Hop Sample Boxy Control", "Hip-Hop", "sample", { "boxiness" }, { lowCut (0, 65.0f, slope12), bell (3, 520.0f, -2.0f, 1.2f), bell (4, 900.0f, -0.8f) });
        addGenre ("Hip-Hop Mix Punch Preserve", "Hip-Hop", "mix bus", { "mix bus" }, { lowCut (0, 28.0f, slope12), bell (2, 300.0f, -0.7f, 0.8f), highShelf (6, 11000.0f, 0.5f) });
        addGenre ("Hip-Hop Mix Vocal Pocket", "Hip-Hop", "mix bus", { "mix bus" }, { bell (3, 2400.0f, -0.7f, 1.0f, midMode), bell (2, 350.0f, -0.7f, 0.8f), highShelf (6, 10000.0f, 0.4f, 0.7f, sideMode) });

        // Funk BR
        addGenre ("Funk BR Vocal Clarity", "Funk BR", "vocal", { "presence" }, { lowCut (0, 100.0f), bell (2, 300.0f, -1.3f), bell (4, 2600.0f, 1.5f), dynamicBell (5, 4200.0f, -2.5f, 1.3f) });
        addGenre ("Funk BR Vocal Bite Control", "Funk BR", "vocal", { "presence" }, { lowCut (0, 100.0f), bell (4, 2600.0f, 1.2f), dynamicBell (5, 4300.0f, -3.0f, 1.3f) });
        addGenre ("Funk BR Vocal De-Esser Tight", "Funk BR", "vocal", { "de-esser" }, { dynamicBell (5, 6800.0f, -5.0f, 1.7f), highShelf (6, 11000.0f, 0.5f) });
        addGenre ("Funk BR Beat Dry Cleanup", "Funk BR", "beat", { "cleanup" }, { lowCut (0, 30.0f, slope12), bell (2, 300.0f, -1.2f), bell (3, 700.0f, -1.0f) });
        addGenre ("Funk BR Beat Top Tame", "Funk BR", "beat", { "de-harsh" }, { dynamicBell (5, 6000.0f, -2.5f, 1.3f), highShelf (6, 13000.0f, -0.5f) });
        addGenre ("Funk BR Kick Punch", "Funk BR", "kick", { "punch" }, { lowCut (0, 28.0f), bell (1, 75.0f, 1.4f), bell (2, 240.0f, -1.0f), bell (4, 2500.0f, 0.9f) });
        addGenre ("Funk BR Kick Front Punch", "Funk BR", "kick", { "punch" }, { lowCut (0, 28.0f), bell (1, 75.0f, 1.2f), bell (4, 2500.0f, 1.0f), bell (2, 240.0f, -1.0f) });
        addGenre ("Funk BR Bass Mono Lock", "Funk BR", "bass / 808", { "low control" }, { lowCut (0, 140.0f, slope24, sideMode), bell (1, 90.0f, 1.0f, 0.9f, midMode), bell (2, 260.0f, -1.0f) });
        addGenre ("Funk BR 808 Street Control", "Funk BR", "bass / 808", { "dynamic" }, { lowCut (0, 24.0f), dynamicBell (1, 55.0f, -3.0f, 1.0f, -18.0f, 18.0f, 170.0f), bell (3, 210.0f, -1.5f) });
        addGenre ("Funk BR 808 Side Cleanup", "Funk BR", "bass / 808", { "side cleanup" }, { lowCut (0, 24.0f, slope12), lowCut (1, 150.0f, slope24, sideMode), bell (2, 240.0f, -1.0f) });
        addGenre ("Funk BR Percussion Brightness", "Funk BR", "percussion", { "presence" }, { lowCut (0, 150.0f), bell (4, 3500.0f, 1.1f), dynamicBell (5, 6500.0f, -2.0f, 1.4f) });
        addGenre ("Funk BR Percussion Room Clean", "Funk BR", "percussion", { "cleanup" }, { lowCut (0, 120.0f), bell (3, 550.0f, -1.8f, 1.2f), highShelf (6, 10000.0f, 0.4f) });
        addGenre ("Funk BR Lead Thin Space", "Funk BR", "lead / synth", { "cleanup" }, { lowCut (0, 180.0f), bell (4, 2500.0f, -1.4f), highShelf (6, 10000.0f, 0.5f, 0.7f, sideMode) });
        addGenre ("Funk BR Mix Low Guard", "Funk BR", "mix bus", { "mix bus" }, { lowCut (0, 24.0f, slope12), dynamicBell (1, 70.0f, -1.5f, 0.9f), bell (2, 280.0f, -0.7f) });

        // Rock
        addGenre ("Rock Vocal Presence", "Rock", "vocal", { "presence" }, { lowCut (0, 85.0f), bell (2, 320.0f, -1.0f), bell (4, 2500.0f, 1.4f), dynamicBell (5, 4200.0f, -2.4f, 1.3f) });
        addGenre ("Rock Vocal Edge Control", "Rock", "vocal", { "de-harsh" }, { lowCut (0, 85.0f), bell (4, 2400.0f, 1.0f), dynamicBell (5, 4000.0f, -3.0f, 1.3f) });
        addGenre ("Rock Vocal Chest Focus", "Rock", "vocal", { "warmth" }, { lowCut (0, 75.0f, slope12), lowShelf (1, 180.0f, 0.9f), bell (2, 350.0f, -1.0f), bell (4, 2000.0f, 0.8f) });
        addGenre ("Rock Electric Guitar Fizz Tame", "Rock", "electric guitar", { "de-harsh" }, { lowCut (0, 90.0f), bell (4, 3800.0f, -1.5f, 1.2f), highCut (6, 10000.0f) });
        addGenre ("Rock Electric Guitar Mid Bite", "Rock", "electric guitar", { "presence" }, { lowCut (0, 85.0f), bell (3, 1600.0f, 1.2f), bell (2, 300.0f, -1.0f) });
        addGenre ("Rock Electric Guitar Double Width", "Rock", "electric guitar", { "width" }, { lowCut (0, 90.0f), highShelf (6, 8000.0f, 0.7f, 0.7f, sideMode), bell (2, 250.0f, -0.8f, 1.0f, sideMode) });
        addGenre ("Rock Acoustic Guitar Strum Cleanup", "Rock", "acoustic guitar", { "cleanup" }, { lowCut (0, 100.0f), bell (2, 260.0f, -1.6f), dynamicBell (5, 4500.0f, -2.0f, 1.3f) });
        addGenre ("Rock Bass Low-Mid Control", "Rock", "bass", { "low control" }, { lowCut (0, 32.0f, slope12), bell (1, 95.0f, 0.9f), bell (2, 260.0f, -1.7f), bell (4, 900.0f, 0.6f) });
        addGenre ("Rock Kick Beater Punch", "Rock", "kick", { "punch" }, { lowCut (0, 28.0f), bell (1, 75.0f, 1.1f), bell (2, 260.0f, -1.4f), bell (4, 3600.0f, 1.0f) });
        addGenre ("Rock Snare Body", "Rock", "snare", { "body" }, { lowCut (0, 85.0f, slope12), bell (2, 210.0f, 1.5f), bell (3, 650.0f, -1.4f), bell (5, 4200.0f, 0.8f) });
        addGenre ("Rock Snare Ring Tame", "Rock", "snare", { "de-harsh" }, { lowCut (0, 90.0f, slope12), dynamicBell (4, 2200.0f, -2.5f, 1.8f), dynamicBell (5, 5000.0f, -2.0f, 1.4f) });
        addGenre ("Rock Drum Bus Punch", "Rock", "drum bus", { "punch" }, { lowCut (0, 28.0f, slope12), lowShelf (1, 80.0f, 0.9f), bell (2, 320.0f, -0.9f), highShelf (6, 10500.0f, 0.5f) });
        addGenre ("Rock Drum Bus Cymbal Tame", "Rock", "drum bus", { "de-harsh" }, { dynamicBell (5, 5500.0f, -1.8f, 1.2f), highShelf (6, 12500.0f, -0.4f) });
        addGenre ("Rock Mix Harsh Guard", "Rock", "mix bus", { "mix bus" }, { bell (2, 320.0f, -0.6f, 0.8f), dynamicBell (5, 3800.0f, -1.4f, 1.1f), highShelf (6, 11000.0f, 0.4f) });

        // Sertanejo
        addGenre ("Sertanejo Vocal Silk Air", "Sertanejo", "vocal", { "air" }, { lowCut (0, 80.0f), bell (2, 300.0f, -1.0f), highShelf (6, 12000.0f, 1.0f), dynamicBell (5, 6500.0f, -3.5f, 1.6f) });
        addGenre ("Sertanejo Vocal Warmth", "Sertanejo", "vocal", { "warmth" }, { lowCut (0, 70.0f, slope12), lowShelf (1, 170.0f, 1.0f), bell (2, 350.0f, -1.0f) });
        addGenre ("Sertanejo Vocal Sibilance Safe", "Sertanejo", "vocal", { "de-esser" }, { dynamicBell (5, 6500.0f, -4.5f, 1.7f), highShelf (6, 11500.0f, 0.5f) });
        addGenre ("Sertanejo Backing Vocal Tuck", "Sertanejo", "backing vocal", { "cleanup" }, { lowCut (0, 130.0f), bell (2, 450.0f, -1.5f), bell (4, 2600.0f, -0.8f) });
        addGenre ("Sertanejo Backing Vocal Air Bed", "Sertanejo", "backing vocal", { "air" }, { lowCut (0, 140.0f), highShelf (6, 10000.0f, 0.8f, 0.7f, sideMode), dynamicBell (5, 6500.0f, -2.0f, 1.5f) });
        addGenre ("Sertanejo Acoustic Guitar Clarity", "Sertanejo", "acoustic guitar", { "clarity" }, { lowCut (0, 95.0f), bell (2, 260.0f, -1.5f), highShelf (6, 9500.0f, 0.8f) });
        addGenre ("Sertanejo Acoustic Guitar Boom Control", "Sertanejo", "acoustic guitar", { "cleanup" }, { lowCut (0, 90.0f), bell (1, 160.0f, -1.6f), bell (2, 320.0f, -1.4f) });
        addGenre ("Sertanejo Viola Presence", "Sertanejo", "viola", { "presence" }, { lowCut (0, 100.0f), bell (3, 800.0f, -0.8f), bell (5, 3000.0f, 1.1f) });
        addGenre ("Sertanejo Viola Nasal Tame", "Sertanejo", "viola", { "nasal" }, { lowCut (0, 100.0f), bell (3, 950.0f, -1.8f, 1.3f), dynamicBell (5, 3600.0f, -1.8f, 1.2f) });
        addGenre ("Sertanejo Bass Round Definition", "Sertanejo", "bass", { "low control" }, { lowCut (0, 30.0f, slope12), bell (1, 95.0f, 0.9f), bell (2, 240.0f, -1.2f), bell (4, 900.0f, 0.6f) });
        addGenre ("Sertanejo Percussion Soft Top", "Sertanejo", "percussion", { "de-harsh" }, { lowCut (0, 160.0f), dynamicBell (5, 6500.0f, -2.3f, 1.4f), highShelf (6, 12000.0f, -0.2f) });
        addGenre ("Sertanejo Mix Vocal Lift", "Sertanejo", "mix bus", { "mix bus" }, { bell (2, 320.0f, -0.6f, 0.8f), bell (4, 2500.0f, 0.5f, 1.0f, midMode), highShelf (6, 11500.0f, 0.5f) });
        addGenre ("Sertanejo Mix Gentle Polish", "Sertanejo", "mix bus", { "mix bus" }, { lowCut (0, 28.0f, slope12), highShelf (6, 12000.0f, 0.6f), dynamicBell (5, 6500.0f, -1.0f, 1.2f) });
        addGenre ("Sertanejo Mix Mud Control", "Sertanejo", "mix bus", { "cleanup" }, { bell (2, 300.0f, -0.9f, 0.8f), dynamicBell (3, 450.0f, -1.0f, 1.0f) });

        // Pop
        addGenre ("Pop Vocal Gloss Control", "Pop", "vocal", { "air" }, { lowCut (0, 90.0f), bell (4, 3200.0f, 1.1f), highShelf (6, 12000.0f, 1.1f), dynamicBell (5, 7000.0f, -3.5f, 1.6f) });
        addGenre ("Pop Vocal Air", "Pop", "vocal", { "air" }, { lowCut (0, 90.0f), bell (4, 3200.0f, 1.0f), highShelf (6, 12500.0f, 1.3f) });
        addGenre ("Pop Vocal Sibilance Dynamic", "Pop", "vocal", { "de-esser" }, { dynamicBell (5, 6900.0f, -4.5f, 1.7f), highShelf (6, 12000.0f, 0.5f) });
        addGenre ("Pop Vocal Low-Mid Cleanup", "Pop", "vocal", { "cleanup" }, { lowCut (0, 90.0f), bell (2, 280.0f, -1.8f), bell (3, 520.0f, -1.0f) });
        addGenre ("Pop Backing Vocal Wide Air", "Pop", "backing vocal", { "width" }, { lowCut (0, 130.0f), highShelf (6, 10500.0f, 0.9f, 0.7f, sideMode), bell (4, 2600.0f, -0.7f, 1.0f, midMode) });
        addGenre ("Pop Backing Vocal Lead Pocket", "Pop", "backing vocal", { "cleanup" }, { lowCut (0, 125.0f), bell (2, 420.0f, -1.5f), bell (4, 2800.0f, -0.9f) });
        addGenre ("Pop Synth Presence Clean", "Pop", "synth", { "presence" }, { lowCut (0, 120.0f), bell (3, 2200.0f, 1.0f), dynamicBell (5, 5600.0f, -2.0f, 1.3f) });
        addGenre ("Pop Drums Punch Polish", "Pop", "drums", { "punch" }, { lowCut (0, 30.0f, slope12), lowShelf (1, 85.0f, 0.8f), bell (2, 320.0f, -0.8f), highShelf (6, 11000.0f, 0.6f) });
        addGenre ("Pop Drum Bus Top Control", "Pop", "drums", { "de-harsh" }, { dynamicBell (5, 6000.0f, -1.8f, 1.2f), highShelf (6, 12000.0f, 0.4f) });
        addGenre ("Pop Bass Tight", "Pop", "bass", { "low control" }, { lowCut (0, 28.0f, slope12), bell (1, 90.0f, 1.0f), bell (2, 250.0f, -1.3f) });
        addGenre ("Pop Bass Note Control", "Pop", "bass", { "dynamic" }, { dynamicBell (1, 80.0f, -2.4f, 1.0f, -18.0f, 18.0f, 160.0f), bell (2, 250.0f, -0.9f) });
        addGenre ("Pop Guitar Bright Pocket", "Pop", "guitar", { "clarity" }, { lowCut (0, 100.0f), bell (2, 260.0f, -1.3f), bell (5, 4200.0f, 1.0f), dynamicBell (6, 6000.0f, -1.6f, 1.2f) });
        addGenre ("Pop Mix Gentle Width", "Pop", "mix bus", { "width" }, { lowCut (1, 130.0f, slope24, sideMode), bell (2, 300.0f, -0.6f, 0.8f), highShelf (6, 11000.0f, 0.5f, 0.7f, sideMode) });
        addGenre ("Pop Mix Safe Lift", "Pop", "mix bus", { "mix bus" }, { lowCut (0, 25.0f, slope12), bell (2, 320.0f, -0.5f, 0.8f), highShelf (6, 12000.0f, 0.6f) });

        // Gospel
        addGenre ("Gospel Lead Vocal Lift Safe", "Gospel", "lead vocal", { "presence" }, { lowCut (0, 80.0f), bell (2, 300.0f, -1.0f), bell (4, 2600.0f, 1.4f) });
        addGenre ("Gospel Lead Vocal De-Harsh", "Gospel", "lead vocal", { "de-harsh" }, { lowCut (0, 80.0f), dynamicBell (4, 3200.0f, -2.8f, 1.3f), dynamicBell (5, 6500.0f, -3.0f, 1.6f) });
        addGenre ("Gospel Lead Vocal Body", "Gospel", "lead vocal", { "body" }, { lowCut (0, 75.0f, slope12), lowShelf (1, 170.0f, 0.8f), bell (2, 340.0f, -0.8f), bell (4, 2500.0f, 0.8f) });
        addGenre ("Gospel Choir Air", "Gospel", "choir", { "air" }, { lowCut (0, 140.0f), bell (4, 2500.0f, -0.6f), highShelf (6, 10000.0f, 0.8f, 0.7f, sideMode) });
        addGenre ("Gospel Choir Sibilance Cloud", "Gospel", "choir", { "de-esser" }, { lowCut (0, 140.0f), dynamicBell (5, 6500.0f, -3.5f, 1.6f), highShelf (6, 11000.0f, 0.4f, 0.7f, sideMode) });
        addGenre ("Gospel Choir Low-Mid Cleanup", "Gospel", "choir", { "cleanup" }, { lowCut (0, 140.0f), bell (2, 320.0f, -1.6f), bell (3, 520.0f, -0.9f) });
        addGenre ("Gospel Piano Clarity", "Gospel", "piano / keys", { "clarity" }, { lowCut (0, 60.0f, slope12), bell (2, 320.0f, -1.3f), bell (4, 2800.0f, 1.0f) });
        addGenre ("Gospel Keys Warm Space", "Gospel", "piano / keys", { "warmth" }, { lowCut (0, 70.0f, slope12), lowShelf (1, 160.0f, 0.7f), bell (2, 380.0f, -0.9f), highShelf (6, 10000.0f, 0.5f, 0.7f, sideMode) });
        addGenre ("Gospel Guitar Clean Presence", "Gospel", "guitar", { "presence" }, { lowCut (0, 95.0f), bell (2, 280.0f, -1.2f), bell (4, 3500.0f, 1.0f), dynamicBell (5, 5200.0f, -1.8f, 1.3f) });
        addGenre ("Gospel Bass Foundation Control", "Gospel", "bass", { "low control" }, { lowCut (0, 30.0f, slope12), bell (1, 90.0f, 0.9f), bell (2, 250.0f, -1.2f) });
        addGenre ("Gospel Drum Bus Natural Punch", "Gospel", "drums", { "punch" }, { lowCut (0, 30.0f, slope12), lowShelf (1, 80.0f, 0.7f), bell (2, 320.0f, -0.8f), highShelf (6, 11000.0f, 0.4f) });
        addGenre ("Gospel Mix Clean Lift", "Gospel", "mix bus", { "mix bus" }, { bell (2, 320.0f, -0.7f, 0.8f), highShelf (6, 11000.0f, 0.6f), dynamicBell (5, 6500.0f, -1.2f, 1.2f) });
        addGenre ("Gospel Mix Side Air Safe", "Gospel", "mix bus", { "width" }, { lowCut (1, 130.0f, slope24, sideMode), highShelf (6, 11000.0f, 0.5f, 0.7f, sideMode) });
        addGenre ("Gospel Mix Mud Tame", "Gospel", "mix bus", { "cleanup" }, { bell (2, 300.0f, -0.9f, 0.8f), dynamicBell (3, 450.0f, -1.0f, 1.0f) });

        // Electronic
        addGenre ("Electronic Kick Tight Punch", "Electronic", "kick", { "punch" }, { lowCut (0, 25.0f), bell (1, 60.0f, 1.3f), bell (2, 220.0f, -1.2f), bell (4, 3500.0f, 0.8f) });
        addGenre ("Electronic Kick Click Control", "Electronic", "kick", { "de-harsh" }, { lowCut (0, 25.0f), dynamicBell (4, 4200.0f, -2.0f, 1.3f), bell (1, 70.0f, 0.8f) });
        addGenre ("Electronic Bass Tight Low", "Electronic", "bass", { "low control" }, { lowCut (0, 25.0f, slope12), bell (2, 220.0f, -1.5f), lowCut (3, 140.0f, slope24, sideMode) });
        addGenre ("Electronic Bass Sub Mono Lock", "Electronic", "bass", { "mono" }, { lowCut (0, 24.0f, slope12), lowCut (1, 150.0f, slope24, sideMode), bell (2, 65.0f, 0.8f, 0.9f, midMode) });
        addGenre ("Electronic Bass Note Tamer", "Electronic", "bass", { "dynamic" }, { dynamicBell (1, 60.0f, -2.8f, 1.0f, -18.0f, 18.0f, 160.0f), bell (2, 220.0f, -1.0f) });
        addGenre ("Electronic Synth Presence", "Electronic", "synth", { "presence" }, { lowCut (0, 120.0f), bell (4, 2200.0f, 1.0f), dynamicBell (5, 5600.0f, -2.0f, 1.3f) });
        addGenre ("Electronic Synth Harshness Control", "Electronic", "synth", { "de-harsh" }, { lowCut (0, 120.0f), dynamicBell (4, 3500.0f, -2.8f, 1.3f), highCut (6, 15000.0f) });
        addGenre ("Electronic Lead Cut Through", "Electronic", "lead", { "presence" }, { lowCut (0, 140.0f), bell (4, 2600.0f, 1.3f), dynamicBell (5, 5600.0f, -2.0f, 1.3f) });
        addGenre ("Electronic Lead Resonance Tame", "Electronic", "lead", { "resonance" }, { lowCut (0, 140.0f), dynamicBell (4, 3000.0f, -2.5f, 1.8f), dynamicBell (5, 6200.0f, -2.0f, 1.6f) });
        addGenre ("Electronic Pad Side Air", "Electronic", "pad", { "width" }, { lowCut (0, 120.0f), lowCut (1, 160.0f, slope24, sideMode), highShelf (6, 10000.0f, 0.8f, 0.7f, sideMode) });
        addGenre ("Electronic Pad Low-Mid Clean", "Electronic", "pad", { "cleanup" }, { lowCut (0, 120.0f), bell (2, 300.0f, -1.6f), bell (3, 600.0f, -0.8f) });
        addGenre ("Electronic Percussion Top Detail", "Electronic", "percussion", { "detail" }, { lowCut (0, 180.0f), bell (5, 5000.0f, 0.8f), dynamicBell (6, 8000.0f, -1.8f, 1.4f) });
        addGenre ("Electronic Mix Side Low Clean", "Electronic", "mix bus", { "mix bus" }, { lowCut (1, 130.0f, slope24, sideMode), bell (2, 300.0f, -0.6f, 0.8f), highShelf (6, 11000.0f, 0.4f, 0.7f, sideMode) });
        addGenre ("Electronic Master Safe Lift", "Electronic", "master", { "master" }, { lowCut (0, 24.0f, slope12), bell (2, 300.0f, -0.5f, 0.8f), highShelf (6, 12000.0f, 0.5f), dynamicBell (5, 6500.0f, -0.8f, 1.2f, -18.0f, 15.0f, 220.0f) });

    }
}
