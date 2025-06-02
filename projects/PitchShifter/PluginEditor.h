#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PitchShifterEditor : public juce::AudioProcessorEditor
{
public:
    PitchShifterEditor(PitchShifterProcessor&);
    ~PitchShifterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PitchShifterProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchShifterEditor)
};
