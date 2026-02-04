#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

#include "MattsAudioHeader.h"

class Oscillator
{
public:

    /**
     *
     * @param _sampleRate_ Set the sample rate of the oscillator
     * @param _frequency_ Set the frequency of the oscillator
     * @param _amplitude_ Set the amplitude of the oscillator (between 0 - 1)
     * @param _chooseShape_ Choose the shape of the oscillator
     */
    explicit Oscillator(int _sampleRate_, float _frequency_, float _amplitude_, std::string _chooseShape_)
    {
        setSampleRate(_sampleRate_);
        setFrequency(_frequency_);
        setAmplitude(_amplitude_);
        setPhaseDelta();
        setShape(_chooseShape_);
    }

    //setSampleRate
    void setSampleRate( int _sampleRate )
    {
        sampleRate = _sampleRate;
    }

    void setPhaseDelta()
    {
        phaseDelta = frequency / sampleRate;
    }

    //getSampleRate
    float getSampleRate()
    {
        return sampleRate;
    }

    // setFrequency
    void setFrequency( float _frequency )
    {
        frequency = _frequency;
    }

    //setAmplitude
    void setAmplitude( float _amplitude )
    {
        amplitude = _amplitude;
    }

    void setShape( std::string _shape )
    {
        shape = _shape;
    }

    auto make()
    {
        if (shape == "sin")
        {
            return processSin();
        }
        else if (shape == "square")
        {
            return processSquare();
        }
        else if (shape == "triangle")
        {
            return processTriangle();
        }
    }

    void phaseIncrement()
    {
        phase += phaseDelta;

        // make sure the phase is in the range 0-1
        if (phase > 1.0)
            phase -= 1.0;
    }

    float processSin()
    {
        phaseIncrement();

        sampleValue = amplitude*sin(2*M_PI*phase);

        return sampleValue;
    }

    float processSquare()
    {
        phaseIncrement();

        if (phase > 0.5) {
            sampleValue = 1;
        }
        else {
            sampleValue = -1;
        }

        return amplitude*sampleValue;
    }

    float processTriangle() {

        phaseIncrement();

        sampleValue = 2*(fabs(phase - 0.5)-0.25);

        return amplitude*sampleValue;
    }


private:
    float frequency{};
    int sampleRate{};
    float phase{0};
    float phaseDelta{};
    float amplitude{};
    float sampleValue{};
    std::string shape{};

};

class envelopeShape {
public:

    /**
     * @param _sampleRate_
     * @param _attack_
     * @param _sustain_
     * @param _decay_
     */
    explicit envelopeShape(int _sampleRate_, float _attack_, float _sustain_, float _decay_)
    {
        setSampleRate(_sampleRate_);
        setAttack(_attack_);
        setSustain(_sustain_);
        setDecay(_decay_);

    }

    //setSampleRate
    void setSampleRate( int _sampleRate )
    {
        sampleRate = _sampleRate;
    }


    void setAttack( float _attack)
    {
        attack = _attack;
        attackSamples = static_cast<int>(attack * sampleRate);
    }

    void setSustain( float _sustain)
    {
        sustain = _sustain;
        sustainSamples = static_cast<int>(sustain * sampleRate);

    }

    void setDecay( float _decay)
    {
        decay = _decay;
        decaySamples = static_cast<int>(decay * sampleRate);
    }


    float processEnvelope()
    {
        int totalSamples = attackSamples + sustainSamples + decaySamples;
        envelope.resize(totalSamples);


        int index = 0;

        // Attack: ramp 0 → 1
        for (; index < attackSamples; ++index)
            envelope[index] = static_cast<float>(index) / attackSamples;

        // Sustain: hold at 1
        for (; index < attackSamples + sustainSamples; ++index)
            envelope[index] = 1.0f;

        // Decay: ramp 1 → 0
        for (; index < totalSamples; ++index)
            envelope[index] = 1.0f - static_cast<float>(index - attackSamples - sustainSamples) / decaySamples;

        // reset sample counter
        sampleNumber = 0;


    }


private:
    float attack{};
    int attackSamples{};
    float sustain{};
    int sustainSamples{};
    float decay{};
    int decaySamples{};
    int sampleRate{};
    float sampleValue{};

    std::vector<float> envelope{};
    int sampleNumber{};

};

int main(int argc, char *argv[]) {

    std::vector<Oscillator> oscillators;

    int globalSampleRate{44100};

    oscillators.emplace_back(   globalSampleRate, 70,    0.2f, "sin");   // osc1
    oscillators.emplace_back(   globalSampleRate, 100,   0.3f, "square");   // osc2
    oscillators.emplace_back(   globalSampleRate, 10000, 0.1f, "triangle");   // osc3
    oscillators.emplace_back(   globalSampleRate, 1000,  0.1f, "square");   // osc4
    oscillators.emplace_back(   globalSampleRate, 1600,  0.1f, "triangle");   // osc5
    oscillators.emplace_back(   globalSampleRate, 60,    0.1f, "square");   // osc6
    oscillators.emplace_back(   globalSampleRate, 90,    0.1f, "triangle");   // osc7
    oscillators.emplace_back(   globalSampleRate, 112,   0.1f, "sin");   // osc8
    oscillators.emplace_back(   globalSampleRate, 200,   0.1f, "triangle");   // osc9


    float duration{10.0};
    int numberOfSamples{ static_cast<int>( globalSampleRate * duration) };
    float gain{0.8};

    std::vector<float> samples(numberOfSamples);

    for (int i = 0; i < numberOfSamples; ++i) {

        for (auto & oscillator : oscillators)
        {
            samples[i] = gain * ( samples[i] + oscillator.make());
        }

    }

    // write to wav file (desktop)
    writeToWav(samples, "oscillator_week_3.wav");


    return 0;
}
