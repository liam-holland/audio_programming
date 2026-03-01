#define _USE_MATH_DEFINES

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <JuceHeader.h>

#pragma once


class Envelope
{
private:
    float sampleRate{};
    float attack{};
    float decay{};
    float sustain{};
    float release{};

    float start{};
    float duration{};

    int attackStart{ 0 };
    int attackEnd{ 0 };

    int decayStart{ 0 };
    int decayEnd{ 0 };

    int sustainStart{ 0 };
    int sustainEnd{ 0 };

    int releaseStart{ 0 };
    int releaseEnd{ 0 };

    //0-1, 1 - 0.8 , 0.8 - 0.8, 0.8 - 0
    float attackDelta{ 0.0 };
    float decayDelta{ 0.0 };
    float sustainDelta{ 0.0 };
    float releaseDelta{ 0.0 };

    float envAmplitude{ 0.0 };
    float delta{ 0.0 };


public:

    // Set the default behaviour
    Envelope() : sampleRate(48000),
        attack(0.01f),
        decay(0.1f),
        sustain(0.8f),
        release(0.2f)
    { 
        setEnvelopeParameters();
    }


     /**
     * @param _sampleRate_ Set the sample rate of the evnvelope
     * @param _attack_ Set the attack time of the envelope
     * @param _decay_ Set the decay time of the envelope
     * @param _sustainLevel_ Set the sustain level (0.0 - 1.0 float)
     * @param _release_ Set the release time after the oscillator ends
     */
    explicit Envelope(int _sampleRate_, float _attack_, float _decay_, float _sustainLevel_, float _release_)
    {
        setSampleRate(_sampleRate_);
        setAttack(_attack_);
        setDecay(_decay_);
        setSustain(_sustainLevel_);
        setRelease(_release_);

        setEnvelopeParameters();
    }

    void setSampleRate(int _sampleRate)
    {
        sampleRate = _sampleRate;
        setEnvelopeParameters();
    }

    void setAttack(float _attack)
    {
        attack = _attack;
        setEnvelopeParameters();
    }

    void setDecay(float _decay)
    {
        decay = _decay;
        setEnvelopeParameters();
    }

    void setSustain(float _sustain)
    {
        sustain = _sustain;
        setEnvelopeParameters();
    }

    void setRelease(float _release)
    {
        release = _release;
        setEnvelopeParameters();
    }


    void setAttackStart(int _attackStart)
    {
        attackStart = _attackStart;
        setEnvelopeParameters();
    }

    void setSustainEnd(int _sustainEnd)
    {
        sustainEnd = _sustainEnd;
        setEnvelopeParameters();
    }


    void setEnvelopeParameters()
    {
        /*attackStart =  0 ;*/
        attackEnd = attackStart + static_cast<int>(sampleRate * attack);

        decayStart = attackEnd + 1;
        decayEnd = decayStart + static_cast<int>(sampleRate * decay);

        sustainStart = decayEnd + 1;
        //sustainEnd = sustainStart + static_cast<int>(sampleRate * (start + duration)) ;

        releaseStart = sustainEnd + 1;
        releaseEnd = releaseStart + static_cast<int>(sampleRate * release);

        //0-1, 1 - 0.8 , 0.8 - 0.8, 0.8 - 0
        attackDelta = 1.0 / (attackEnd - attackStart);
        decayDelta = - (1.0 - sustain) / (decayEnd - decayStart);
        sustainDelta = 0.0;
        releaseDelta = -0.8 / (releaseEnd - releaseStart);
    }

    int getReleaseEnd()
    {
        return releaseEnd;
    }

    float processEnvelope(int _sample)
    {
        // Attack Phase (0.0 to 1.0)
        if (_sample >= attackStart && _sample < attackEnd) {
            float progress = static_cast<float>(_sample - attackStart) / (attackEnd - attackStart);
            envAmplitude = progress; // Directly interpolate from 0 to 1
        }
        // Decay Phase (1.0 to 0.8)
        else if (_sample >= decayStart && _sample < decayEnd) {
            float progress = static_cast<float>(_sample - decayStart) / (decayEnd - decayStart);
            // 1.0 down to 0.8
            envAmplitude = 1.0f - (( 1 - sustain) * progress);
        }
        // Sustain Phase (Hold at 0.8)
        else if (_sample >= sustainStart && _sample < sustainEnd) {
            envAmplitude = 0.8f;
        }
        // Release Phase (0.8 to 0.0)
        else if (_sample >= releaseStart && _sample < releaseEnd) {
            float progress = static_cast<float>(_sample - releaseStart) / (releaseEnd - releaseStart);
            // Lerp from 0.8 down to 0.0
            envAmplitude = 0.8f - (sustain * progress);
        }
        // Note is fully finished
        else {
            envAmplitude = 0.0f;
        }

        return envAmplitude;
    }

};




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

    float processSaw()
    {
        phaseIncrement();

        sampleValue = phase;
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
    int oscID{};
    float start{};
    float duration{};
    float frequency{};
    std::string shape{};
    float amplitude{};


    int sampleRate{};
    int firstSample{};
    int lastSample{};
    float phase{0};
    float phaseDelta{};
    float sampleValue{};

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


class OscillatorGroup
{
private:

    std::vector<Oscillator> oscGroup;

public:

    //Return the oscillator index
    Oscillator& operator[](size_t index)
    {
        return oscGroup[index];
    }

    const Oscillator& operator[](size_t index) const
    {
        return oscGroup[index];
    }

    void clear()
    {
        oscGroup.clear();
    }

    void addOscillator(Oscillator _osc)
    {
        oscGroup.emplace_back(_osc);
    }

    void setStartAll(float _start)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setStart(_start);
            oscGroup[o].setStartSample();

        }
    }

    void setDurationAll(float _duration)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setDuration(_duration);
            oscGroup[o].setLastSample();

        }
    }

    void setEnvelopeAll(float _sampleRate, float _attackTime, float _decayTime, float _sustainLevel, float _releaseTime)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setEnvelope( _sampleRate, _attackTime , _decayTime , _sustainLevel , _releaseTime);

        }
    }


    float processOscList()
    {
        float mix{ 0.0f };

        for (int o = 0; o < oscGroup.size(); o++)
        {

            mix += oscGroup[o].make();

        }

        mix = 0.5f * (mix / (oscGroup.size()));

        return mix;
    }

};


