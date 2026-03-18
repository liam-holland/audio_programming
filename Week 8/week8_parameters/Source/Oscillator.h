#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <vector>
#include <JuceHeader.h>

#include "Envelope.h"


#pragma once


class Oscillator
{
public:

    /**
     * @param _sampleRate_ Set the sample rate of the oscillator
     * @param _start_ Set the start time of the oscillator
     * @param _duration_ The length of time in seconds that the oscillator is on for
     * @param _frequency_ Set the frequency of the oscillator
     * @param _chooseShape_ Choose the shape of the oscillator
     * @param _amplitude_ Set the amplitude of the oscillator (between 0 - 1)
     */

     // Set the default behaviour
    Oscillator() : sampleRate(48000),
        start(0.01f),
        duration(0.1f),
        frequency(0.8f),
        shape("sin"),
        amplitude(0.5f)

    {
        setPhaseDelta();
        setStartSample();
        setLastSample();
    }

    explicit Oscillator
    (   
          int _sampleRate_
        , float _start_
        , float _duration_
        , float _frequency_
        , std::string _chooseShape_
        , float _amplitude_
    )

    {
        setSampleRate(_sampleRate_);

        setStart(_start_);
        setDuration(_duration_);
        setFrequency(_frequency_);
        setShape(_chooseShape_);
        setAmplitude(_amplitude_);
        
        setPhaseDelta();
        setStartSample();
        setLastSample();
        
    }


    // Sample rate ----------------------------

    
    void setSampleRate(int _sampleRate)
    {
        sampleRate = _sampleRate;
    }

    
    float getSampleRate()
    {
        return sampleRate;
    }


    // Start -------------------------------

    void setStart(float _start)
    {
        start = _start;
    }

    float getStart()
    {
        return start;
    }

    // Duration -----------------------------

    
    void setDuration(float _duration)
    {
        duration = _duration;
    }

    
    float getDuration()
    {
        return duration;
    }


    // Frequency -----------------------------


    void setFrequency(float _frequency)
    {
        frequency = _frequency;
        setPhaseDelta();
    }

    float getFrequency()
    {
        return frequency;
    }


    //Amplitude ------------------------
    void setAmplitude(float _amplitude)
    {
        amplitude = _amplitude;
    }

    float getAmplitude()
    {
        return amplitude;
    }

    // Shape -------------------------

    void setShape(std::string _shape)
    {
        shape = _shape;
    }

    std::string getShape()
    {
        return shape;
    }


    // We now move to variables that are cacluated from our initialiation

    // First sample
    void setStartSample()
    {
        firstSample = static_cast<int>(sampleRate * start);
    }

    int getStartSample()
    {
        return firstSample;
    }

    //Last sample
    void setLastSample()
    {
        lastSample = static_cast<int> (sampleRate * (start + duration) );
    }

    int getLastSample()
    {
        return lastSample;
    }


    // Phase and phase delta

    void setPhaseDelta()
    {
        phaseDelta = frequency / sampleRate;
    }

    void setPhaseShift( float _phaseShift)
    {
        phaseShift = _phaseShift;
    }

    void phaseIncrement()
    {
        phase += phaseDelta;

        // make sure the phase is in the range 0-1
        if (phase > 1.0)
            phase -= 1.0;
    }

    

    // Process the oscillators


    float process()
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
        else if (shape == "saw")
        {
            return processSaw();
        }
        else if (shape == "white")
        {
            return processWhite();
        }
        else if (shape == "pink")
        {
            return processPink();
        }

        return 0.0f;
    }

    void setEnvelope(int _sampleRate_, float _attack_, float _decay_, float _sustain_, float _release_)
    {
        envelope = Envelope(_sampleRate_, _attack_, _decay_, _sustain_, _release_);
        envelope.setAttackStart(getStartSample());
        envelope.setSustainEnd(getLastSample());

    }

    void setCurrentSample()
    {
        currentSample ++;
    }

    float make()
    {
        setCurrentSample();

        env = envelope.processEnvelope(currentSample);
                
        if (env > 0.0f)
            output = process() * env;
        else
            output = 0.0f;

        return output;

    }


    float processSin()
    {
        
        sampleValue = amplitude * sin(2 * 3.14159265358979323846 * (phase + phaseShift));

        phaseIncrement();

        return sampleValue;
    }

    float processSquare()
    {
        

        if (phase > 0.5) {
            sampleValue = 1;
        }
        else {
            sampleValue = -1;
        }

        phaseIncrement();

        return amplitude*sampleValue;
    }

    float processTriangle() {


        sampleValue = 4*(fabs( (phase + phaseShift)  - 0.5)-0.25);

        phaseIncrement();

        return amplitude*sampleValue;
    }

    float processSaw()
    {
 
        sampleValue = (phase + phaseShift);
        phaseIncrement();
        return amplitude * sampleValue;
        
    }

    float processWhite()
    {
        sampleValue = ((random.nextFloat() * 2.0f) - 1);
        return amplitude * sampleValue;
    }

    float processPink()
    {
        float white{ processWhite() };

        //Filter to make pink noise from white(updated March 2000)
        //    ------------------------------------

        //This is an approximation to a - 10dB / decade filter using a weighted sum
        //of first order filters.It is accurate to within + / -0.05dB above 9.2Hz
        //(44100Hz sampling rate).Unity gain is at Nyquist, but can be adjusted
        //by scaling the numbers at the end of each line.

        //If 'white' consists of uniform random numbers, such as those generated
        //by the rand() function, 'pink' will have an almost gaussian level
        //distribution.

        // Written by paul.kellett@maxim.abel.co.uk

        b0 = 0.99886 * b0 + white * 0.0555179;
        b1 = 0.99332 * b1 + white * 0.0750759;
        b2 = 0.96900 * b2 + white * 0.1538520;
        b3 = 0.86650 * b3 + white * 0.3104856;
        b4 = 0.55000 * b4 + white * 0.5329522;
        b5 = -0.7616 * b5 - white * 0.0168980;
        b6 = white * 0.115926;
        float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;

        return pink;
        
    }


private:


    float start{ 0.0f };
    float duration{ 180.0f };
    float frequency{ 440.0f };
    std::string shape{"sin"};
    float amplitude{ 0.5f };


    int sampleRate{ 48000 };
    int firstSample{0};
    int lastSample{0};
    float phase{0};
    float phaseDelta{0};
    float sampleValue{0};

    float phaseShift{ 0.0f };

    bool isActive{ false };
    juce::Random random;

    float b0{ 0 };
    float b1{ 0 };
    float b2{ 0 };
    float b3{ 0 };
    float b4{ 0 };
    float b5{ 0 };
    float b6{ 0 };

    Envelope envelope;

    float env{ 0 };

    int currentSample{ 0 };

    float output{ 0 };

};




