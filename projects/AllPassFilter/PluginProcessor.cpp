#include "PluginProcessor.h"
#include "PluginEditor.h"


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Range::EnabledOff, Param::Range::EnabledOn, true },
};

AllPassFilterProcessor::AllPassFilterProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    AllPassFilter(MaxDelaySizeMs, MaxChannels),
    enableRamp(0.05f)

{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });
}

AllPassFilterProcessor::~AllPassFilterProcessor()
{
}

void AllPassFilterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };

    AllPassFilter.prepare(sampleRate, MaxDelaySizeMs, MaxChannels);
    AllPassFilter.setDelayTime(MaxDelaySizeMs);
    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);

    parameterManager.updateParameters(true);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void AllPassFilterProcessor::releaseResources()
{
    AllPassFilter.clear();
}

void AllPassFilterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));


    AllPassFilter.process(fxBuffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
    enableRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
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