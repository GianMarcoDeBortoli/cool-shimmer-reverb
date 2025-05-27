#include "DattorroReverb.h"

#include <algorithm>
#include <cmath>

namespace DSP
{

DattorroReverb::DattorroReverb(double sampleRate, unsigned int numChannels, float preDelayTimeMs, float bandwidth, float inputDiffusion1, float inputDiffusion2, float decayDiffusion1, float decayDiffusion2, float damping, float decay) :
    preDelay(static_cast<unsigned int>(std::ceil(preDelayTimeMs * 0.001f * sampleRate)), numChannels),
    preDiffusionFilter(bandwidth),
    inputDiffusion1a(InputDiffusion1a_Delay, numChannels),
    inputDiffusion1b(InputDiffusion1b_Delay, numChannels),
    inputDiffusion2a(InputDiffusion2a_Delay, numChannels),
    inputDiffusion2b(InputDiffusion2b_Delay, numChannels),
    decayDiffusion1L(DecayDiffusion1L_Delay, numChannels),
    decayDelay1L(DecayDelay1L_Delay, numChannels),
    dampingL(damping),
    decayDiffusion2L(DecayDiffusion2L_Delay, numChannels),
    decayDelay2L(DecayDelay2L_Delay, numChannels),
    decayDiffusion1R(DecayDiffusion1R_Delay, numChannels),
    decayDelay1R(DecayDelay1R_Delay, numChannels),
    dampingR(damping),
    decayDiffusion2R(DecayDiffusion2R_Delay, numChannels),
    decayDelay2R(DecayDelay2R_Delay, numChannels)
{
    sampleRate = std::fmax(sampleRate, 1.f);
    inputDiffusion1a.setCoeff(std::clamp(inputDiffusion1, 0.f, MaxDiffusion));
    inputDiffusion1b.setCoeff(std::clamp(inputDiffusion1, 0.f, MaxDiffusion));
    inputDiffusion2a.setCoeff(std::clamp(inputDiffusion2, 0.f, MaxDiffusion));
    inputDiffusion2b.setCoeff(std::clamp(inputDiffusion2, 0.f, MaxDiffusion));
    decayCoeff = std::clamp(decay, 0.f, MaxDecay);
    decayDiffusion1L.setCoeff(std::clamp(decayDiffusion1, 0.f, MaxDiffusion));
    decayDiffusion2L.setCoeff(std::clamp(decayDiffusion2, 0.f, MaxDiffusion));
    decayDiffusion1R.setCoeff(std::clamp(decayDiffusion1, 0.f, MaxDiffusion));
    decayDiffusion2R.setCoeff(std::clamp(decayDiffusion2, 0.f, MaxDiffusion));
}

DattorroReverb::~DattorroReverb()
{
}

void DattorroReverb::clear()
{
    preDelay.clear();
    preDiffusionFilter.clear();
    inputDiffusion1a.clear();
    inputDiffusion1b.clear();
    inputDiffusion2a.clear();
    inputDiffusion2b.clear();
    decayDiffusion1L.clear();
    decayDelay1L.clear();
    dampingL.clear();
    decayDiffusion2L.clear();
    decayDelay2L.clear();
    decayDiffusion1R.clear();
    decayDelay1R.clear();
    dampingR.clear();
    decayDiffusion2R.clear();
    decayDelay2R.clear();
}

void DattorroReverb::prepare(double newSampleRate, unsigned int numChannels)
{
    sampleRate = newSampleRate;

    // Prepare pre-delay
    preDelay.prepare(static_cast<unsigned int>(std::ceil(preDelayTimeMs * 0.001f * sampleRate)), numChannels);
    preDiffusionFilter.prepare(sampleRate);

    // Prepare input diffusion
    inputDiffusion1a.prepare(sampleRate, InputDiffusion1a_Delay, numChannels);
    inputDiffusion1b.prepare(sampleRate, InputDiffusion1b_Delay, numChannels);
    inputDiffusion2a.prepare(sampleRate, InputDiffusion2a_Delay, numChannels);
    inputDiffusion2b.prepare(sampleRate, InputDiffusion2b_Delay, numChannels);

    // Prepare decay diffusion left channel
    decayDiffusion1L.prepare(sampleRate, DecayDiffusion1L_Delay, numChannels);
    decayDelay1L.prepare(static_cast<unsigned int>(std::ceil(DecayDelay1L_Delay * sampleRate)), numChannels);
    dampingL.prepare(sampleRate);
    decayDiffusion2L.prepare(sampleRate, DecayDiffusion2L_Delay, numChannels);
    decayDelay2L.prepare(static_cast<unsigned int>(std::ceil(DecayDelay2L_Delay * sampleRate)), numChannels);

    // Prepare decay diffusion right channel
    decayDiffusion1R.prepare(sampleRate, DecayDiffusion1R_Delay, numChannels);
    decayDelay1R.prepare(static_cast<unsigned int>(std::ceil(DecayDelay1R_Delay * sampleRate)), numChannels);
    dampingR.prepare(sampleRate);
    decayDiffusion2R.prepare(sampleRate, DecayDiffusion2R_Delay, numChannels);
    decayDelay2R.prepare(static_cast<unsigned int>(std::ceil(DecayDelay2R_Delay * sampleRate)), numChannels);

    // Clear buffers
    clear();
}

void DattorroReverb::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    
}

