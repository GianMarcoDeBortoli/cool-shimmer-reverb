#include "PluginProcessor.h"
#include "PluginEditor.h"

KeithBarrReverbProcessorEditor::KeithBarrReverbProcessorEditor(KeithBarrReverbProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    addAndMakeVisible(genericParameterEditor);
    const int numOfParams { static_cast<int>(audioProcessor.getParameterManager().getParameters().size()) };
    setSize(300, numOfParams * genericParameterEditor.parameterWidgetHeight);
}

KeithBarrReverbProcessorEditor::~KeithBarrReverbProcessorEditor()
{
}

//==============================================================================
void KeithBarrReverbProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void KeithBarrReverbProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}