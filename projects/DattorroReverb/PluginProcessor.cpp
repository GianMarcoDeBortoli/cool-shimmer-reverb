#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,   Param::Range::EnabledOff,   Param::Range::EnabledOn, Param::Range::EnabledDefault },
    { Param::ID::PreDelay, Param::Name::PreDelay,  Param::Units::Ms, Param::Range::PreDelayDefault, Param::Range::PreDelayMin, Param::Range::PreDelayMax, Param::Range::PreDelayInc, Param::Range::PreDelaySkw },
    { Param::ID::ToneControl, Param::Name::ToneControl, "", Param::Range::ToneControlDefault, Param::Range::ToneControlMin, Param::Range::ToneControlMax, Param::Range::ToneControlInc, Param::Range::ToneControlSkw },
    { Param::ID::InDiffCoeff_1, Param::Name::InDiffCoeff_1, "", Param::Range::InDiffCoeffDefault, Param::Range::InDiffCoeffMin, Param::Range::InDiffCoeffMax, Param::Range::InDiffCoeffInc, Param::Range::InDiffCoeffSkw },
    { Param::ID::InDiffCoeff_2, Param::Name::InDiffCoeff_2, "", Param::Range::InDiffCoeffDefault, Param::Range::InDiffCoeffMin, Param::Range::InDiffCoeffMax, Param::Range::InDiffCoeffInc, Param::Range::InDiffCoeffSkw },
    {Param::ID::LFOType, Param::Name::LFOType, Param::Range::LFOTypeSin, Param::Range::LFOTypeTri, Param::Range::LFOTypeDefault},
    {Param::ID::LFOFreqHz, Param::Name::LFOFreqHz, Param::Units::Hz, Param::Range::LFOFreqDefault, Param::Range::LFOFreqMin, Param::Range::LFOFreqMax, Param::Range::LFOFreqInc, Param::Range::LFOFreqSkw},
    {Param::ID::LFODepthMs, Param::Name::LFODepthMs, Param::Units::Ms, Param::Range::LFODepthDefault, Param::Range::LFODepthMin, Param::Range::LFODepthMax, Param::Range::LFODepthInc, Param::Range::LFODepthSkw}
};

DattorroReverbProcessor::DattorroReverbProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    dattorroReverb(
        MaxChannels,
        static_cast<unsigned int>(std::round(Param::Range::PreDelayMax * static_cast<float>(0.001 * sampleRate))),
        Param::Range::ToneControlDefault,
        inputDiffDelayMs_1,
        inputDiffDelayMs_2,
        Param::Range::InDiffCoeffDefault,
        inputDiffDelayMs_3,
        inputDiffDelayMs_4,
        Param::Range::InDiffCoeffDefault,
        static_cast<DSP::LFO::LFOType>(Param::Range::LFOTypeDefault),
        Param::Range::LFOFreqDefault,
        Param::Range::LFODepthDefault
    ),
    enabled { Param::Range::EnabledDefault },
    enableRamp(0.05f),
    preDelayMs { Param::Range::PreDelayDefault },
    toneControl { Param::Range::ToneControlDefault },
    inputDiffusionCoeff_1 { Param::Range::InDiffCoeffDefault },
    inputDiffusionCoeff_2 { Param::Range::InDiffCoeffDefault },
    lfoType { Param::Range::LFOTypeDefault },
    lfoFreqHz { Param::Range::LFOFreqDefault },
    lfoDepthMs { Param::Range::LFODepthDefault }
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
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
    parameterManager.registerParameterCallback(Param::ID::InDiffCoeff_1,
    [this](float newInDiffCoeff, bool /*force*/)
    {
        inputDiffusionCoeff_1 = std::clamp(newInDiffCoeff, Param::Range::InDiffCoeffMin, Param::Range::InDiffCoeffMax);
        dattorroReverb.setInputDiffusionCoeff_1(inputDiffusionCoeff_1);
    });
    parameterManager.registerParameterCallback(Param::ID::InDiffCoeff_2,
    [this](float newInDiffCoeff, bool /*force*/)
    {
        inputDiffusionCoeff_2 = std::clamp(newInDiffCoeff, Param::Range::InDiffCoeffMin, Param::Range::InDiffCoeffMax);
        dattorroReverb.setInputDiffusionCoeff_2(inputDiffusionCoeff_2);
    });
    parameterManager.registerParameterCallback(Param::ID::LFOType,
    [this](float newLFOType, bool /*force*/)
    {
        lfoType = static_cast<DSP::LFO::LFOType>(std::round(newLFOType));
        lfoType = (std::min(std::max(lfoType, DSP::LFO::Sin), DSP::LFO::Tri));
        dattorroReverb.setLFOtype(lfoType);
    });
    parameterManager.registerParameterCallback(Param::ID::LFOFreqHz,
    [this](float newLFOFreqHz, bool /*force*/)
    {
        lfoFreqHz = std::clamp(newLFOFreqHz, Param::Range::LFOFreqMin, Param::Range::LFOFreqMax);
        dattorroReverb.setLFOfreqHz(lfoFreqHz);
    });
    parameterManager.registerParameterCallback(Param::ID::LFODepthMs,
    [this](float newLFODepthMs, bool /*force*/)
    {
        lfoDepthMs = std::clamp(newLFODepthMs, Param::Range::LFODepthMin, Param::Range::LFODepthMax);
        dattorroReverb.setLFOdepthMs(lfoDepthMs);
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