void DattorroReverb::process(float* output, const float* input, unsigned int numChannels)
{
    
}

void DattorroReverb::process(float* const* audioOutput, const float* const* audioInput, const float* const* modInput, unsigned int numChannels, unsigned int numSamples)
{
    
}

void DattorroReverb::process(float* audioOutput, const float* audioInput, const float* modInput, unsigned int numChannels)
{
    
}

void DattorroReverb::setPreDelayTime(float newPreDelayMs)
{
    unsigned int newDelaySamples = static_cast<unsigned int>(std::round(preDelayTimeMs * 0.001f * sampleRate));
    preDelay.setDelaySamples(newDelaySamples);
}

void DattorroReverb::setPreDiffusionBandwidth(float newBandwidth)
{
    float newValue = std::clamp(newBandwidth, 0.0f, MaxBandwidth);
    preDiffusionFilter.setCoeff(newValue);
}

void DattorroReverb::setInputDiffusion1(float newCoeff)
{
    float newValue = std::clamp(newCoeff, 0.0f, MaxDiffusion);
    inputDiffusion1a.setCoeff(newValue);
    inputDiffusion1b.setCoeff(newValue);
}

void DattorroReverb::setInputDiffusion2(float newCoeff)
{
    float newValue = std::clamp(newCoeff, 0.0f, MaxDiffusion);
    inputDiffusion2a.setCoeff(newValue);
    inputDiffusion2b.setCoeff(newValue);
}

void DattorroReverb::setDecayDiffusion1(float newCoeff)
{
    float newValue = std::clamp(newCoeff, 0.0f, MaxDiffusion);
    decayDiffusion1L.setCoeff(newValue);
    decayDiffusion1R.setCoeff(newValue);
}

void DattorroReverb::setDamping(float newDamping)
{
    float newValue = std::clamp(newDamping, 0.0f, MaxDamping);
    dampingL.setCoeff(newValue);
    dampingR.setCoeff(newValue);
}

void DattorroReverb::setDecayDiffusion2(float newCoeff)
{
    float newValue = std::clamp(newCoeff, 0.0f, MaxDiffusion);
    decayDiffusion2L.setCoeff(newValue);
    decayDiffusion2R.setCoeff(newValue);
}

void DattorroReverb::setDecayCoefficient(float newDecayCoeff)
{
    decayCoeff = std::clamp(newDecayCoeff, 0.0f, MaxDecay);
}

}