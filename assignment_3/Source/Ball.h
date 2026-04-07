/*
  ==============================================================================

    Ball.h
    Created: 4 Apr 2026 3:02:45pm
    Author:  Liam Holland

  ==============================================================================
*/

#pragma once
#include <cmath>

class Ball
{
public:
        
	Ball() = default;
	~Ball() = default;

    struct BallState
    {
        float position;
        float velocity;
    };

    void setGravity(float _gravity)
    {
        gravity = _gravity;
    }

    void setLoss(float _loss)
    {
        loss = _loss;
    }

    // Call this in your processor's prepareToPlay 
    void prepare(double newSampleRate)
    {
        sampleRate = static_cast<int>(newSampleRate);
        updateTimeStep();
    }

    void updateTimeStep()
    {
        timeStep = 1.0f / sampleRate;
    }

    void updateVelocity()
    {
        xVelocity += gravity * timeStep;
    }

    void updatePosition()
    {
        xPosition += xVelocity * timeStep;
    }

    BallState processMovement()
    {
        updateVelocity();
        updatePosition();

        // Check for collision
        if (xPosition <= 0.0f)
        {
            xPosition = 0.0f;     // Snap to floor
            xVelocity *= -(1 -loss);   // Flip velocity

            // Check if I need to stop the ball
            if (std::abs(xVelocity) < 0.01f)
            {
                xVelocity = 0.0f;
            }
        }

        return { xPosition, xVelocity };
    }

private:

    float gravity{ -20.0f }; //We are saying that "up" is the positive direction
    float loss{ 0.01f };
    float xPosition{ 1.0f };
    float xVelocity{ 0.0f };
    float mass{ 1.0f };
    float time{ 0.0f };
    int sample{ 0 };
    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000 };

};
