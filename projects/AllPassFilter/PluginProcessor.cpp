#include "PluginProcessor.h"
#include "PluginEditor.h"


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Ranges::EnabledOff, Param::Ranges::EnabledOn, true },
};

AllPassFilterProcessor::AllPassFilterProcessor() :
    parameterManager(*this, ProjectInfo::projectName, parameters)
{
}

AllPassFilterProcessor::~AllPassFilterProcessor()
{
}

void AllPassFilterProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    AllPassFilter.prepare(sampleRate);
    parameterManager.updateParameters(true);
}

void AllPassFilterProcessor::releaseResources()
{
}

void AllPassFilterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    AllPassFilter.process(buffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
}

void AllPassFilterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void AllPassFilterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
const juce::String AllPassFilterProcessor::getName() const { return JucePlugin_Name; }
bool AllPassFilterProcessor::acceptsMidi() const { return false; }
bool AllPassFilterProcessor::producesMidi() const { return false; }
bool AllPassFilterProcessor::isMidiEffect() const { return false; }
double AllPassFilterProcessor::getTailLengthSeconds() const { return 0.0; }
int AllPassFilterProcessor::getNumPrograms() { return 1; }
int AllPassFilterProcessor::getCurrentProgram() { return 0; }
void AllPassFilterProcessor::setCurrentProgram(int) { }
const juce::String AllPassFilterProcessor::getProgramName(int) { return {}; }
void AllPassFilterProcessor::changeProgramName(int, const juce::String&) { }
bool AllPassFilterProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AllPassFilterProcessor::createEditor() { return new AllPassFilterProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AllPassFilterProcessor();
}