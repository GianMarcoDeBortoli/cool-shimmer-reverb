#include "PluginProcessor.h"
#include "PluginEditor.h"


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Range::EnabledOff, Param::Range::EnabledOn, true },
    { Param::ID::Coeff,    Param::Name::Coeff,    "",  Param::Range::CoeffDefault,  Param::Range::CoeffMin,   Param::Range::CoeffMax,   Param::Range::CoeffInc,   Param::Range::CoeffSkw },

};

LeakyIntegratorProcessor::LeakyIntegratorProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    leakyIntegrator(Param::Range::CoeffDefault),
    enableRamp(0.05f)
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });
    parameterManager.registerParameterCallback(Param::ID::Coeff, 
    [this](float newValue, bool force)
    {
        newValue = std::clamp(newValue, 0.01f, 0.99f);
        leakyIntegrator.setCoeff(newValue);
    });
}

LeakyIntegratorProcessor::~LeakyIntegratorProcessor()
{
}

void LeakyIntegratorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };

    leakyIntegrator.prepare(sampleRate);
    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();

    parameterManager.updateParameters(true);
}

void LeakyIntegratorProcessor::releaseResources()
{
    leakyIntegrator.clear();
    fxBuffer.clear();
}

void LeakyIntegratorProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        fxBuffer.copyFrom(ch, 0, buffer.getReadPointer(ch), numSamples);
    }
    leakyIntegrator.process(fxBuffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
    enableRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);

    enableRamp.applyInverseGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
}

void LeakyIntegratorProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void LeakyIntegratorProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
const juce::String LeakyIntegratorProcessor::getName() const { return JucePlugin_Name; }
bool LeakyIntegratorProcessor::acceptsMidi() const { return false; }
bool LeakyIntegratorProcessor::producesMidi() const { return false; }
bool LeakyIntegratorProcessor::isMidiEffect() const { return false; }
double LeakyIntegratorProcessor::getTailLengthSeconds() const { return 0.0; }
int LeakyIntegratorProcessor::getNumPrograms() { return 1; }
int LeakyIntegratorProcessor::getCurrentProgram() { return 0; }
void LeakyIntegratorProcessor::setCurrentProgram(int) { }
const juce::String LeakyIntegratorProcessor::getProgramName(int) { return {}; }
void LeakyIntegratorProcessor::changeProgramName(int, const juce::String&) { }
bool LeakyIntegratorProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* LeakyIntegratorProcessor::createEditor() { return new LeakyIntegratorProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LeakyIntegratorProcessor();
}