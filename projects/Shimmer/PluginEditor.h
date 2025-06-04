#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ShimmerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ShimmerAudioProcessorEditor(ShimmerAudioProcessor&);
    ~ShimmerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ShimmerAudioProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShimmerAudioProcessorEditor)
};