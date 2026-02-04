#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

int main() {

    // Size of vectors
    int n{50};

    std::vector<float> volumes(n);
    std::vector<float> frequencies(n);

    // Initialise randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_volume(0, 1);
    std::uniform_real_distribution<float> dist_freq(220, 880);

    for (int i = 0; i < n; ++i)
    {

        volumes[i] = dist_volume(gen);
        frequencies[i] = dist_freq(gen);
    }

    // Show different ways of finding a min or max
    float min_volume{*std::ranges::min_element(volumes)};
    float max_volume{*std::max_element(volumes.begin(),volumes.end())};

    float min_frequency{*std::ranges::min_element(frequencies)};
    float max_frequency{*std::max_element(frequencies.begin(),frequencies.end())};


    std::cout << "The minimum frequency is: " << min_frequency << std::endl;
    std::cout << "The maximum frequency is: " << max_frequency << '\n' << std::endl;

    std::cout << "The minimum volume is: " <<min_volume << std::endl;
    std::cout << "The maximum volume is: " <<max_volume << std::endl;


    return 0;
}