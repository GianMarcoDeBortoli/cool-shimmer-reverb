#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class KeithBarrReverbProcessorEditor : public juce::AudioProcessorEditor
{
public:
    KeithBarrReverbProcessorEditor(KeithBarrReverbProcessor&);
    ~KeithBarrReverbProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KeithBarrReverbProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeithBarrReverbProcessorEditor)
};