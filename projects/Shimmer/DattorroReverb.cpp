#include "DattorroReverb.h"

namespace DSP
{

DattorroReverb::DattorroReverb(
    double initSampleRate,
    unsigned int initNumChannels,
    float initDampingFilterCoeff,
    float initDampingCoeff
) :
    sampleRate { initSampleRate },
    preDelay(static_cast<unsigned int>(preDelayMs * static_cast<float>(0.001 * sampleRate)), 1u),
    toneControl(toneControlCoeff),
    inputDiffuser_1(inputDiffDelayMs_1, inputDiffCoeff_1_2, 1u),
    inputDiffuser_2(inputDiffDelayMs_2, inputDiffCoeff_1_2, 1u),
    inputDiffuser_3(inputDiffDelayMs_3, inputDiffCoeff_3_4, 1u),
    inputDiffuser_4(inputDiffDelayMs_4, inputDiffCoeff_3_4, 1u),
    lfo(lfoType, lfoFreqHz, lfoDepthMs, 0.f),
    decayDiffuser_left_1(decayDiffDelayMs_left_1 + lfoDepthMs, decayDiffCoeff_1, 1u),
    decayDiffuser_right_1(decayDiffDelayMs_right_1 + lfoDepthMs, decayDiffCoeff_1, 1u),
    delay_left_1(static_cast<unsigned int>(delayMs_left_1 * static_cast<float>(0.001 * sampleRate)), 1u),
    delay_right_1(static_cast<unsigned int>(delayMs_right_1 * static_cast<float>(0.001 * sampleRate)), 1u),
    dampingFilter(initDampingFilterCoeff),
    decayCoeff { initDampingCoeff },
    decayDiffuser_left_2(decayDiffDelayMs_left_2, decayDiffCoeff_2, 1u),
    decayDiffuser_right_2(decayDiffDelayMs_right_2, decayDiffCoeff_2, 1u),
    delay_left_2(static_cast<unsigned int>(delayMs_left_2 * static_cast<float>(0.001 * sampleRate)), 1u),
    delay_right_2(static_cast<unsigned int>(delayMs_right_2 * static_cast<float>(0.001 * sampleRate)), 1u)
{
    decayDiffuser_left_1.setDelayTime(decayDiffDelayMs_left_1);
    decayDiffuser_right_1.setDelayTime(decayDiffDelayMs_right_1);
    decayCoeffRamp.prepare(sampleRate, true, decayCoeff);
}

DattorroReverb::~DattorroReverb()
{
}

void DattorroReverb::prepare(double newSampleRate, unsigned int newNumChannels)
{   
    unsigned int numChannels = std::max(newNumChannels, MaxChannels);
    sampleRate = std::max(newSampleRate, 1.0);

    // Prepare pre-delay
    preDelay.prepare(static_cast<unsigned int>(preDelayMs * static_cast<float>(0.001 * sampleRate)), 1u);
    // Prepare tone control
    toneControl.prepare(sampleRate);
    // Prepare input diffusers
    inputDiffuser_1.prepare(sampleRate, 1u);
    inputDiffuser_2.prepare(sampleRate, 1u);
    inputDiffuser_3.prepare(sampleRate, 1u);
    inputDiffuser_4.prepare(sampleRate, 1u);
    // Prepare LFO
    lfo.prepare(sampleRate);
    // Prepare decay diffusers 1
    decayDiffuser_left_1.prepare(sampleRate, 1u);
    decayDiffuser_right_1.prepare(sampleRate, 1u);
    // Prepare delay lines 1
    delay_left_1.prepare(static_cast<unsigned int>(delayMs_left_1 * static_cast<float>(0.001 * sampleRate)), 1u);
    delay_right_1.prepare(static_cast<unsigned int>(delayMs_right_1 * static_cast<float>(0.001 * sampleRate)), 1u);
    // Prepare damping filter
    dampingFilter.prepare(sampleRate);
    // Prepare decay coefficient ramp
    decayCoeffRamp.prepare(sampleRate, true, decayCoeff);
    // Prepare decay diffusers 2
    decayDiffuser_left_2.prepare(sampleRate, 1u);
    decayDiffuser_right_2.prepare(sampleRate, 1u);
    // Prepare delay lines 2
    delay_left_2.prepare(static_cast<unsigned int>(delayMs_left_2 * static_cast<float>(0.001 * sampleRate)), 1u);
    delay_right_2.prepare(static_cast<unsigned int>(delayMs_right_2 * static_cast<float>(0.001 * sampleRate)), 1u);
    // Prepare feedback state
    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;

    clear();
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
    // Clear decay diffusers 1
    decayDiffuser_left_1.clear();
    decayDiffuser_right_1.clear();
    // Clear delay lines 1
    delay_left_1.clear();
    delay_right_1.clear();
    // Clear damping filter
    dampingFilter.clear();
    // Clear decay diffusers 2
    decayDiffuser_left_2.clear();
    decayDiffuser_right_2.clear();
    // Clear delay lines 2
    delay_left_2.clear();
    delay_right_2.clear();
    // Clear feedback state
    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;
}

void DattorroReverb::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{   
    // Sum the input channels to the first channel
    for (unsigned int n = 0; n < numSamples; ++n)
    {   
        // Preallocate output channels
        float out_left { 0.f };
        float out_right { 0.f };

        // --------- FEEDFORWARD ---------

        // Join stereo channels to mono
        float left { input[0][n] };
        float right { (numChannels > 1) ? input[1][n] : input[0][n] };
        float mono { 0.5f * (left + right) };

        // Predelay processing
        preDelay.process(&mono, &mono, 1u);

        // Tone control processing
        toneControl.process(&mono, &mono, 1u);

        // Input diffusion processing
        inputDiffuser_1.process(&mono, &mono, 1u);
        inputDiffuser_2.process(&mono, &mono, 1u);
        inputDiffuser_3.process(&mono, &mono, 1u);
        inputDiffuser_4.process(&mono, &mono, 1u);

        // ---------- RECURSION ----------

        // Divide in stereo channels
        left = mono + feedbackState[0];
        right = mono + feedbackState[1];

        // LFO for allpass delay line modulation
        float* lfoValue = lfo.process();

        // Decay Diffusion 1 processing
        decayDiffuser_left_1.process(&left, &left, 1u, &lfoValue[0]);
        decayDiffuser_right_1.process(&right, &right, 1u, &lfoValue[1]);

        // Delay line 1 processing
        delay_left_1.process(&left, &left, 1u);
        delay_right_1.process(&right, &right, 1u);

        // First and second tap out
        out_left += 0.6 * delay_right_1.getSample(0, static_cast<unsigned int>(tapOutMs_left_1 * static_cast<float>(0.001 * sampleRate)));
        out_left += 0.6 * delay_right_1.getSample(0, static_cast<unsigned int>(tapOutMs_left_2 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.6 * delay_left_1.getSample(0, static_cast<unsigned int>(tapOutMs_right_1 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.6 * delay_left_1.getSample(0, static_cast<unsigned int>(tapOutMs_right_2 * static_cast<float>(0.001 * sampleRate)));

        // Damping processing 1
        dampingFilter.process(&left, &left, 1u);
        dampingFilter.process(&right, &right, 1u);

        // Decay processing 1
        // left *= decayCoeff;
        decayCoeffRamp.applyGain(&left, 1u);
        // right *= decayCoeff;
        decayCoeffRamp.applyGain(&right, 1u);

        // Decay Diffusion 2 processing
        decayDiffuser_left_2.process(&left, &left, 1u);
        decayDiffuser_right_2.process(&right, &right, 1u);

        // Third tap out
        out_left -= 0.6 * decayDiffuser_right_2.getSample(0, static_cast<unsigned int>(tapOutMs_left_3 * static_cast<float>(0.001 * sampleRate)));
        out_right -= 0.6 * decayDiffuser_left_2.getSample(0, static_cast<unsigned int>(tapOutMs_right_3 * static_cast<float>(0.001 * sampleRate)));

        // Delay line 2 processing
        delay_left_2.process(&left, &left, 1u);
        delay_right_2.process(&right, &right, 1u);

        // Forth tap out
        out_left += 0.6 * delay_right_2.getSample(0, static_cast<unsigned int>(tapOutMs_left_4 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.6 * delay_left_2.getSample(0, static_cast<unsigned int>(tapOutMs_right_4 * static_cast<float>(0.001 * sampleRate)));

        // Decay processing 2
        // left *= decayCoeff;
        decayCoeffRamp.applyGain(&left, 1u);
        // right *= decayCoeff;
        decayCoeffRamp.applyGain(&right, 1u);

        // Fifth, sixth, and seventh tap out
        out_left -= 0.6 * delay_left_1.getSample(0, static_cast<unsigned int>(tapOutMs_left_5 * static_cast<float>(0.001 * sampleRate)));
        out_left -= 0.6 * decayDiffuser_left_2.getSample(0, static_cast<unsigned int>(tapOutMs_left_6 * static_cast<float>(0.001 * sampleRate)));
        out_left -= 0.6 * delay_left_2.getSample(0, static_cast<unsigned int>(tapOutMs_left_7 * static_cast<float>(0.001 * sampleRate)));
        out_right -= 0.6 * delay_right_1.getSample(0, static_cast<unsigned int>(tapOutMs_right_5 * static_cast<float>(0.001 * sampleRate)));
        out_right -= 0.6 * decayDiffuser_right_2.getSample(0, static_cast<unsigned int>(tapOutMs_right_6 * static_cast<float>(0.001 * sampleRate)));
        out_right -= 0.6 * delay_right_2.getSample(0, static_cast<unsigned int>(tapOutMs_right_7 * static_cast<float>(0.001 * sampleRate)));

        // Update feedback state
        feedbackState[0] = left;
        feedbackState[1] = right;

        // Output
        output[0][n] = out_left;
        output[1][n] = out_right;
    }
}

void DattorroReverb::setBrightness(float newCoeff)
{
    dampingFilter.setCoeff(newCoeff);
}

void DattorroReverb::setDecay(float newCoeff)
{
    decayCoeff = std::clamp(newCoeff, 0.f, 1.f);
    decayCoeffRamp.setTarget(decayCoeff);
}

}