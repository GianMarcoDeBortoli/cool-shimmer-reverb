#include "PluginProcessor.h"
#include "PluginEditor.h"

static const std::vector<mrta::ParameterInfo> ParameterInfos
{
    { Param::ID::PitchRatio, Param::Name::PitchRatio, "", 1.0f, 0.25f, 2.0f, 0.01f, 1.0f }
};

PitchShifterProcessor::PitchShifterProcessor()
    : parameterManager(*this, ProjectInfo::projectName, ParameterInfos),
      pitchShifter(20.0f, 2) // maxDelay = 1s, blockSize = 20ms, stereo
{
    parameterManager.registerParameterCallback(Param::ID::PitchRatio,
        [this](float value, bool forced)
        {
            DBG("Pitch Ratio: " << value);
            pitchShifter.setPitchRatio(value);
        });
}

PitchShifterProcessor::~PitchShifterProcessor() {}

void PitchShifterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    pitchShifter.prepare(sampleRate);
    parameterManager.updateParameters(true);
}

void PitchShifterProcessor::releaseResources()
{
    pitchShifter.clear();
}

void PitchShifterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    float* const* writePtrs = buffer.getArrayOfWritePointers();
    const float* const* readPtrs = buffer.getArrayOfReadPointers();

    pitchShifter.process(writePtrs, readPtrs, buffer.getNumChannels(), buffer.getNumSamples());
}

juce::AudioProcessorEditor* PitchShifterProcessor::createEditor()
{
    return new PitchShifterEditor(*this);
}

bool PitchShifterProcessor::hasEditor() const { return true; }
const juce::String PitchShifterProcessor::getName() const { return JucePlugin_Name; }

bool PitchShifterProcessor::acceptsMidi() const { return false; }
bool PitchShifterProcessor::producesMidi() const { return false; }
bool PitchShifterProcessor::isMidiEffect() const { return false; }

double PitchShifterProcessor::getTailLengthSeconds() const { return 0.0; }

int PitchShifterProcessor::getNumPrograms() { return 1; }
int PitchShifterProcessor::getCurrentProgram() { return 0; }
void PitchShifterProcessor::setCurrentProgram(int) {}
const juce::String PitchShifterProcessor::getProgramName(int) { return {}; }
void PitchShifterProcessor::changeProgramName(int, const juce::String&) {}

void PitchShifterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void PitchShifterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchShifterProcessor();
}