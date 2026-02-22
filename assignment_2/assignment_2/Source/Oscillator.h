#define _USE_MATH_DEFINES

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

#pragma once


class Oscillator
{
public:

    /**
     * @param _oscID_ Set the ID of the oscillator (must be intergers)  
     * @param _sampleRate_ Set the sample rate of the oscillator
     * @param _start_ Set the start time of the oscillator
     * @param _duration_ The length of time in seconds that the oscillator is on for
     * @param _frequency_ Set the frequency of the oscillator
     * @param _chooseShape_ Choose the shape of the oscillator
     * @param _amplitude_ Set the amplitude of the oscillator (between 0 - 1)
     * @param _addSubtractMultiply Choose how the oscillator effects other oscillators
     * @param _oscRange_ Which oscillators to apply it to
     */

    explicit Oscillator
    (   
          int _oscID_
        , int _sampleRate_
        , float _start_
        , float _duration_
        , float _frequency_
        , std::string _chooseShape_
        , float _amplitude_
        , std::string _addSubtractMultiply_
        , std::vector<int> _oscRange_

    )

    {

        setSampleRate(_sampleRate_);

        setOscID(_oscID_);
        setStart(_start_);
        setDuration(_duration_);
        setFrequency(_frequency_);
        setShape(_chooseShape_);
        setAmplitude(_amplitude_);
        setAddSubtractMultiply(_addSubtractMultiply_);
        setOscRange(_oscRange_);
        
        setPhaseDelta();
        setStartSample();
        setLastSample();
        
    }

    // Osc ID

    void setOscID(int _oscID)
    {
        oscID = _oscID;
    }


    float getOscID()
    {
        return oscID;
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

    // Add, subtract, multiply --------------------------------

    void setAddSubtractMultiply(std::string _addSubtractMultiply)
    {
        addSubtractMultiply = _addSubtractMultiply;
    }

    std::string getAddSubtractMultiply()
    {
        return addSubtractMultiply;
    }

    // Osc range ----------------------------------

    void setOscRange(std::vector<int> _oscRange)
    {
        oscRange = _oscRange;
    }

    std::vector<int> getOscRange()
    {
        return oscRange;
    }

    // We now move to variables that car cacluated from our initialiation

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

    //Active or not

    void setActive( bool _isActive )
    {
        isActive = _isActive;
    }

    bool getActive()
    {
        return isActive;
    }



    // Phase and phase delta

    void setPhaseDelta()
    {
        phaseDelta = frequency / sampleRate;
    }


    void phaseIncrement()
    {
        phase += phaseDelta;

        // make sure the phase is in the range 0-1
        if (phase > 1.0)
            phase -= 1.0;
    }

    // Process the oscillators


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


    float processSin()
    {
        phaseIncrement();

        sampleValue = amplitude * sin(2 * 3.14159265358979323846 * phase);

        //sampleValue = amplitude*sin(2*M_PI*phase);

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
    int oscID{};
    float start{};
    float duration{};
    float frequency{};
    std::string shape{};
    float amplitude{};
    std::string addSubtractMultiply{};
    std::vector<int> oscRange{};

    int sampleRate{};
    int firstSample{};
    int lastSample{};
    float phase{0};
    float phaseDelta{};
    float sampleValue{};

    bool isActive{ false };
    

};


// I think JUCE already has a delay!!!

//class delayEffect
//{
//public:
//    /**
//    *
//    * 
//    * 
//    * 
//    */
//    explicit delayEffect(int _sampleRate_, float _start_, float _duration_, float _delayTime_, float _wet_)
//    {
//        setSampleRate(_sampleRate_);
//        setStart(_start_);
//        setDuration(_duration_);
//        setDelayTime(_delayTime_);
//        setWet(_wet_);
//
//        setDelaySamples();
//
//    }
//
//    void setSampleRate(int _sampleRate)
//    {
//        sampleRate = _sampleRate;
//    }
//
//    void setStart(float _start)
//    {
//        start = _start;
//    }
//
//    void setDuration(float _duration)
//    {
//        duration = _duration;
//    }
//
//    void setDelayTime(float _delayTime)
//    {
//        delayTime = _delayTime;
//    }
//
//    void setWet(float _wet)
//    {
//        wet = _wet;
//    }
//
//    void setDelaySamples()
//    {
//        delaySamples = static_cast<int>(delayTime * sampleRate);
//    }
//
//    void sampleTimes()
//    {
//
//    }
//
//    void setDelayedSample()
//    {
//        startSample = static_cast<int>(sampleRate * start);
//        endSample = static_cast<int>(sampleRate * (start + duration));
//    }
//
//    float processDelay(float sampleX)
//    {
//
//        if (sampleCount >= startSample && sampleCount < endSample)
//        sampleY = sampleX + wet * sampleXN;
//
//       
//        sample
//
//        sampleCount++;
//    }
//
//
//private:
//    
//    int sampleRate;
//    float start{};
//    float duration{};
//    float delayTime{};
//    float wet{};
//
//    int startSample{};
//    int endSample{};
//
//    int delaySamples{};
//    float sampleX{};
//    float sampleXN{0};
//    int sampleCount{0};
//
//    float sampleY{};
//
//
//};



