#pragma once

#include <JuceHeader.h>

#include "DattorroReverb.h"
#include "Ramp.h"
#include "juce_core/juce_core.h"

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String PreDelay { "predelay" };
        static const juce::String ToneControl { "tone_control" };
        static const juce::String InDiffCoeff_1 { "input_diffusion_coeff_1" };
        static const juce::String InDiffCoeff_2 { "input_diffusion_coeff_2" };
        static const juce::String LFOType { "lfo_type" };
        static const juce::String LFOFreqHz { "lfo_freq_hz" };
        static const juce::String LFOOffsetMs { "lfo_offset_ms" };
        static const juce::String LFODepthMs { "lfo_depth_ms" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String PreDelay { "Predelay" };
        static const juce::String ToneControl { "Tone Control" };
        static const juce::String InDiffCoeff_1 { "Input Diffusion 1" };
        static const juce::String InDiffCoeff_2 { "Input Diffusion 2" };
        static const juce::String LFOType { "LFO Type" };
        static const juce::String LFOFreqHz { "LFO Frequency (Hz)" };
        static const juce::String LFOOffsetMs { "LFO Offset (ms)" };
        static const juce::String LFODepthMs { "LFO Depth (ms)" };
    }

    namespace Range
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };
        static constexpr bool EnabledDefault { true };
        
        static constexpr float PreDelayDefault { 10.f };
        static constexpr float PreDelayMin { 0.f };
        static constexpr float PreDelayMax { 1000.f };
        static constexpr float PreDelayInc { 1.f };
        static constexpr float PreDelaySkw { 1.f };

        static constexpr float ToneControlDefault { 0.5f };
        static constexpr float ToneControlMin { 0.f };
        static constexpr float ToneControlMax { 1.f };
        static constexpr float ToneControlInc { 0.01f };
        static constexpr float ToneControlSkw { 0.5f };

        static constexpr float InDiffCoeffDefault { 0.5f };
        static constexpr float InDiffCoeffMin { 0.f };
        static constexpr float InDiffCoeffMax { 1.f };
        static constexpr float InDiffCoeffInc { 0.01f };
        static constexpr float InDiffCoeffSkw { 0.5f };

        static const juce::String LFOTypeSin { "Sine" };
        static const juce::String LFOTypeTri { "Triangle" };
        static constexpr unsigned int LFOTypeDefault { 0 };

        static constexpr float LFOFreqDefault { 10.f };
        static constexpr float LFOFreqMin { 0.f };
        static constexpr float LFOFreqMax { 10.f };
        static constexpr float LFOFreqInc { 0.01f };
        static constexpr float LFOFreqSkw { 1.f };

        static constexpr float LFODepthDefault { 10.f };
        static constexpr float LFODepthMin { 0.f };
        static constexpr float LFODepthMax { 20.f };
        static constexpr float LFODepthInc { 1.f };
        static constexpr float LFODepthSkw { 1.f };
    }

    namespace Units
    {
        static const juce::String Ms { "ms" };
        static const juce::String Hz { "Hz" };
    }
}

class DattorroReverbProcessor : public juce::AudioProcessor
{
public:
    DattorroReverbProcessor();
    ~DattorroReverbProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParameterManager() { return parameterManager; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    //==============================================================================

    static constexpr int MaxChannels { 2 };
    static constexpr float MaxPreDelay { Param::Range::PreDelayMax };
    static constexpr float inputDiffDelayMs_1 { 142.f / 30000.f };
    static constexpr float inputDiffDelayMs_2 { 107.f / 30000.f };
    static constexpr float inputDiffDelayMs_3 { 379.f / 30000.f };
    static constexpr float inputDiffDelayMs_4 { 277.f / 30000.f };

private:
    mrta::ParameterManager parameterManager;
    // Sample rate
    double sampleRate { 48000.0 };
    //Enable/Disable the effect
    DSP::Ramp<float> enableRamp;
    bool enabled { true };
    // Dattorro Reverb instance
    DSP::DattorroReverb dattorroReverb;
    juce::AudioBuffer<float> revBuffer;
    // Predelay
    float preDelayMs;
    // Tone control
    float toneControl;
    // Input diffusion
    float inputDiffusionCoeff_1;
    float inputDiffusionCoeff_2;
    // LFO modulation
    DSP::LFO::LFOType lfoType;
    float lfoFreqHz;
    float lfoDepthMs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DattorroReverbProcessor)
};