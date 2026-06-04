#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Theme.h"
#include "Localization.h"
#include <array>
#include <functional>
#include <vector>
#include "../Source/ParameterIDs.h"

namespace AUREQ
{
    struct VisualEQBand
    {
        bool enabled = false;
        bool bypassed = false;
        int type = 0;
        float frequencyHz = 1000.0f;
        float gainDb = 0.0f;
        float q = 1.0f;
        int slope = 12;
        bool dynamicEnabled = false;
        float dynamicRange = 0.0f;
        float dynamicThreshold = -24.0f;
        float dynamicGainDb = 0.0f;
        int channelMode = 0;
    };

    class EQGraphView : public juce::Component
    {
    public:
        EQGraphView();
        ~EQGraphView() override;

        void setThemeColors(const ThemeColors& colors);
        void setLanguage (AureqLanguage language);
        
        void setBands(const std::array<VisualEQBand, AUREQ::Params::numBands>& newBands);

        /**
         * Receives the spectrum magnitude bins (in dBFS) from the AnalyzerProcessor.
         * Called from the UI timer — runs on the message thread only.
         * numBins should be AnalyzerProcessor::NUM_BINS (1025).
         */
        void setSpectrumData (const float* bins, int numBins);
        
        int getSelectedBandIndex() const { return selectedBandIndex; }
        void setSelectedBandIndex(int index)
        {
            if (selectedBandIndex != index)
            {
                selectedBandIndex = index;
                repaint();
                if (onBandSelectionChanged)
                    onBandSelectionChanged (selectedBandIndex);
            }
        }

        std::function<void(int newSelectedBandIndex)> onBandSelectionChanged;
        
        void paint(juce::Graphics& g) override;
        void resized() override;

        // Mouse handlers for dragging handles
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;
        void mouseDoubleClick(const juce::MouseEvent& event) override;
        void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

        // Coordinate conversion helpers
        float frequencyToX(float frequencyHz) const;
        float xToFrequency(float x) const;
        float gainToY(float gainDb) const;
        float yToGain(float y) const;

        // Callbacks for editor interaction
        std::function<void(int bandIndex)> onDragStart;
        std::function<void(int bandIndex, float frequencyHz, float gainDb)> onBandDragged;
        std::function<void(int bandIndex)> onDragEnd;
        std::function<void(int bandIndex, float q)> onBandQChanged;
        std::function<void(float frequencyHz, float gainDb, int filterTypeIndex, int slopeIndex)> onBandCreateRequested;
        std::function<void(int bandIndex, juce::Point<int> screenPosition)> onBandContextMenuRequested;
        std::function<void(int bandIndex, bool isSolo)> onBandSoloStatusChanged;

    private:
        ThemeColors themeColors;
        AureqLanguage currentLanguage { AureqLanguage::PortugueseBR };
        std::array<VisualEQBand, AUREQ::Params::numBands> bands;
        
        int activeBandDragIndex = -1;
        int selectedBandIndex = -1;

        // Convert a frequency in Hz to a normalized x coordinate (0.0 to 1.0)
        float getNormalizedX(float frequencyHz) const;

        // Convert a gain in dB to a normalized y coordinate (0.0 to 1.0)
        float getNormalizedY(float gainDb) const;

        // Spectrum analyzer bins (magnitude in dBFS, pre-smoothed by AnalyzerProcessor)
        std::vector<float> spectrumBins;
        int spectrumNumBins = 0;

        // Convert a normalized x coordinate (pixel) back to frequency in Hz
        float getFrequencyForX(float x) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQGraphView)
    };
}
