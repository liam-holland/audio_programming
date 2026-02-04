#include <iostream>
#include <cmath>

float midiToFrequency(float _midiValue)
{
    float freq { 440 * powf(2.0,(_midiValue - 69.0)/12.0) };
    return freq;
}

void printFirst8Harmonics( float _frequency)
{
    for (int i = 1; i < 9; ++i)
    {
        std::cout << "Harmonic " << i << ": " << 2*_frequency * i << "Hz" << std::endl;
    }
}

int main() {

    float midiNote{};
    std::cout << "Give me a MIDI note value!" << '\n';
    std::cin >> midiNote;

    if (midiNote < 0 | midiNote > 127)
    {
        std::cout << "Please enter a value between 0 and 127 inclusive";

        return 1;
    }

    float frequency {midiToFrequency(midiNote)};
    std::cout << "Note " << midiNote << " corresponds to a frequency value of " << frequency << " Hz" << '\n' <<std::endl;

    printFirst8Harmonics( frequency);


    return 0;
}
