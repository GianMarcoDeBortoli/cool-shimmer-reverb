#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>


static std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,     Param::Name::Enabled,     Param::Range::EnabledOff,   Param::Range::EnabledOn, Param::Range::EnabledDefault },
    { Param::ID::Mix,         Param::Name::Mix,         "", Param::Range::MixDefault, Param::Range::MixMin, Param::Range::MixMax, Param::Range::MixInc, Param::Range::MixSkw },
    // Pitch shifter parameters
    { Param::ID::Shift1,      Param::Name::Shift1,      Param::Units::Hz, Param::Range::ShiftDefault, Param::Range::ShiftMin, Param::Range::ShiftMax, Param::Range::ShiftInc, Param::Range::ShiftSkw },
    { Param::ID::Shift2,      Param::Name::Shift2,      Param::Units::Hz, Param::Range::ShiftDefault, Param::Range::ShiftMin, Param::Range::ShiftMax, Param::Range::ShiftInc, Param::Range::ShiftSkw },
    // Keith Barr's reverb parameters
    { Param::ID::ShiftAmount, Param::Name::ShiftAmount, "", Param::Range::ShiftDefault, Param::Range::ShiftMin, Param::Range::ShiftMax, Param::Range::ShiftInc, Param::Range::ShiftSkw },
    { Param::ID::BuildUp,     Param::Name::BuildUp,     Param::Units::Ms, Param::Range::BuildUpDefault, Param::Range::BuildUpMin, Param::Range::BuildUpMax, Param::Range::BuildUpInc, Param::Range::BuildUpSkw },
    { Param::ID::Size,        Param::Name::Size,        "", Param::Range::SizeDefault, Param::Range::SizeMin, Param::Range::SizeMax, Param::Range::SizeInc, Param::Range::SizeSkw },
    { Param::ID::Brightness,  Param::Name::Brightness,  "", Param::Range::BrightnessDefault, Param::Range::BrightnessMin, Param::Range::BrightnessMax, Param::Range::BrightnessInc, Param::Range::BrightnessSkw },
    // Jon Dattorro's reverb parameters
    { Param::ID::Decay,       Param::Name::Decay,       "", Param::Range::DecayDefault, Param::Range::DecayMin, Param::Range::DecayMax, Param::Range::DecayInc, Param::Range::DecaySkw },
};

DattorroReverbProcessor::DattorroReverbProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    enabled { Param::Range::EnabledDefault },
    enableRamp(0.05f),
    mix { Param::Range::MixDefault },
    mixRamp(0.05f),
    // Pitch shifter parameters
    shift1 { Param::Range::ShiftDefault },
    shift2 { Param::Range::ShiftDefault },
    // Keith Barr's reverb parameters
    amount { Param::Range::ShiftAmountDefault },
    amountRamp(0.01f),
    buildUpMs { Param::Range::BuildUpDefault },
    buildUpRamp(0.01f),
    size { Param::Range::SizeDefault },
    brightness { Param::Range::BrightnessDefault },
    // Jon Dattorro's reverb parameters
    decay { Param::Range::DecayDefault },
    dattorroReverb(
        sampleRate,
        MaxChannels,
        Param::Range::BrightnessDefault,
        Param::Range::DecayDefault
    )
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
        mixRamp.setTarget(mix);
    });
    // Pitch shifter parameters
    // Keith Barr's reverb parameters
    parameterManager.registerParameterCallback(Param::ID::BuildUp,
    [this](float newBuildUpMs, bool /*force*/)
    {
        buildUpMs = std::clamp(newBuildUpMs, Param::Range::BuildUpMin, Param::Range::BuildUpMax);
        buildUpRamp.setTarget(buildUpMs);
    });
    parameterManager.registerParameterCallback(Param::ID::Brightness,
    [this](float newBrightness, bool /*force*/)
    {
        brightness = std::clamp(newBrightness, Param::Range::BrightnessMin, Param::Range::BrightnessMax);
        dattorroReverb.setBrightness(brightness);
    });
    // Jon Dattorro's reverb parameters
    parameterManager.registerParameterCallback(Param::ID::Decay,
    [this](float newDecay, bool /*force*/)
    {
        decay = std::clamp(newDecay, Param::Range::DecayMin, Param::Range::DecayMax);
        dattorroReverb.setDecay(decay);
    });
}

DattorroReverbProcessor::~DattorroReverbProcessor()
{
}

void DattorroReverbProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };
    sampleRate = std::max(newSampleRate, 1.0);

    dattorroReverb.prepare(sampleRate, numChannels);
    dattorroBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);

    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);
    mixRamp.prepare(sampleRate, true, mix);

    parameterManager.updateParameters(true);
}

void DattorroReverbProcessor::releaseResources()
{
    dattorroReverb.clear();
    dattorroBuffer.clear();
}

void DattorroReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        dattorroBuffer.copyFrom(ch, 0, buffer.getReadPointer(ch), numSamples);
    }

    dattorroReverb.process(dattorroBuffer.getArrayOfWritePointers(), buffer.getArrayOfReadPointers(), numChannels, numSamples);
    enableRamp.applyGain(dattorroBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    mixRamp.applyGain(dattorroBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    // Mix dry and wet signals
    float dry = 1.f - mixRamp.getNext();
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        buffer.applyGain(ch, 0, numSamples, dry);
        buffer.addFrom(ch, 0, dattorroBuffer, ch, 0, numSamples);
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