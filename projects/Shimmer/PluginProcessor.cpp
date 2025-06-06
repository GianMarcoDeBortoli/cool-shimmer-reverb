#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>

static const std::vector<mrta::ParameterInfo> Parameters
{
    { Param::ID::Enabled,    Param::Name::Enabled,    Param::Ranges::EnabledOff,   Param::Ranges::EnabledOn, Param::Ranges::EnabledDefault },
    { Param::ID::Mix,        Param::Name::Mix,        "",                Param::Ranges::MixDefault,        Param::Ranges::MixMin,        Param::Ranges::MixMax,        Param::Ranges::MixInc,        Param::Ranges::MixSkw },
    // Pitch shifter parameters
    { Param::ID::Buildup,    Param::Name::Buildup,    Param::Units::Ms,  Param::Ranges::BuildupDefault,    Param::Ranges::BuildupMin,    Param::Ranges::BuildupMax,    Param::Ranges::BuildupInc,    Param::Ranges::BuildupSkw },
    { Param::ID::Shift1,     Param::Name::Shift1,     "",                Param::Ranges::Shift1Default,     Param::Ranges::Shift1Min,     Param::Ranges::Shift1Max,     Param::Ranges::Shift1Inc,     Param::Ranges::Shift1Skw },
    { Param::ID::Shift2,     Param::Name::Shift2,     "",                Param::Ranges::Shift2Default,     Param::Ranges::Shift2Min,     Param::Ranges::Shift2Max,     Param::Ranges::Shift2Inc,     Param::Ranges::Shift2Skw },
    { Param::ID::Amount,     Param::Name::Amount,     "",                Param::Ranges::AmountDefault,     Param::Ranges::AmountMin,     Param::Ranges::AmountMax,     Param::Ranges::AmountInc,     Param::Ranges::AmountSkw },
    // Keith Barr's reverb parameters
    { Param::ID::Damping,    Param::Name::Damping,    "",                Param::Ranges::DampCoeffDefault,  Param::Ranges::DampCoeffMin,  Param::Ranges::DampCoeffMax,  Param::Ranges::DampCoeffInc,  Param::Ranges::DampCoeffSkw },
    // Jon Dattorro's reverb parameters
    { Param::ID::Brightness, Param::Name::Brightness, "",                Param::Ranges::BrightnessDefault, Param::Ranges::BrightnessMin, Param::Ranges::BrightnessMax, Param::Ranges::BrightnessInc, Param::Ranges::BrightnessSkw },
    // Jon Dattorro's reverb parameters
    { Param::ID::Decay,      Param::Name::Decay,      "",                Param::Ranges::DecayDefault,      Param::Ranges::DecayMin,      Param::Ranges::DecayMax,      Param::Ranges::DecayInc,      Param::Ranges::DecaySkw },
};

