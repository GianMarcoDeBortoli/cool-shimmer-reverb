#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>

static const std::vector<mrta::ParameterInfo> Parameters
{
    { Param::ID::Buildup,       Param::Name::Buildup,       Param::Units::Ms,  500.f,  Param::Ranges::BuildupMin,       Param::Ranges::BuildupMax,       Param::Ranges::BuildupInc,       Param::Ranges::BuildupSkw },
    { Param::ID::Shift1,   Param::Name::Shift1,   "",                2.f,   Param::Ranges::Shift1Min,   Param::Ranges::Shift1Max,   Param::Ranges::Shift1Inc,   Param::Ranges::Shift1Skw },
    { Param::ID::Shift2,   Param::Name::Shift2,   "",                0.5f,   Param::Ranges::Shift2Min,   Param::Ranges::Shift2Max,   Param::Ranges::Shift2Inc,   Param::Ranges::Shift2Skw }
};

ShimmerAudioProcessor::ShimmerAudioProcessor() :
    parameterManager(*this, ProjectInfo::projectName, Parameters),
    shimmer(Param::Ranges::BuildupMax, 20.f, 2)
{
    parameterManager.registerParameterCallback(Param::ID::Buildup,
    [this] (float value, bool /*force*/)
    {
        shimmer.setBuildup(value);
    });

    parameterManager.registerParameterCallback(Param::ID::Shift1,
    [this] (float value, bool /*force*/)
    {
        shimmer.setRatio1(value);
    });

    parameterManager.registerParameterCallback(Param::ID::Shift2,
    [this] (float value, bool /*force*/)
    {
        shimmer.setRatio2(value);
    });

}

ShimmerAudioProcessor::~ShimmerAudioProcessor()
{
}

void ShimmerAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };

    shimmer.prepare(newSampleRate, Param::Ranges::BuildupMax, numChannels, samplesPerBlock);

    parameterManager.updateParameters(true);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void ShimmerAudioProcessor::releaseResources()
{
    shimmer.clear();
}

void ShimmerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));

    shimmer.process(fxBuffer.getArrayOfWritePointers(), fxBuffer.getArrayOfReadPointers(), numChannels, numSamples);

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
}

void ShimmerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void ShimmerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
bool ShimmerAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ShimmerAudioProcessor::createEditor() { return new ShimmerAudioProcessorEditor(*this); }
const juce::String ShimmerAudioProcessor::getName() const { return JucePlugin_Name; }
bool ShimmerAudioProcessor::acceptsMidi() const { return false; }
bool ShimmerAudioProcessor::producesMidi() const { return false; }
bool ShimmerAudioProcessor::isMidiEffect() const { return false; }
double ShimmerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int ShimmerAudioProcessor::getNumPrograms() { return 1; }
int ShimmerAudioProcessor::getCurrentProgram() { return 0; }
void ShimmerAudioProcessor::setCurrentProgram(int) { } 
const juce::String ShimmerAudioProcessor::getProgramName (int) { return {}; }
void ShimmerAudioProcessor::changeProgramName (int, const juce::String&) { }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ShimmerAudioProcessor();
}