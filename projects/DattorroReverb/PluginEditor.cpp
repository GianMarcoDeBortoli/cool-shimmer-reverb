#include "PluginProcessor.h"
#include "PluginEditor.h"

DattorroReverbProcessorEditor::DattorroReverbProcessorEditor(DattorroReverbProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    addAndMakeVisible(genericParameterEditor);
    const int numOfParams { static_cast<int>(audioProcessor.getParameterManager().getParameters().size()) };
    setSize(300, numOfParams * genericParameterEditor.parameterWidgetHeight);
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
    genericParameterEditor.setBounds(getLocalBounds());
}