ShimmerAudioProcessor::ShimmerAudioProcessor() :
    parameterManager(*this, ProjectInfo::projectName, Parameters),
    enabled { Param::Ranges::EnabledDefault },
    enableRamp(0.05f),
    mix { Param::Ranges::MixDefault },
    mixRamp(0.05f),
    // Shimmer effect
    shimmer(Param::Ranges::BuildupMax, 5.f, 2),
    // Pitch shifter parameters
    shift1 { Param::Ranges::Shift1Default },
    shift2 { Param::Ranges::Shift2Default },
    amount { Param::Ranges::AmountDefault },
    amountRamp(Param::Ranges::AmountDefault),
    buildUp { Param::Ranges::BuildupDefault },
    // Keith Barr's reverb effect
    KBReverb(MaxChannels, Param::Ranges::DampCoeffDefault),
    eq(2), // 2 band parametric equalizer
    // Keith Barr's reverb parameters
    dampingCoeff { Param::Ranges::DampCoeffDefault },
    // Jon Dattorro's reverb effect
    dattorroReverb(
        sampleRate,
        MaxChannels,
        Param::Ranges::BrightnessDefault,
        Param::Ranges::DecayDefault
    ),
    // Jon Dattorro's reverb parameters
    brightness { Param::Ranges::BrightnessDefault },
    decay { Param::Ranges::DecayDefault }
{
    // Set the Parameteric Equalizer 
    eq.setBandType(0, static_cast<DSP::ParametricEqualizer::FilterType>(std::round(2)));    // Low Shelf
    eq.setBandType(1, static_cast<DSP::ParametricEqualizer::FilterType>(std::round(5)));    // High Shelf 
    eq.setBandFrequency(0, 15000.f); // Low Shelf frequency
    eq.setBandFrequency(1, 100.f);   // High Shelf frequency
    eq.setBandResonance(0, 0.7071f); // Low Shelf resonance
    eq.setBandResonance(1, 0.7071f); // High Shelf resonance
    eq.setBandGain(0, 8.1f);         // Low Shelf gain ( I found these gain values from PlugInDoctor, It is going to give a boost unfortunately)
    eq.setBandGain(1, 8.1f);         // High Shelf gain


    // CALLBACKS
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });
    parameterManager.registerParameterCallback(Param::ID::Mix,
    [this](float newMix, bool /*force*/)
    {
        mix = std::clamp(newMix, Param::Ranges::MixMin, Param::Ranges::MixMax);
        mixRamp.setTarget(mix);
    });
    // Pitch Shifter Parameters
    parameterManager.registerParameterCallback(Param::ID::Amount,
    [this] (float value, bool /*force*/)
    {
        amount = std::clamp(value, Param::Ranges::AmountMin, Param::Ranges::AmountMax);;
        amountRamp.setTarget(amount);
    });
    parameterManager.registerParameterCallback(Param::ID::Buildup,
    [this] (float value, bool /*force*/)
    {
        buildUp = std::clamp(value, Param::Ranges::BuildupMin, Param::Ranges::BuildupMax);
        shimmer.setBuildup(buildUp);
    });

    parameterManager.registerParameterCallback(Param::ID::Shift1,
    [this] (float value, bool /*force*/)
    {
        shift1 = std::clamp(value, Param::Ranges::Shift1Min, Param::Ranges::Shift1Max);
        shimmer.setRatio1(shift1);
    });

    parameterManager.registerParameterCallback(Param::ID::Shift2,
    [this] (float value, bool /*force*/)
    {
        shift2 = std::clamp(value, Param::Ranges::Shift2Min, Param::Ranges::Shift2Max);
        shimmer.setRatio2(shift2);
    });
    // Keith Barr Reverb Parameters
    parameterManager.registerParameterCallback(Param::ID::Damping,
    [this] (float value, bool /*force*/)
    {
        dampingCoeff = std::clamp(value, Param::Ranges::DampCoeffMin, Param::Ranges::DampCoeffMax);
        KBReverb.setDampingCoeff(dampingCoeff);
    });
    parameterManager.registerParameterCallback(Param::ID::Brightness,
    [this](float newBrightness, bool /*force*/)
    {
        brightness = std::clamp(newBrightness, Param::Ranges::BrightnessMin, Param::Ranges::BrightnessMax);
        dattorroReverb.setBrightness(brightness);
    });
    // Jon Dattorro's reverb parameters
    parameterManager.registerParameterCallback(Param::ID::Decay,
    [this](float newDecay, bool /*force*/)
    {
        decay = std::clamp(newDecay, Param::Ranges::DecayMin, Param::Ranges::DecayMax);
        dattorroReverb.setDecay(decay);
    });
}

ShimmerAudioProcessor::~ShimmerAudioProcessor()
{
}

void ShimmerAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    sampleRate = std::max(newSampleRate, 1.0);

    shimmer.prepare(sampleRate, Param::Ranges::BuildupMax, numChannels, samplesPerBlock);
    eq.prepare(sampleRate, numChannels);
    KBReverb.prepare(sampleRate, numChannels);
    amountRamp.prepare(sampleRate, true, Param::Ranges::AmountDefault);
    dattorroReverb.prepare(sampleRate, numChannels);

    enableRamp.prepare(sampleRate, true, enabled ? 1.f : 0.f);
    mixRamp.prepare(sampleRate, true, mix);

    shimmerBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    shimmerBuffer.clear();

    reverbBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    reverbBuffer.clear();

    dryBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    dryBuffer.clear();

    parameterManager.updateParameters(true);
}

void ShimmerAudioProcessor::releaseResources()
{
    shimmer.clear();
    eq.clear();
    KBReverb.clear();
    dattorroReverb.clear();
    shimmerBuffer.clear();
    reverbBuffer.clear();
    dryBuffer.clear();
}

void ShimmerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
    {
        shimmerBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));
        reverbBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));
    } 

    shimmer.process(shimmerBuffer.getArrayOfWritePointers(), shimmerBuffer.getArrayOfReadPointers(), numChannels, numSamples);
    eq.process(shimmerBuffer.getArrayOfWritePointers(), shimmerBuffer.getArrayOfReadPointers(), numChannels, numSamples);
    KBReverb.process(shimmerBuffer.getArrayOfWritePointers(), shimmerBuffer.getArrayOfReadPointers(), numChannels, numSamples);
    // Add KR reverb
    amountRamp.applyGain(shimmerBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    // Input to Dattorro reverb
    amountRamp.applyInverseGain(reverbBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        reverbBuffer.addFrom(ch, 0, shimmerBuffer, ch, 0, static_cast<int>(numSamples));
    // Add Dattorro reverb
    dattorroReverb.process(reverbBuffer.getArrayOfWritePointers(), reverbBuffer.getArrayOfReadPointers(), numChannels, numSamples);
    mixRamp.applyGain(reverbBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    enableRamp.applyGain(reverbBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    
    // Add dry and wet signals
    mixRamp.applyInverseGain(dryBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
    {
        buffer.addFrom(ch, 0, dryBuffer, ch, 0, static_cast<int>(numSamples));
        buffer.addFrom(ch, 0, reverbBuffer, ch, 0, static_cast<int>(numSamples));
    }
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