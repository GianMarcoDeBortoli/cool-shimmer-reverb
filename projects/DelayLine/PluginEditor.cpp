#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayLineProcessorEditor::DelayLineProcessorEditor(DelayLineProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    addAndMakeVisible(genericParameterEditor);
    const int numOfParams { static_cast<int>(audioProcessor.getParameterManager().getParameters().size()) };
    setSize(300, numOfParams * genericParameterEditor.parameterWidgetHeight);
}

DelayLineProcessorEditor::~DelayLineProcessorEditor()
{
}

//==============================================================================
void DelayLineProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DelayLineProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}