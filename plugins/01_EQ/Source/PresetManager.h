#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

namespace AUREQ
{
    struct PresetParameter
    {
        juce::String id;
        float value;
    };

    struct Preset
    {
        juce::String name;
        juce::String category;
        juce::String subcategory;
        juce::StringArray tags;
        std::vector<PresetParameter> parameters;
    };

    struct UserPreset
    {
        juce::String name;
        juce::File file;
    };

    class PresetManager
    {
    public:
        PresetManager (juce::AudioProcessorValueTreeState& apvtsToUse);
        ~PresetManager() = default;

        int getNumPresets() const { return static_cast<int> (presets.size()); }
        juce::String getPresetName (int index) const;
        juce::String getPresetCategory (int index) const;
        juce::String getPresetSubcategory (int index) const;
        juce::StringArray getPresetTags (int index) const;
        int getCurrentPresetIndex() const { return currentPresetIndex; }
        int getCurrentUserPresetIndex() const { return currentUserPresetIndex; }
        juce::String getCurrentPresetName() const;
        
        void loadPreset (int index);
        void refreshUserPresets();
        int getNumUserPresets() const { return static_cast<int> (userPresets.size()); }
        juce::String getUserPresetName (int index) const;
        bool loadUserPreset (int index);
        bool saveCurrentUserPreset (const juce::String& name);
        juce::File getUserPresetFile (int index) const;
        juce::File getUserPresetDirectoryForUser() const;
        bool revealUserPresetDirectory() const;
        bool deleteUserPreset (int index);

    private:
        void initializePresets();
        static juce::File getUserPresetDirectory();
        static juce::String sanitizePresetName (const juce::String& name);
        static juce::File getAvailableUserPresetFile (const juce::String& sanitizedName);

        juce::AudioProcessorValueTreeState& apvts;
        std::vector<Preset> presets;
        std::vector<UserPreset> userPresets;
        int currentPresetIndex = 0;
        int currentUserPresetIndex = -1;
        juce::String currentPresetName;
    };
}
