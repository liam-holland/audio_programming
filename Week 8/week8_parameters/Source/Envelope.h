/*
  ==============================================================================

    Envelope.h
    Created: 2 Mar 2026 3:09:21pm
    Author:  Liam Holland

  ==============================================================================
*/


#pragma once


/**
    This class alows the user to set an envelope on an oscillator. Without this class there will be a noticable audio
    pop as a oscillator starts and stops
*/
class Envelope
{

public:

    // Set the default behaviour (sample rate = 48000, attack = 0.01s, decay = 0.1s, sustain = 80%, release = 0.2s)
    Envelope() : sampleRate(48000),
        attack(0.01f),
        decay(0.1f),
        sustain(0.8f),
        release(0.2f)
    {
        setEnvelopeParameters();
    }

    // Set the envelope of an oscillator
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
        attackEnd = attackStart + static_cast<int>(sampleRate * attack);

        decayStart = attackEnd + 1;
        decayEnd = decayStart + static_cast<int>(sampleRate * decay);

        sustainStart = decayEnd + 1;

        releaseStart = sustainEnd + 1;
        releaseEnd = releaseStart + static_cast<int>(sampleRate * release);

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
        // Decay Phase (1.0 to sutain level)
        else if (_sample >= decayStart && _sample < decayEnd) {
            float progress = static_cast<float>(_sample - decayStart) / (decayEnd - decayStart);
            // 1.0 down to sustain
            envAmplitude = 1.0f - ((1 - sustain) * progress);
        }
        // Sustain Phase (Hold at sustain level)
        else if (_sample >= sustainStart && _sample < sustainEnd) {
            envAmplitude = sustain;
        }
        // Release Phase (sustain level to 0.0)
        else if (_sample >= releaseStart && _sample < releaseEnd) {
            float progress = static_cast<float>(_sample - releaseStart) / (releaseEnd - releaseStart);
            // Sustain level down to 0.0
            envAmplitude = sustain - (sustain * progress);
        }
        // Note is fully finished
        else {
            envAmplitude = 0.0f;
        }

        return envAmplitude;
    }

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

    float envAmplitude{ 0.0 };


};
