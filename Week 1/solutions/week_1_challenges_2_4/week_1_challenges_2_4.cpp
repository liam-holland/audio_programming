// week_1_challenges_2_4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

int main()
{
    // Set the variables
    float duration{1.23456};
    int sampleRate{44100};

    int durationInSamples{ static_cast<int>(std::ceil(sampleRate * duration)) };

    std::vector<float> samples(durationInSamples);

    for (int i{ 0 }; i < durationInSamples; ++i)
        samples[i] = 0;

    std::cout << "Length of array is " << samples.size();

    return 0;
}
    