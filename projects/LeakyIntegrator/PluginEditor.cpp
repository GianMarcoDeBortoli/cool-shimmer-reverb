#include "PluginProcessor.h"
#include "PluginEditor.h"

LeakyIntegratorProcessorEditor::LeakyIntegratorProcessorEditor(LeakyIntegratorProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    addAndMakeVisible(genericParameterEditor);
    const int numOfParams { static_cast<int>(audioProcessor.getParameterManager().getParameters().size()) };
    setSize(300, numOfParams * genericParameterEditor.parameterWidgetHeight);
}

LeakyIntegratorProcessorEditor::~LeakyIntegratorProcessorEditor()
{
}

//==============================================================================
void LeakyIntegratorProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void LeakyIntegratorProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}