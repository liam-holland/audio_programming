#define _USE_MATH_DEFINES

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

// Source - https://stackoverflow.com/a/26065433
// Posted by Hemant Gangwar, modified by community. See post 'Timeline' for change history
// Retrieved 2026-04-02, License - CC BY-SA 4.0

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


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

    Oscillator() : Oscillator(48000, 440.0f, 1.0f, "sin") {}

    Oscillator(int _sampleRate_, float _frequency_, float _amplitude_, std::string _chooseShape_)
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
        
        setPhaseDelta();
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
        setPhaseDelta();
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

    auto process()
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

        sampleValue = sin(2*M_PI*phase);

        return amplitude * sampleValue;
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

        sampleValue = 4*(fabs(phase - 0.5)-1.0f);

        return amplitude*sampleValue;
    }


private:

    float frequency{440.0f};
    int sampleRate{48000};
    float phase{0.0f};
    float phaseDelta{0.0f};
    float amplitude{1.0f};
    float sampleValue{0};
    std::string shape{"sin"};
};


