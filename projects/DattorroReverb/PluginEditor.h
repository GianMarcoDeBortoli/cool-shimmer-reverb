#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DattorroReverbProcessorEditor : public juce::AudioProcessorEditor
{
public:
    DattorroReverbProcessorEditor(DattorroReverbProcessor&);
    ~DattorroReverbProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DattorroReverbProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DattorroReverbProcessorEditor)
};