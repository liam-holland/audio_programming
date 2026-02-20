#define _USE_MATH_DEFINES

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

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
        setShape(_chooseShape_);
        setPhaseDelta();
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


class oscillatorArray 
{

private:
    

public:
    
    /**
    *
    * 
    * 
    * 
    * 
    *
    */
    explicit oscillatorArray(std::vector<float> _oscVector_)
    {

    }

    // Start
    
    // Duration (s)

    // Frequency (static or modulating)

    // Wave shape

    // Amplitude (static or modulating)

    // Add or subtract or multiply

    // Range of oscillators to apply to


};


class randomEffects //Use this class to create distortion etc at random times
{
    


};


