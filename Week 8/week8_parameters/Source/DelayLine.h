/*
  ==============================================================================

    DelayLine.h
    Created: 10 Mar 2026 12:36:53pm
    Author:  Liam Holland

  ==============================================================================
*/

#include <vector>
#include <cmath>

#pragma once

class DelayLine
{

public:

    DelayLine() = default;

    void prepare(float _maxDelay, int _sampleRate) {
        maxDelay = _maxDelay;
        bufferSamples = static_cast<int>(maxDelay * _sampleRate);
        buffer.assign(bufferSamples, 0.0f);
        
        readHead = 0;
        writeHead = 0;
    }

    void setSampleRate(float _sampleRate)
    {
        sampleRate = _sampleRate;
    }

    void setMaxDelay(float _maxDelayTime)
    {
        maxDelay = _maxDelayTime;
        bufferSamples = static_cast<int> (maxDelay * sampleRate);

        // Change buffer size and set every value to 0.0
        buffer.assign(bufferSamples, 0.0f);

    }


    void setDelay(float _delayTime)
    {
        delayTime = std::clamp(_delayTime,0.0f,maxDelay);
        float fracDelaySamples = _delayTime * sampleRate;
        delaySamples = static_cast<int> (floor(fracDelaySamples));
       
        readHead = (bufferSamples - 1) - fracDelaySamples;
    }


    void setWriteHeadValue(float _writeValue)
    {
        buffer[writeHead] = _writeValue;
    }


    float getReadHead()
    {
        int buffer_1 = floor(readHead);
        int buffer1 = buffer_1 + 1;
        if (buffer1 >= bufferSamples) buffer1 = 0;

        // Do linear interpolation
        float buffer1Value = buffer[buffer1];
        float buffer_1Value = buffer[buffer_1];
        float frac = readHead - buffer_1;
               
        return frac*(buffer1Value - buffer_1Value) + buffer1Value;
    }

    float processDelay( float _sampleValue) {

        // If a buffer has not been allocated, pass through clean audio
        if (bufferSamples <= 0) return _sampleValue;

        setWriteHeadValue(_sampleValue);
        float delayedSample =  getReadHead();

        writeHead++;
        if ( writeHead > (bufferSamples - 1) )
        {
            writeHead = 0;
        }

        readHead++;
        if (readHead > (bufferSamples - 1.0f))
        {
            readHead = 0.0f;
        }

        return delayedSample;

    }


private:
    int sampleRate{ 48000 };
    std::vector<float> buffer;
    float maxDelay{};
    int delaySamples{};
    float readHead{};
    int writeHead{0};
    float delayTime{0.05f};
    int bufferSamples{};
};
