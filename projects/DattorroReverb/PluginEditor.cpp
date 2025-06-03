#include "PluginProcessor.h"
#include "PluginEditor.h"

DattorroReverbProcessorEditor::DattorroReverbProcessorEditor(DattorroReverbProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    enableParameterEditor(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::Enabled, Param::ID::Mix }
    ),
    diffusionParameterEditor(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::PreDelay, Param::ID::ToneControl }
    ),
    decayParameterEditor(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::DampFilterCoeff, Param::ID::DecayCoeff }
    )
{
    addAndMakeVisible(enableParameterEditor);
    addAndMakeVisible(diffusionParameterEditor);
    addAndMakeVisible(decayParameterEditor);

    setSize(totWidth, totHeight);
}

DattorroReverbProcessorEditor::~DattorroReverbProcessorEditor()
{
}

//==============================================================================
void DattorroReverbProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DattorroReverbProcessorEditor::resized()
{
    auto localBounds { getLocalBounds() };
    enableParameterEditor.setBounds(localBounds.removeFromLeft(enableWidth));
    diffusionParameterEditor.setBounds(localBounds.removeFromLeft(diffusionWidth));
    decayParameterEditor.setBounds(localBounds);
}