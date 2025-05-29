#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>


static std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,   Param::Range::EnabledOff,   Param::Range::EnabledOn, Param::Range::EnabledDefault },
    { Param::ID::PreDelay, Param::Name::PreDelay,  Param::Units::Ms, Param::Range::PreDelayDefault, Param::Range::PreDelayMin, Param::Range::PreDelayMax, Param::Range::PreDelayInc, Param::Range::PreDelaySkw },
    { Param::ID::ToneControl, Param::Name::ToneControl, "", Param::Range::ToneControlDefault, Param::Range::ToneControlMin, Param::Range::ToneControlMax, Param::Range::ToneControlInc, Param::Range::ToneControlSkw },
    { Param::ID::InputDiffCoeff_1, Param::Name::InputDiffCoeff_1, "", Param::Range::InputDiffCoeff1Default, Param::Range::InputDiffCoeffMin, Param::Range::InputDiffCoeffMax, Param::Range::InputDiffCoeffInc, Param::Range::InputDiffCoeffSkw },
    { Param::ID::InputDiffCoeff_2, Param::Name::InputDiffCoeff_2, "", Param::Range::InputDiffCoeff2Default, Param::Range::InputDiffCoeffMin, Param::Range::InputDiffCoeffMax, Param::Range::InputDiffCoeffInc, Param::Range::InputDiffCoeffSkw },
    {Param::ID::LFOType, Param::Name::LFOType, Param::Range::LFOTypeSin, Param::Range::LFOTypeTri, Param::Range::LFOTypeDefault},
    {Param::ID::LFOFreqHz, Param::Name::LFOFreqHz, Param::Units::Hz, Param::Range::LFOFreqDefault, Param::Range::LFOFreqMin, Param::Range::LFOFreqMax, Param::Range::LFOFreqInc, Param::Range::LFOFreqSkw},
    {Param::ID::LFODepthMs, Param::Name::LFODepthMs, Param::Units::Ms, Param::Range::LFODepthDefault, Param::Range::LFODepthMin, Param::Range::LFODepthMax, Param::Range::LFODepthInc, Param::Range::LFODepthSkw},
    {Param::ID::LFOOffsetMs, Param::Name::LFOOffsetMs, Param::Units::Ms, Param::Range::LFOOffsetDefault, Param::Range::LFOOffsetMin, Param::Range::LFOOffsetMax, Param::Range::LFOOffsetInc, Param::Range::LFOOffsetSkw},
    {Param::ID::DecayDiffCoeff_1, Param::Name::DecayDiffCoeff_1, "", Param::Range::DecayDiffCoeff1Default, Param::Range::DecayDiffCoeff1Min, Param::Range::DecayDiffCoeff1Max, Param::Range::DecayDiffCoeff1Inc, Param::Range::DecayDiffCoeff1Skw},
    {Param::ID::DampFilterCoeff, Param::Name::DampFilterCoeff, "", Param::Range::DampFilterCoeffDefault, Param::Range::DampFilterCoeffMin, Param::Range::DampFilterCoeffMax, Param::Range::DampFilterCoeffInc, Param::Range::DampFilterCoeffSkw},
    {Param::ID::DecayCoeff, Param::Name::DecayCoeff, "", Param::Range::DecayCoeffDefault, Param::Range::DecayCoeffMin, Param::Range::DecayCoeffMax, Param::Range::DecayCoeffInc, Param::Range::DecayCoeffSkw},
    {Param::ID::DecayDiffCoeff_2, Param::Name::DecayDiffCoeff_2, "", Param::Range::DecayDiffCoeff2Default, Param::Range::DecayDiffCoeff2Min, Param::Range::DecayDiffCoeff2Max, Param::Range::DecayDiffCoeff2Inc, Param::Range::DecayDiffCoeff2Skw},
};

DattorroReverbProcessor::DattorroReverbProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    dattorroReverb(
        sampleRate,
        MaxChannels,
        static_cast<unsigned int>(std::round(Param::Range::PreDelayMax * static_cast<float>(0.001 * sampleRate))),
        Param::Range::ToneControlDefault,
        Param::Range::InputDiffCoeff1Default,
        Param::Range::InputDiffCoeff2Default,
        static_cast<DSP::LFO::LFOType>(Param::Range::LFOTypeDefault),
        Param::Range::LFOFreqDefault,
        Param::Range::LFODepthDefault,
        Param::Range::LFODepthDefault,
        Param::Range::DecayDiffCoeff1Default,
        Param::Range::DampFilterCoeffDefault,
        Param::Range::DecayCoeffDefault,
        Param::Range::DecayDiffCoeff2Default
    ),
    enabled { Param::Range::EnabledDefault },
    enableRamp(0.05f),
    preDelayMs { Param::Range::PreDelayDefault },
    toneControl { Param::Range::ToneControlDefault },
    inputDiffusionCoeff_1 { Param::Range::InputDiffCoeff1Default },
    inputDiffusionCoeff_2 { Param::Range::InputDiffCoeff2Default },
    lfoType { Param::Range::LFOTypeDefault },
    lfoFreqHz { Param::Range::LFOFreqDefault },
    lfoDepthMs { Param::Range::LFODepthDefault },
    lfoOffsetMs { Param::Range::LFODepthDefault },
    decayDiffusionCoeff_1 { Param::Range::DecayDiffCoeff1Default },
    dampingFilterCoeff { Param::Range::DampFilterCoeffDefault },
    decayCoeff { Param::Range::DecayCoeffDefault },
    decayDiffusionCoeff_2 { Param::Range::DecayDiffCoeff2Default }
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
    parameterManager.registerParameterCallback(Param::ID::InputDiffCoeff_1,
    [this](float newInDiffCoeff, bool /*force*/)
    {
        inputDiffusionCoeff_1 = std::clamp(newInDiffCoeff, Param::Range::InputDiffCoeffMin, Param::Range::InputDiffCoeffMax);
        dattorroReverb.setInputDiffusionCoeff_1(inputDiffusionCoeff_1);
    });
    parameterManager.registerParameterCallback(Param::ID::InputDiffCoeff_2,
    [this](float newInDiffCoeff, bool /*force*/)
    {
        inputDiffusionCoeff_2 = std::clamp(newInDiffCoeff, Param::Range::InputDiffCoeffMin, Param::Range::InputDiffCoeffMax);
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
    parameterManager.registerParameterCallback(Param::ID::LFOOffsetMs,
    [this](float newLFOOffsetMs, bool /*force*/)
    {
        lfoOffsetMs = std::clamp(newLFOOffsetMs, Param::Range::LFOOffsetMin, Param::Range::LFOOffsetMax);
        dattorroReverb.setLFOoffsetMs(lfoOffsetMs);
    });
    parameterManager.registerParameterCallback(Param::ID::DecayDiffCoeff_1,
    [this](float newDecayDiffCoeff, bool /*force*/)
    {
        decayDiffusionCoeff_1 = std::clamp(newDecayDiffCoeff, Param::Range::DecayDiffCoeff1Min, Param::Range::DecayDiffCoeff1Max);
        dattorroReverb.setDecayDiffusionCoeff_1(decayDiffusionCoeff_1);
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
    parameterManager.registerParameterCallback(Param::ID::DecayDiffCoeff_2,
    [this](float newDecayDiffCoeff, bool /*force*/)
    {
        decayDiffusionCoeff_2 = std::clamp(newDecayDiffCoeff, Param::Range::DecayDiffCoeff2Min, Param::Range::DecayDiffCoeff2Max);
        dattorroReverb.setDecayDiffusionCoeff_2(decayDiffusionCoeff_2);
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
    enableRamp.applyInverseGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);
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