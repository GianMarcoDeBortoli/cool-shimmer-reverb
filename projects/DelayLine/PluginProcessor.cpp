#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>


static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,   Param::Range::EnabledOff,   Param::Range::EnabledOn, true },
    { Param::ID::MethodOn, Param::Name::MethodOn,  Param::Range::MethodOff,    Param::Range::MethodOn,  true },
    { Param::ID::Index,    Param::Name::Index, "", Param::Range::IndexDefault, Param::Range::IndexMin, Param::Range::IndexMax, Param::Range::IndexInc, Param::Range::IndexSkw },
    { Param::ID::OffsetMs, Param::Name::OffsetMs,  "", Param::Range::OffsetDefault, Param::Range::OffsetMin, Param::Range::OffsetMax, Param::Range::OffsetInc, Param::Range::OffsetSkw },
    { Param::ID::DepthMs,  Param::Name::DepthMs,   "", Param::Range::DepthDefault,  Param::Range::DepthMin,  Param::Range::DepthMax,  Param::Range::DepthInc,  Param::Range::DepthSkw },
    { Param::ID::ModeRateHz, Param::Name::ModeRateHz, "", Param::Range::ModRateDefault, Param::Range::ModRateMin, Param::Range::ModRateMax, Param::Range::ModRateInc, Param::Range::ModRateSkw },
    { Param::ID::ModType,  Param::Name::ModType,  Param::Range::ModTypeSin, Param::Range::ModTypeTri, 0 }
};

DelayLineProcessor::DelayLineProcessor():
    parameterManager(*this, ProjectInfo::projectName, parameters),
    delayLine(MaxDelay, MaxChannels),
    enableRamp(0.05f),
    methodRamp(0.05f),
    indexRamp(0.001f)
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });
    parameterManager.registerParameterCallback(Param::ID::Index, 
    [this](float newValue, bool /*force*/)
    {
        index = std::clamp(newValue, Param::Range::IndexMin, Param::Range::IndexMax);
        indexRamp.setTarget(index);

    });

    parameterManager.registerParameterCallback(Param::ID::MethodOn,
    [this](float newValue, bool force)
    {
        method = newValue > 0.5f;
        methodRamp.setTarget(method ? 1.f : 0.f, force);
    });

    parameterManager.registerParameterCallback(Param::ID::OffsetMs,
        [this](float newOffsetMs, bool /*force*/)
    {
        offsetMs = std::fmax(newOffsetMs - 1.f, 0.f);
        offsetRamp.setTarget(offsetMs * static_cast<float>(0.001 * sampleRate));
    });

    parameterManager.registerParameterCallback(Param::ID::DepthMs,
        [this](float newDepthMs, bool /*force*/)
    {
        modDepthMs = std::fmax(newDepthMs, 0.f);
        modDepthRamp.setTarget(modDepthMs * static_cast<float>(0.001 * sampleRate));
    });

    parameterManager.registerParameterCallback(Param::ID::ModeRateHz,
        [this](float newModRateHz, bool /*force*/)
    {
        modRate = std::fmax(newModRateHz, 0.f);
        phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
    });

    parameterManager.registerParameterCallback(Param::ID::ModType,
        [this](float newModType, bool /*force*/)
    {
        ModulationType modType = static_cast<ModulationType>(std::round(newModType));
        modType = (std::min(std::max(modType, Sin), Tri));
    });
}

DelayLineProcessor::~DelayLineProcessor()
{
}

void DelayLineProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    unsigned int numChannels { std::max(static_cast<unsigned int>(getMainBusNumInputChannels()), static_cast<unsigned int>(MaxChannels)) };
    sampleRate = newSampleRate;

    delayLine.prepare(MaxDelay, numChannels);
    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);
    methodRamp.prepare(sampleRate, true, method ? 1.f : 0.f);
    indexRamp.prepare(sampleRate, true, index);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();

    methodBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    methodBuffer.clear();

    parameterManager.updateParameters(true);
}

void DelayLineProcessor::releaseResources()
{
    delayLine.clear();
    fxBuffer.clear();
    methodBuffer.clear();
}

void DelayLineProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels{ static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples{ static_cast<unsigned int>(buffer.getNumSamples()) };

    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        fxBuffer.copyFrom(ch, 0, buffer.getReadPointer(ch), numSamples);
    }

    for (unsigned int n = 0; n < numSamples; ++n) {

        // Process LFO acording to mod type
        float lfo[numChannels];
        switch (modType)
        {
        case Tri:
            lfo[1] = std::fabs((phaseState - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            lfo[2] = std::fabs((phaseState - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            break;

        case Sin:
            lfo[1] = 0.5f + 0.5f * std::sin(phaseState);
            lfo[2] = 0.5f + 0.5f * std::sin(phaseState);
            break;
        }

        // Increment and wrap phase states
        phaseState = std::fmod(phaseState + phaseInc, static_cast<float>(2 * M_PI));

        // Apply mod depth and offset ramps
        modDepthRamp.applyGain(lfo, numChannels);
        offsetRamp.applySum(lfo, numChannels);

        float x[numChannels];
        float y[numChannels];
        float z;

        for (unsigned int ch = 0; ch < numChannels; ++ch) {
            x[ch] = fxBuffer.getSample(ch, n);
        }

        delayLine.process(y, x, numChannels);

        for (unsigned int ch = 0; ch < numChannels; ++ch) {
            fxBuffer.setSample(ch, n, y[ch]);
        }

        for (unsigned int ch = 0; ch < numChannels; ++ch) {
            z = delayLine.getSample(ch, index, lfo);
            methodBuffer.setSample(ch, n, z);
        }
    }
    
    enableRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    methodRamp.applyInverseGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    enableRamp.applyGain(methodBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    methodRamp.applyGain(methodBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    enableRamp.applyInverseGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
    {
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
        buffer.addFrom(ch, 0, methodBuffer, ch, 0, static_cast<int>(numSamples));
    }
}

void DelayLineProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void DelayLineProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
const juce::String DelayLineProcessor::getName() const { return JucePlugin_Name; }
bool DelayLineProcessor::acceptsMidi() const { return false; }
bool DelayLineProcessor::producesMidi() const { return false; }
bool DelayLineProcessor::isMidiEffect() const { return false; }
double DelayLineProcessor::getTailLengthSeconds() const { return 0.0; }
int DelayLineProcessor::getNumPrograms() { return 1; }
int DelayLineProcessor::getCurrentProgram() { return 0; }
void DelayLineProcessor::setCurrentProgram(int) { }
const juce::String DelayLineProcessor::getProgramName(int) { return {}; }
void DelayLineProcessor::changeProgramName(int, const juce::String&) { }
bool DelayLineProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* DelayLineProcessor::createEditor() { return new DelayLineProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayLineProcessor();
}