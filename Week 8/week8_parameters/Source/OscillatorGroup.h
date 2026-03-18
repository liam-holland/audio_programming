/*
  ==============================================================================

    OscillatorGroup.h
    Created: 2 Mar 2026 3:09:50pm
    Author:  Liam Holland

  ==============================================================================
*/

#include <vector>
#include <JuceHeader.h>

#include "Oscillator.h"

#pragma once

class OscillatorGroup
{
private:

    std::vector<Oscillator> oscGroup;

public:

    //These two functions let me use indexing
    Oscillator& operator[](size_t index)
    {
        return oscGroup[index];
    }

    const Oscillator& operator[](size_t index) const
    {
        return oscGroup[index];
    }

    // Clear the group
    void clear()
    {
        oscGroup.clear();
    }

    // Add an oscillator
    void addOscillator(Oscillator _osc)
    {
        oscGroup.emplace_back(_osc);
    }

    // Overwrite the starts of all the oscillators
    void setStartAll(float _start)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setStart(_start);
            oscGroup[o].setStartSample();

        }
    }

    // Overwrite the ends of all the oscillators
    void setDurationAll(float _duration)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setDuration(_duration);
            oscGroup[o].setLastSample();

        }
    }

    // Overwrite the envelopes of all the oscillators
    void setEnvelopeAll(float _sampleRate, float _attackTime, float _decayTime, float _sustainLevel, float _releaseTime)
    {
        for (int o = 0; o < oscGroup.size(); o++)
        {

            oscGroup[o].setEnvelope(_sampleRate, _attackTime, _decayTime, _sustainLevel, _releaseTime);

        }
    }

    // Process all of the oscillators in a group
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
