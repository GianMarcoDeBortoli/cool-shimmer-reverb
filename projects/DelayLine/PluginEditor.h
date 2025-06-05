#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DelayLineProcessorEditor : public juce::AudioProcessorEditor
{
public:
    DelayLineProcessorEditor(DelayLineProcessor&);
    ~DelayLineProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DelayLineProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayLineProcessorEditor)
};