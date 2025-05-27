#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LeakyIntegratorProcessorEditor : public juce::AudioProcessorEditor
{
public:
    LeakyIntegratorProcessorEditor(LeakyIntegratorProcessor&);
    ~LeakyIntegratorProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LeakyIntegratorProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeakyIntegratorProcessorEditor)
};