
#include "MattsAudioHeader.h"
#include <iostream>
#include <ctime>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

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
    float timeElapsed{};
    float flipTime{ 5 };
    float flipSample{ flipTime * sampleRate };
    float frequency{10};

    for (int i{ 0 }; i < numberOfSamples; ++i)
    {

        volumeEnvelope = (std::sin(2 * M_PI * frequency* i / sampleRate) + 1) /2;

        randomNumber = rand() / static_cast<float>(RAND_MAX);
        sample[i] = (randomNumber - 0.5) * volumeEnvelope * gain / 0.5;
    }

    writeToWav(sample, "audio.wav", false, "C:\\Users\\Liam Holland\\Desktop\\");

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}
