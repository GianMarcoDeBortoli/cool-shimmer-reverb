#include "PluginProcessor.h"
#include "PluginEditor.h"


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Range::EnabledOff, Param::Range::EnabledOn, true },
    { Param::ID::DampingFactor,    Param::Name::DampingFactor,    "",  Param::Range::DampFilterCoeffDefault,  Param::Range::DampFilterCoeffMin,   Param::Range::DampFilterCoeffMax,   Param::Range::DampFilterCoeffInc,   Param::Range::DampFilterCoeffSkw },
};

KeithBarrReverbProcessor::KeithBarrReverbProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    KBReverb(MaxChannels, static_cast<float>(1.0f)),
    enableRamp(0.05f)
    
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });

    parameterManager.registerParameterCallback(Param::ID::DampingFactor,
    [this](float newValue, bool /*force*/)
    {
        KBReverb.setDampingFilterCoeff(newValue);
    });
}

KeithBarrReverbProcessor::~KeithBarrReverbProcessor()
{
}

void KeithBarrReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };

    KBReverb.prepare(sampleRate, MaxChannels);
    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);

    parameterManager.updateParameters(true);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void KeithBarrReverbProcessor::releaseResources()
{
    KBReverb.clear();
    fxBuffer.clear();
}

void KeithBarrReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    // for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
    //     fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        fxBuffer.copyFrom(ch, 0, buffer.getReadPointer(ch), numSamples);
    }

    KBReverb.process(fxBuffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
    enableRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    // enableRamp.applyInverseGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, numSamples);
    }
}

void KeithBarrReverbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void KeithBarrReverbProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
const juce::String KeithBarrReverbProcessor::getName() const { return JucePlugin_Name; }
bool KeithBarrReverbProcessor::acceptsMidi() const { return false; }
bool KeithBarrReverbProcessor::producesMidi() const { return false; }
bool KeithBarrReverbProcessor::isMidiEffect() const { return false; }
double KeithBarrReverbProcessor::getTailLengthSeconds() const { return 0.0; }
int KeithBarrReverbProcessor::getNumPrograms() { return 1; }
int KeithBarrReverbProcessor::getCurrentProgram() { return 0; }
void KeithBarrReverbProcessor::setCurrentProgram(int) { }
const juce::String KeithBarrReverbProcessor::getProgramName(int) { return {}; }
void KeithBarrReverbProcessor::changeProgramName(int, const juce::String&) { }
bool KeithBarrReverbProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* KeithBarrReverbProcessor::createEditor() { return new KeithBarrReverbProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KeithBarrReverbProcessor();
}