#include <iostream>
#include <vector>
#include <cmath>                    // for the inverse tangent function, sin, M_PI
#include "MattsAudioHeader.h"       // for the writeWavFile function


float clipDistortion(float inSample, float threshold)
{
    
    float distortedSample = inSample; // a placeholder for our distortion process

    if (distortedSample > threshold) 
    {
        distortedSample = threshold;        
    }
    else if (distortedSample < -threshold) 
    {
       distortedSample = -threshold; 
    }
    
    return distortedSample;           // return the result back to the main program that called the function
}

float tanDistortion(float inputSample, float inputGainValue)
{
    float scaledInput{ inputSample * inputGainValue };

    return clipDistortion(tanhf(scaledInput),inputGainValue);
}



int main(int argc, const char * argv[]) {

    // set up DSP things
    float duration = 5.0f;
    float sampleRate = 44100;
    int durationInSamples = duration * sampleRate;

    float gain{100.0};

    // set up a sine oscillator to test our distortion with
    float frequency = 220.0f;            // freq in Hz
    float phase = 0.0;              // keep track of osc phase
    float phaseDelta = frequency  / sampleRate;  // phaseDelta - how much increment the phase by for each sample

    // vector of floats
    std::vector<float> samples (durationInSamples);


    // DSP LOOP
    for (int i = 0 ; i < durationInSamples; i++)
    {


        // STEP 1:   get sine value from current phase
        float sineSample = sin( phase * 2 * 3.141593 );

        // STEP 2:   APPLY DISTORTION
        // distort the sine sample and write it into the samples array
        samples[i] = tanDistortion(sineSample,gain);


        // STEP 3:   UPDATE PHASE
        // increment the phase a little
        phase += phaseDelta;

        // make sure the phase is in the range 0-1
        if (phase > 1.0)
            phase -= 1.0;
    }
    // END DSP LOOP

    // write to wav file (desktop)
    writeToWav(samples, "challenge_5_3_distortion.wav");

    std::cout << "done playing audio\n";
    return 0;
}
