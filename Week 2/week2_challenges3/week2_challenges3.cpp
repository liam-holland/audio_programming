
#include "MattsAudioHeader.h"
#include <iostream>
#include <ctime>
#include <cmath>

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
    float gain{0.8};
    float volumeEnvelope{};
    float timeElapsed{};
    float flipTime{ 5 };
    float flipSample{ flipTime * sampleRate };

    for (int i{ 0 }; i < numberOfSamples; ++i)
    {

        if (i < flipSample)
        {
            volumeEnvelope = i / static_cast<float>(flipSample);
        }
        else
        {
            volumeEnvelope = 1 - std::fmod(i / static_cast<float>(flipSample),1);
        }
   
        randomNumber = rand() / static_cast<float>(RAND_MAX);
        sample[i] = (randomNumber - 0.5) * volumeEnvelope * gain/0.5;
    }

    writeToWav(sample, "audio.wav",false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}
