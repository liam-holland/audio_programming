
#include "MattsAudioHeader.h"
#include <iostream>
#include <ctime>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

/*

// Pulse white noise
int main() {

    // Set the variables
    float duration{ 10 };
    int sampleRate{ 44100 };

    int numberOfSamples{ static_cast<int>(std::ceil(sampleRate * duration)) };

    // set up a vector to store the sample data
    std::vector<float> sample(numberOfSamples);

    // set up the for loop below and use it to populate the samples vector
    // Get a different random number each time the program runs
    srand(time(0));
    float randomNumber{};
    float gain{ 0.8 };
    float volumeEnvelope{};
    float onOffFrequency{ 0.25 };
    int onOffSample{ static_cast<int>(std::ceil(sampleRate * onOffFrequency )) };
    float onOffInitial{ 0 };

    for (int i{ 0 }; i < numberOfSamples; ++i)
    {   
        
        // It will change state on the first sample since 0 mod anything is 0
        if (i % onOffSample == 0 )
        {
            onOffInitial = 1 - onOffInitial;
            volumeEnvelope = onOffInitial;
        }

        randomNumber = rand() / static_cast<float>(RAND_MAX);
        sample[i] = (randomNumber - 0.5) * volumeEnvelope * gain / 0.5;
    }

    writeToWav(sample, "audio.wav", false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}
*/


/*
// Pulse white noise where the gaps between the pulses increases linearly over time until 5 seconds, then decreases
int main()
{

    // Set the variables
    float duration{ 10 };
    int sampleRate{ 44100 };

    int numberOfSamples{ static_cast<int>(std::ceil(sampleRate * duration)) };

    // set up a vector to store the sample data
    std::vector<float> sample(numberOfSamples);

    // set up the for loop below and use it to populate the samples vector
    // Get a different random number each time the program runs
    srand(time(0));
    float randomNumber{};

    // 'gain' variables
    float gain{ 0.8 };
    float volumeEnvelope{};

    // Set the flip time to be half of the total duration and find which sample this lies on
    float flipTime{ duration/2 };
    int flipSample{ static_cast<int>(std::ceil( flipTime * sampleRate )) };

    // Gate will turn the sample on and off, we need to also countdown until this occurs
    int gate{ 0 };
    int samplesUntilToggle{ 0 };

    // We are going to set a min and max gap length
    int minGapSamples{ static_cast<int>(std::ceil(sampleRate * 0.01)) };  // 10 ms
    int maxGapSamples{ static_cast<int>(std::ceil(sampleRate * 0.5 )) };   // 500 ms


    for (int i{ 0 }; i < numberOfSamples; ++i)
    {

        // Time in seconds
        float time{};
        time  = static_cast<float>(i) / sampleRate;

        // 0 > 1 > 0 ramp over 10 seconds
        float ramp{};

        if (time < flipTime) {
            ramp = time / flipTime;                  // increasing
        }
        else {
            ramp = 1.0 - (time - flipTime) / flipTime; // decreasing
        }

        // Convert ramp into a gap length (in samples)
        int gapSamples{};
        gapSamples = static_cast<int>(
            minGapSamples + ramp * (maxGapSamples - minGapSamples)
            );

        // Countdown to toggle the gate
        // This will be true intially, so sampleUntilToggle will be a positve value
        if (samplesUntilToggle <= 0) {
            gate = 1 - gate;               // flip 0 > 1 or 1 > 0
            samplesUntilToggle = gapSamples;
        }


        // Count down, by subtracting one
        samplesUntilToggle--;

        randomNumber = rand() / static_cast<float>(RAND_MAX);
        sample[i] = (randomNumber - 0.5) * gate * gain / 0.5;
    }

    writeToWav(sample, "audio.wav", false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}


*/





// Implementing a low pass filter y_n = ax_n = (1-a)x_n-1

int main() {

    // Set the variables
    float duration{ 10 };
    int sampleRate{ 44100 };
    int delayOrder{ 1 };
    float a{ 0.5 };

    int numberOfSamples{ static_cast<int>(std::ceil(sampleRate * duration) + delayOrder) };

    // set up a vector to store the sample data
    std::vector<float> sample(numberOfSamples);

    // set up a vector to store the buffer data
    std::vector<float> buffer(delayOrder + 1);
    buffer[0,1] = 0,0;

    // set up the for loop below and use it to populate the samples vector
    // Get a different random number each time the program runs
    srand(time(0));
    float randomNumber{};
    float gain{ 0.8 };


    for (int i{ 1 }; i < numberOfSamples; ++i)
    {

        // Update last element of buffer
        buffer[1] = rand() / static_cast<float>(RAND_MAX);

        // Create output based on inputs
        float y_n{};
        y_n = a*buffer[1] + (1-a)*buffer[0];

        sample[i] = (y_n - 0.5) * gain / 0.5;

        // Set the 'previous' value
        buffer[0] = buffer[1];

    }

    writeToWav(sample, "audio_lpf.wav", false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}





/*


// Normal white noise

int main() {

    // Set the variables
    float duration{ 10 };
    int sampleRate{ 44100 };

    int numberOfSamples{ static_cast<int>(std::ceil(sampleRate * duration)) };

    // set up a vector to store the sample data
    std::vector<float> sample(numberOfSamples);

    // set up the for loop below and use it to populate the samples vector
    // Get a different random number each time the program runs
    srand(time(0));
    float randomNumber{};
    float gain{ 0.8 };


    for (int i{ 1 }; i < numberOfSamples; ++i)
    {

        // Update last element of buffer
        randomNumber = rand() / static_cast<float>(RAND_MAX);

        sample[i] = (randomNumber - 0.5) * gain / 0.5;


    }

    writeToWav(sample, "audio.wav", false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}
*/