#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AllPassFilterProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AllPassFilterProcessorEditor(AllPassFilterProcessor&);
    ~AllPassFilterProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AllPassFilterProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AllPassFilterProcessorEditor)
};