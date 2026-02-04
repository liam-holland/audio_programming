#include <iostream>
#include <vector>
#include <cmath>

#include "MattsAudioHeader.h"

int main(int argc, char *argv[]) {

    float time{15};
    int sampleRate{44100};
    int durationInSamples{ static_cast<int>( time) * sampleRate };

    float frequency_c{400};
    float frequency_m{1000};

    float phase_delta_c{frequency_c / sampleRate};
    float phase_delta_m{frequency_m / sampleRate};

    float phase_c{0};
    float phase_m{0};

    float alpha{0};
    float gain{0.8};

    std::vector<float> c(durationInSamples);
    // std::vector<float> m(durationInSamples);


    // DSP LOOP
    for (int i = 0 ; i < durationInSamples; i++)
    {

        alpha = ( i / static_cast<float>(durationInSamples) ) * 2;

        // STEP 1:   get sine value from current phase
        float value_m = sin( phase_m * 2 * M_PI );

        float value_c = sin( phase_c * 2 * M_PI + alpha * value_m );

        c[i] = gain * value_c;

        phase_c += phase_delta_c;
        phase_m += phase_delta_m;

        // make sure the phase is in the range 0-1
        if (phase_c > 1.0)
            phase_c -= 1.0;

        if (phase_m > 1.0)
            phase_m -= 1.0;
    }

    writeToWav(c,"phase_modulation.wav");

}
