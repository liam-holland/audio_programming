/* Challenges:

1) change the oscillator type so that it is a square wave (±0.5) instead of a sinusoid

 2) change the oscillator type so that it is a linear ramp (±0.5) instead of a sinusoid

 3) change the oscillator type so that it is a triangle wave (±0.5) instead of a sinusoid


*/

#include <iostream>
#include <vector>
#include <cmath>                // for sine and M_PI
#include "MattsAudioHeader.h"

int main(int argc, const char * argv[]) {

    int sampleRate = 44100;

    int duration = 5;                               // duration in seconds
    int durationInSamples = sampleRate * duration;  // total number of samples for the whole output

    std::vector<float> samples (durationInSamples);  // create a float vector to store all of our samples

    float frequency = 440;                          // the frequency of the tone to generate

    float phase = 0.0;                                  // keep track of the phase of our oscillator
    float phaseDelta = frequency  / float(sampleRate);  // phaseDelta - how much increment the phase by for each sample


    // ************
    // OUR DSP loop
    for (int i=0; i<durationInSamples; i++)
    {
        // increment the phase a little
        phase += phaseDelta;

        // make sure the phase is in the range 0-1
        if (phase > 1.0)
        {
            phase -= 1.0;
        }

        // calculate the sine tone value from this phase --> with sin(phase * 2 * pi)
        float sample = sin( phase * 2 * M_PI );

        // store this sample in our samples vector (also halving the volume)
        samples[i] = 0.5 * sample;

    }
    // END OF DSP LOOP
    // ***********

    // write to wav file (desktop)
    writeToWav(samples, "challenge_4_4_osc_types.wav");


    std::cout << "Done!\n";
    return 0;
}
