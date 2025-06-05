#include "PluginProcessor.h"
#include "PluginEditor.h"

AllPassFilterProcessorEditor::AllPassFilterProcessorEditor(AllPassFilterProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    addAndMakeVisible(genericParameterEditor);
    const int numOfParams { static_cast<int>(audioProcessor.getParameterManager().getParameters().size()) };
    setSize(300, numOfParams * genericParameterEditor.parameterWidgetHeight);
}

AllPassFilterProcessorEditor::~AllPassFilterProcessorEditor()
{
}

//==============================================================================
void AllPassFilterProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AllPassFilterProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}