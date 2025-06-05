#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MrtaLAF.h"

class DattorroReverbProcessorEditor : public juce::AudioProcessorEditor
{
public:
    DattorroReverbProcessorEditor(DattorroReverbProcessor&);
    ~DattorroReverbProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    static const int textHeight { 20 };
    static const int textLeftOffset { 10 };
    static const int textTopOffset { 10 };

    static const int paramHeight { 80 };
    static const int paramWidth { 200 };

    static const int totHeight { 4 * paramHeight + textTopOffset + textHeight };
    static const int totWidth  { 3 * paramWidth };

private:
    DattorroReverbProcessor& audioProcessor;
    mrta::GenericParameterEditor main;
    mrta::GenericParameterEditor pitchShifter;
    mrta::GenericParameterEditor reverberator;

    juce::Label column1Label;
    juce::Label column2Label;
    juce::Label column3Label;

    GUI::MrtaLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DattorroReverbProcessorEditor)
};