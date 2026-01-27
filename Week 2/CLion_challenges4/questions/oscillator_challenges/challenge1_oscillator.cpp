/* Challenges:
 
 1) update the code so that it plays a sine tone at 660Hz for 10 seconds
 
 2) currently the volume is 0.5. Change it to 0.1 (1 tenth of full volume)
 
 3) set up a new variable for the volume above the frequency variable. Replace the 0.1 with your volume variable.
 
 4) update the code so that after half of the total time has elapsed, the frequency drops to 330 Hz
 
 5) add a second oscillator at a different frequency from the first
 -- -- hint: you'll need to add extra variables for the new osc, including a new phase variable, phaseDelta, and frequency. Make sure these all have different names! e.g. phase2, phaseDelta2.
 -- -- hint: the final sample[i] should be the SUM of the two oscillator values - add them together.
 
*/
 
#include <iostream>
#include <vector>
#include <cmath>                // for sin and M_PI
#include "MattsAudioHeader.h"

int main(int argc, const char * argv[]) {
    
    int sampleRate = 44100;
    
    int duration = 5;                               // duration in seconds
    int durationInSamples = sampleRate * duration;  // total number of samples for the whole output
    
    std::vector<float> samples (durationInSamples);  // create a vector to store all of our samples
    
    float frequency = 440;                          // the frequency of the tone to generate
    
    float phase = 0.0;                                  // keep track of the phase of our oscillator
    float phaseDelta = frequency  / float(sampleRate);  // phaseDelta - how much increment the phase by for each sample
    
    
    // ************
    // OUR DSP loop
    for (int i=0; i<durationInSamples; i++)
    {
        
        // calculate the sine tone value from this phase --> with sin(phase * 2 * pi)
        float sample = sin( phase * 2 * M_PI );
        
        // add this sample to our samples vector (also halving the volume)
        samples[i] = 0.5 * sample;
        
        
        // increment the phase a little
        phase += phaseDelta;
        
        // make sure the phase is in the range 0-1
        if (phase > 1.0)
        {
            phase -= 1.0;
        }
        
    }
    // END OF DSP LOOP
    // ***********
    
    // write to wav file (desktop)
    writeToWav(samples, "challenge_4_2_osc.wav");
    
    
    std::cout << "Done!\n";
    return 0;
}
