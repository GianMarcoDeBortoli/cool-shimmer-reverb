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

    static const int totHeight { 200 };
    static const int totWidth  { 900 };

    static const int paramHeight { 100 };
    static const int enableWidth { 300 };
    static const int diffusionWidth { 300 };

private:
    DattorroReverbProcessor& audioProcessor;
    mrta::GenericParameterEditor enableParameterEditor;
    mrta::GenericParameterEditor diffusionParameterEditor;
    mrta::GenericParameterEditor decayParameterEditor;

    GUI::MrtaLAF laf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DattorroReverbProcessorEditor)
};