#include "KeithBarrReverb.h"

namespace DSP
{

KeithBarrReverb::KeithBarrReverb(
    unsigned int initNumChannels,
    float initDampingFilterCoeff,
    float initDampingCoeff

) :
    // initialize input All Pass filters (mono)
    inputAllPass_1(inputDiffDelayMs_1, AllPassCoeff, 1u),
    inputAllPass_2(inputDiffDelayMs_2, AllPassCoeff, 1u),
    inputAllPass_3(inputDiffDelayMs_3, AllPassCoeff, 1u),
    inputAllPass_4(inputDiffDelayMs_4, AllPassCoeff, 1u),
    // initialize ring All Pass filters (mono)
    ringAllPass_1(ringDelayMs_1, AllPassCoeff, 1u),
    ringAllPass_2(ringDelayMs_2, AllPassCoeff, 1u),
    ringAllPass_3(ringDelayMs_3, AllPassCoeff, 1u),
    ringAllPass_4(ringDelayMs_4, AllPassCoeff, 1u),
    // initialize delay lines (stereo output)
    delay_1(static_cast<unsigned int>(delayMs_1 * static_cast<float>(0.001 * sampleRate)), 1u),
    delay_2(static_cast<unsigned int>(delayMs_2 * static_cast<float>(0.001 * sampleRate)), 1u),
    delay_3(static_cast<unsigned int>(delayMs_3 * static_cast<float>(0.001 * sampleRate)), 1u),
    delay_4(static_cast<unsigned int>(delayMs_4 * static_cast<float>(0.001 * sampleRate)), 1u),
    // dampling filters
    dampingFilter_1(initDampingFilterCoeff),
    dampingFilter_2(initDampingFilterCoeff),
    dampingFilter_3(initDampingFilterCoeff),
    dampingFilter_4(initDampingFilterCoeff),
    decayCoeff { initDampingFilterCoeff }
{
}

KeithBarrReverb::~KeithBarrReverb()
{
}

void KeithBarrReverb::prepare(double newSampleRate, unsigned int newNumChannels)
{   
    unsigned int numChannels = std::max(newNumChannels, MaxChannels);
    sampleRate = std::max(newSampleRate, 1.0);

    // Prepare input allpass
    inputAllPass_1.prepare(sampleRate, 1u);
    inputAllPass_2.prepare(sampleRate, 1u);
    inputAllPass_3.prepare(sampleRate, 1u);
    inputAllPass_4.prepare(sampleRate, 1u);
    // Prepare ring allpass
    ringAllPass_1.prepare(sampleRate, 1u);
    ringAllPass_2.prepare(sampleRate, 1u);
    ringAllPass_3.prepare(sampleRate, 1u);
    ringAllPass_4.prepare(sampleRate, 1u);
    // Prepare delay lines
    delay_1.prepare(static_cast<unsigned int>(delayMs_1 * static_cast<float>(0.001 * sampleRate)), 1u);
    delay_2.prepare(static_cast<unsigned int>(delayMs_2 * static_cast<float>(0.001 * sampleRate)), 1u);
    delay_3.prepare(static_cast<unsigned int>(delayMs_3 * static_cast<float>(0.001 * sampleRate)), 1u);
    delay_4.prepare(static_cast<unsigned int>(delayMs_4 * static_cast<float>(0.001 * sampleRate)), 1u);
    // Prepare damping filter
    dampingFilter_1.prepare(sampleRate);
    dampingFilter_2.prepare(sampleRate);
    dampingFilter_3.prepare(sampleRate);
    dampingFilter_4.prepare(sampleRate);

    // Prepare feedback state
    feedbackState_1 = 0.f;
    feedbackState_2 = 0.f;
    feedbackState_3 = 0.f;
    feedbackState_4 = 0.f;
}

void KeithBarrReverb::clear()
{
    // Clear input all pass filters
    inputAllPass_1.clear();
    inputAllPass_2.clear();
    inputAllPass_3.clear();
    inputAllPass_4.clear();
    // Clear ring all pass filters
    ringAllPass_1.clear();
    ringAllPass_2.clear();
    ringAllPass_3.clear();
    ringAllPass_4.clear();
    // Clear delay lines
    delay_1.clear();
    delay_2.clear();
    delay_3.clear();
    delay_4.clear();
    // Clear damping filter
    dampingFilter_1.clear();
    dampingFilter_2.clear();
    dampingFilter_3.clear();
    dampingFilter_4.clear();
    // Clear feedback state
    feedbackState_1 = 0.f;
    feedbackState_2 = 0.f;
    feedbackState_3 = 0.f;
    feedbackState_4 = 0.f;
}

void KeithBarrReverb::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{   
    // Sum the input channels to the first channel
    for (unsigned int n = 0; n < numSamples; ++n)
    {   
        // Preallocate output channels
        float out_left { 0.f };
        float out_right { 0.f };

        // Join stereo channels to mono
        float left { input[0][n] };
        float right { (numChannels > 1) ? input[1][n] : input[0][n] };
        float mono { 0.5f * (left + right) };

        // ---------- INPUT ALLPASS ----------
        inputAllPass_1.process(&mono, &mono, 1u);
        inputAllPass_2.process(&mono, &mono, 1u);
        inputAllPass_3.process(&mono, &mono, 1u);
        inputAllPass_4.process(&mono, &mono, 1u);

        // ---------- RING ----------

        // Update inputs of the ring all pass filters
        float input_1 = mono + feedbackState_4;
        float input_2 = mono + feedbackState_1;
        float input_3 = mono + feedbackState_2;
        float input_4 = mono + feedbackState_3;

        float output_1 = 0.f;
        float output_2 = 0.f;
        float output_3 = 0.f;
        float output_4 = 0.f;
        
        // ring all pass processing
        ringAllPass_1.process(&input_1, &input_1, 1u);
        ringAllPass_2.process(&input_2, &input_2, 1u);
        ringAllPass_3.process(&input_3, &input_3, 1u);
        ringAllPass_4.process(&input_4, &input_4, 1u);   

        // Delay line processing
        delay_1.process(&output_1, &input_1, 1u);
        delay_2.process(&output_2, &input_2, 1u);
        delay_3.process(&output_3, &input_3, 1u);
        delay_4.process(&output_4, &input_4, 1u);

        // Read outputs
        out_left += 0.25 * delay_1.getSample(0, static_cast<unsigned int>(tapOutMs_left_1 * static_cast<float>(0.001 * sampleRate)));
        out_left += 0.25 * delay_2.getSample(0, static_cast<unsigned int>(tapOutMs_left_2 * static_cast<float>(0.001 * sampleRate)));
        out_left += 0.25 * delay_3.getSample(0, static_cast<unsigned int>(tapOutMs_left_3 * static_cast<float>(0.001 * sampleRate)));
        out_left += 0.25 * delay_4.getSample(0, static_cast<unsigned int>(tapOutMs_left_4 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.25 * delay_1.getSample(0, static_cast<unsigned int>(tapOutMs_right_1 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.25 * delay_2.getSample(0, static_cast<unsigned int>(tapOutMs_right_2 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.25 * delay_3.getSample(0, static_cast<unsigned int>(tapOutMs_right_3 * static_cast<float>(0.001 * sampleRate)));
        out_right += 0.25 * delay_4.getSample(0, static_cast<unsigned int>(tapOutMs_right_4 * static_cast<float>(0.001 * sampleRate)));

        // Damping processing and update feedback state
        dampingFilter_1.process(&feedbackState_1, &output_1, 1u);
        dampingFilter_2.process(&feedbackState_2, &output_2, 1u);
        dampingFilter_3.process(&feedbackState_3, &output_3, 1u);
        dampingFilter_4.process(&feedbackState_4, &output_4, 1u);

        // Output
        output[0][n] = out_left;
        output[1][n] = out_right;
    }
}


void KeithBarrReverb::setDampingFilterCoeff(float newCoeff)
{
    dampingFilter_1.setCoeff(newCoeff);
    dampingFilter_2.setCoeff(newCoeff);
    dampingFilter_3.setCoeff(newCoeff);
    dampingFilter_4.setCoeff(newCoeff);
}
}