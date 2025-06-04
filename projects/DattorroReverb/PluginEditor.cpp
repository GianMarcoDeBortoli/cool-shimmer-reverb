#include "PluginProcessor.h"
#include "PluginEditor.h"

DattorroReverbProcessorEditor::DattorroReverbProcessorEditor(DattorroReverbProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    main(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::Enabled, Param::ID::Mix }
    ),
    pitchShifter(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::Shift1, Param::ID::Shift2, Param::ID::ShiftAmount }
    ),
    reverberator(
        audioProcessor.getParameterManager(),
        paramHeight,
        { Param::ID::BuildUp, Param::ID::Size, Param::ID::Brightness, Param::ID::Decay }
    )
{
    addAndMakeVisible(main);
    addAndMakeVisible(pitchShifter);
    addAndMakeVisible(reverberator);

    column1Label.setText("", juce::dontSendNotification);
    column1Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(column1Label);

    column2Label.setText("Pitch Shifter", juce::dontSendNotification);
    column2Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(column2Label);

    column3Label.setText("Reverberator", juce::dontSendNotification);
    column3Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(column3Label);

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
    column1Label.setBounds(textLeftOffset, textTopOffset, paramWidth, textHeight);   // Adjust size/position as needed
    column2Label.setBounds(textLeftOffset + paramWidth, textTopOffset, paramWidth, textHeight);
    column3Label.setBounds(textLeftOffset + 2 * paramWidth, textTopOffset, paramWidth, textHeight);

    auto localBounds { getLocalBounds().removeFromBottom(4 * paramHeight) };
    main.setBounds(localBounds.removeFromLeft(paramWidth));
    pitchShifter.setBounds(localBounds.removeFromLeft(paramWidth));
    reverberator.setBounds(localBounds);
}