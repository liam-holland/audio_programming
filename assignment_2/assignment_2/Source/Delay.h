/*
  ==============================================================================

    Delay.h
    Created: 28 Feb 2026 1:42:54pm
    Author:  Liam Holland

  ==============================================================================
*/

#pragma once

class delayEffect
{
public:
    /**
    *
    *
    *
    *
    */
    explicit delayEffect(int _sampleRate_, float _start_, float _duration_, float _delayTime_, float _wet_)
    {
        setSampleRate(_sampleRate_);
        setStart(_start_);
        setDuration(_duration_);
        setDelayTime(_delayTime_);
        setWet(_wet_);

        setDelaySamples();

    }

    void setSampleRate(int _sampleRate)
    {
        sampleRate = _sampleRate;
    }

    void setStart(float _start)
    {
        start = _start;
    }

    void setDuration(float _duration)
    {
        duration = _duration;
    }

    void setDelayTime(float _delayTime)
    {
        delayTime = _delayTime;
    }

    void setWet(float _wet)
    {
        wet = _wet;
    }

    void setDelaySamples()
    {
        delaySamples = static_cast<int>(delayTime * sampleRate);
    }

    void sampleTimes()
    {

    }

    void setDelayedSample()
    {
        startSample = static_cast<int>(sampleRate * start);
        endSample = static_cast<int>(sampleRate * (start + duration));
    }

    float processDelay(float _sampleX)
    {

        if (sampleCount >= startSample && sampleCount < endSample)
            sampleY = sampleX + wet * sampleXN;

        sampleCount++;

        return sampleY;
    }


private:

    int sampleRate;
    float start{};
    float duration{};
    float delayTime{};
    float wet{};

    int startSample{};
    int endSample{};

    int delaySamples{};
    float sampleX{};
    float sampleXN{ 0 };
    int sampleCount{ 0 };

    float sampleY{};


};
