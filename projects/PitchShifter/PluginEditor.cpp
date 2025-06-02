#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchShifterEditor::PitchShifterEditor(PitchShifterProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      genericParameterEditor(audioProcessor.getParameterManager())
{
    int height = static_cast<int>(audioProcessor.getParameterManager().getParameters().size())
               * genericParameterEditor.parameterWidgetHeight;
    setSize(300, height);
    addAndMakeVisible(genericParameterEditor);
}

PitchShifterEditor::~PitchShifterEditor()
{
}

void PitchShifterEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PitchShifterEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}
