#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>


static std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,   Param::Range::EnabledOff,   Param::Range::EnabledOn, Param::Range::EnabledDefault },
    { Param::ID::Mix,      Param::Name::Mix,       "", Param::Range::MixDefault, Param::Range::MixMin, Param::Range::MixMax, Param::Range::MixInc, Param::Range::MixSkw },
    { Param::ID::PreDelay, Param::Name::PreDelay,  Param::Units::Ms, Param::Range::PreDelayDefault, Param::Range::PreDelayMin, Param::Range::PreDelayMax, Param::Range::PreDelayInc, Param::Range::PreDelaySkw },
    { Param::ID::ToneControl, Param::Name::ToneControl, "", Param::Range::ToneControlDefault, Param::Range::ToneControlMin, Param::Range::ToneControlMax, Param::Range::ToneControlInc, Param::Range::ToneControlSkw },
    {Param::ID::DampFilterCoeff, Param::Name::DampFilterCoeff, "", Param::Range::DampFilterCoeffDefault, Param::Range::DampFilterCoeffMin, Param::Range::DampFilterCoeffMax, Param::Range::DampFilterCoeffInc, Param::Range::DampFilterCoeffSkw},
    {Param::ID::DecayCoeff, Param::Name::DecayCoeff, "", Param::Range::DecayCoeffDefault, Param::Range::DecayCoeffMin, Param::Range::DecayCoeffMax, Param::Range::DecayCoeffInc, Param::Range::DecayCoeffSkw},
};

DattorroReverbProcessor::DattorroReverbProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    dattorroReverb(
        sampleRate,
        MaxChannels,
        static_cast<unsigned int>(std::round(Param::Range::PreDelayMax * static_cast<float>(0.001 * sampleRate))),
        Param::Range::ToneControlDefault,
        Param::Range::DampFilterCoeffDefault,
        Param::Range::DecayCoeffDefault
    ),
    enabled { Param::Range::EnabledDefault },
    enableRamp(0.05f),
    mix { Param::Range::MixDefault },
    mixRamp(0.001f),
    preDelayMs { Param::Range::PreDelayDefault },
    toneControl { Param::Range::ToneControlDefault },
    dampingFilterCoeff { Param::Range::DampFilterCoeffDefault },
    decayCoeff { Param::Range::DecayCoeffDefault }
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });
    parameterManager.registerParameterCallback(Param::ID::Mix,
    [this](float newMix, bool /*force*/)
    {
        mix = std::clamp(newMix, Param::Range::MixMin, Param::Range::MixMax);
        mixRamp.setTarget(mix, true);
    });
    parameterManager.registerParameterCallback(Param::ID::PreDelay, 
    [this](float newPreDelayMs, bool /*force*/)
    {
        preDelayMs = std::clamp(newPreDelayMs, Param::Range::PreDelayMin, Param::Range::PreDelayMax);
        unsigned int preDelaySamples { static_cast<unsigned int>(std::round(preDelayMs * static_cast<float>(0.001 * sampleRate))) };
        dattorroReverb.setPreDelay(preDelaySamples);
    });
    parameterManager.registerParameterCallback(Param::ID::ToneControl,
    [this](float newToneControl, bool /*force*/)
    {
        toneControl = std::clamp(newToneControl, Param::Range::ToneControlMin, Param::Range::ToneControlMax);
        dattorroReverb.setToneControl(toneControl);
    });
    parameterManager.registerParameterCallback(Param::ID::DampFilterCoeff,
    [this](float newDampFilterCoeff, bool /*force*/)
    {
        dampingFilterCoeff = std::clamp(newDampFilterCoeff, Param::Range::DampFilterCoeffMin, Param::Range::DampFilterCoeffMax);
        dattorroReverb.setDampingFilterCoeff(dampingFilterCoeff);
    });
    parameterManager.registerParameterCallback(Param::ID::DecayCoeff,
    [this](float newDecayCoeff, bool /*force*/)
    {
        decayCoeff = std::clamp(newDecayCoeff, Param::Range::DecayCoeffMin, Param::Range::DecayCoeffMax);
        dattorroReverb.setDecayCoeff(decayCoeff);
    });
}

DattorroReverbProcessor::~DattorroReverbProcessor()
{
}

void DattorroReverbProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };
    sampleRate = std::max(newSampleRate, 1.0);

    unsigned int preDelaySamples { static_cast<unsigned int>(std::round(Param::Range::PreDelayMax * static_cast<float>(0.001 * sampleRate))) };
    dattorroReverb.prepare(sampleRate, numChannels, preDelaySamples);
    revBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);

    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);
    mixRamp.prepare(sampleRate, true, mix);

    parameterManager.updateParameters(true);
}

void DattorroReverbProcessor::releaseResources()
{
    dattorroReverb.clear();
}

void DattorroReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        revBuffer.copyFrom(ch, 0, buffer.getReadPointer(ch), numSamples);
    }

    dattorroReverb.process(revBuffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
    enableRamp.applyGain(revBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    mixRamp.applyGain(revBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    // enableRamp.applyInverseGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        buffer.addFrom(ch, 0, revBuffer, ch, 0, numSamples);
    }

}

void DattorroReverbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void DattorroReverbProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
const juce::String DattorroReverbProcessor::getName() const { return JucePlugin_Name; }
bool DattorroReverbProcessor::acceptsMidi() const { return false; }
bool DattorroReverbProcessor::producesMidi() const { return false; }
bool DattorroReverbProcessor::isMidiEffect() const { return false; }
double DattorroReverbProcessor::getTailLengthSeconds() const { return 0.0; }
int DattorroReverbProcessor::getNumPrograms() { return 1; }
int DattorroReverbProcessor::getCurrentProgram() { return 0; }
void DattorroReverbProcessor::setCurrentProgram(int) { }
const juce::String DattorroReverbProcessor::getProgramName(int) { return {}; }
void DattorroReverbProcessor::changeProgramName(int, const juce::String&) { }
bool DattorroReverbProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* DattorroReverbProcessor::createEditor() { return new DattorroReverbProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DattorroReverbProcessor();
}