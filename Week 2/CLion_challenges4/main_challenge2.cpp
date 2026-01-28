
// FIND THE ERRORS!
// This code should compile and run, but we don't hear the 5-second tone. Why not?

// The code needs a few changes, and many of the changes relate to one particular issue
// See if you can spot it, and if it is not clear (it is not obvious!) then look at the previous challenge to try to spot the difference



#include <iostream>
#include <cmath>                // for sine and M_PI
#include <vector>
#include "MattsAudioHeader.h"

int main() {
    
    int sampleRate = 44100;
    
    int duration = 5;                               // duration in seconds
    int numberOfSamples = sampleRate * duration;
    
    std::vector<float> samples (numberOfSamples);   // create an empty array to store all of our samples
    
    int frequency = 440;                            // the frequency of the tone to generate
    
    float phase = 0.0;                              // keep track of the phase of our oscillator
    float phaseDelta =  static_cast<float>(frequency)  / static_cast<float>(sampleRate);     // phaseDelta - how much increment the phase by for each sample
    
    
    // ************
    // OUR DSP loop
    for (int i=0; i < numberOfSamples; i++)
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
        
        // add this sample to our samples array (also halving the volume)
        samples[i] = 0.5 * sample;
        
    }
    // END OF DSP LOOP
    // ***********
    
    // write to wav file (desktop)
    writeToWav(samples, "challenge_4_3_error_correction.wav");
    
    
    std::cout << "Done!\n";
    return 0;
}
