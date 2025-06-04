#include "PluginProcessor.h"
#include "PluginEditor.h"

ShimmerAudioProcessorEditor::ShimmerAudioProcessorEditor(ShimmerAudioProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    unsigned int numParams { static_cast<unsigned int>(audioProcessor.getParameterManager().getParameters().size()) };
    unsigned int paramHeight { static_cast<unsigned int>(genericParameterEditor.parameterWidgetHeight) };

    addAndMakeVisible(genericParameterEditor);
    setSize(300, numParams * paramHeight);
}

ShimmerAudioProcessorEditor::~ShimmerAudioProcessorEditor()
{
    genericParameterEditor.setLookAndFeel(nullptr);
}

void ShimmerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void ShimmerAudioProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}