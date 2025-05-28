#include "DattorroReverb.h"

namespace DSP
{

DattorroReverb::DattorroReverb(
    unsigned int initNumChannels,
    unsigned int initPreDelaySamples,
    float initToneControlCoeff,
    float initInputDiffusionDelayMs_1,
    float initInputDiffusionDelayMs_2,
    float initInputDiffusionCoeff_1_2,
    float initInputDiffusionDelayMs_3,
    float initInputDiffusionDelayMs_4,
    float initInputDiffusionCoeff_3_4,
    LFO::LFOType initLFOType,
    float initLFOFreqHz,
    float initLFODepthMs
) :
    preDelay(initPreDelaySamples, 1u),
    toneControl(initToneControlCoeff),
    inputDiffuser_1(initInputDiffusionDelayMs_1, initInputDiffusionCoeff_1_2, 1u),
    inputDiffuser_2(initInputDiffusionDelayMs_2, initInputDiffusionCoeff_1_2, 1u),
    inputDiffuser_3(initInputDiffusionDelayMs_3, initInputDiffusionCoeff_3_4, 1u),
    inputDiffuser_4(initInputDiffusionDelayMs_4, initInputDiffusionCoeff_3_4, 1u),
    lfo(initLFOType, initLFOFreqHz, initLFODepthMs)
{
}

DattorroReverb::~DattorroReverb()
{
}

void DattorroReverb::prepare(double newSampleRate, unsigned int newNumChannels, unsigned int preDelaySamples)
{   
    unsigned int numChannels = std::max(newNumChannels, MaxChannels);

    // Prepare pre-delay
    preDelay.prepare(preDelaySamples, 1u);
    // Prepare tone control
    toneControl.prepare(newSampleRate);
    // Prepare input diffusers
    inputDiffuser_1.prepare(newSampleRate, 1u);
    inputDiffuser_2.prepare(newSampleRate, 1u);
    inputDiffuser_3.prepare(newSampleRate, 1u);
    inputDiffuser_4.prepare(newSampleRate, 1u);
    // Prepare LFO
    lfo.prepare(newSampleRate);
}

void DattorroReverb::clear()
{
    // Clear pre-delay
    preDelay.clear();
    // Clear tone control
    toneControl.clear();
    // Clear input diffusers
    inputDiffuser_1.clear();
    inputDiffuser_2.clear();
    inputDiffuser_3.clear();
    inputDiffuser_4.clear();
}

void DattorroReverb::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{   
    // Sum the input channels to the first channel
    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Join stereo channels to mono
        float left = input[0][n];
        float right = (numChannels > 1) ? input[1][n] : input[0][n];
        float mono { 0.5f * (left + right) };

        // Predelay processing
        preDelay.process(&mono, &mono, 1u);

        // Tone control processing
        toneControl.process(&mono, &mono, 1u);

        // LFO for allpass delay line modulation
        float lfoValue = lfo.process();

        // Input diffusion processing
        inputDiffuser_1.process(&mono, &mono, 1u, &lfoValue);
        // inputDiffuser_2.process(&mono, &mono, 1u);
        // inputDiffuser_3.process(&mono, &mono, 1u);
        // inputDiffuser_4.process(&mono, &mono, 1u);

        // Output
        for (unsigned int ch = 0; ch < numChannels; ++ch)
        {
            output[ch][n] = 0.5 * ( mono );
        }
    }
}

void DattorroReverb::setPreDelay(unsigned int newPreDelaySamples)
{
    preDelay.setDelaySamples(newPreDelaySamples);
}

void DattorroReverb::setToneControl(float newCoeff)
{
    toneControl.setCoeff(newCoeff);
}

void DattorroReverb::setInputDiffusionCoeff_1(float newDiffCoeff)
{
    inputDiffuser_1.setCoeff(newDiffCoeff);
    inputDiffuser_2.setCoeff(newDiffCoeff);
}

void DattorroReverb::setInputDiffusionCoeff_2(float newDiffCoeff)
{
    inputDiffuser_3.setCoeff(newDiffCoeff);
    inputDiffuser_4.setCoeff(newDiffCoeff);
}

void DattorroReverb::setLFOtype(LFO::LFOType newModType)
{
    lfo.setType(newModType);
}

void DattorroReverb::setLFOfreqHz(float newModRateHz)
{
    lfo.setFrequency(newModRateHz);
}

void DattorroReverb::setLFOdepthMs(float newModDepthMs)
{
    lfo.setDepth(newModDepthMs);
}

}