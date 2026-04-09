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
        float xPosition;
        float xVelocity;
        float yPosition;
        float yVelocity;
        float xMaxVelocity;
        float yMaxVelocity;
        bool triggerX;
        bool triggerY;
    };

    void setAcceleration(float _gravity , float _push)
    {
        push = _push;
        gravity = _gravity;
        
    }

    void setStartVelocity(float _StartVelocityX , float _StartVelocityY)
    {
        xVelocity = _StartVelocityX;
        yVelocity = _StartVelocityY;
    }

    void setStartPosition(float _StartPositionX, float _StartPositionY)
    {
        xPosition = _StartPositionX;
        yPosition = _StartPositionY;
    }

    void setLoss(float _lossX, float _lossY)
    {
        lossX = _lossX;
        lossY = _lossY;
    }

    void solveMaxVelocity()
    {
        // The maximum possible distance is 1.0 (from 0.0 to 1.0 or vice versa)
        float maxDist = 1.0f;

        // Use absolute values to ensure I never sqrt a negative number
        maxVelocityX = std::sqrt( std::pow(xVelocity, 2) + 2 * std::abs(push) * maxDist);
        maxVelocityY = std::sqrt (std::pow(yVelocity, 2) + 2 * std::abs(gravity) * maxDist);
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
        // Inside updateVelocity()
        if (std::abs(yPosition) > 0.001f || std::abs(yVelocity) > 0.01f) {
            yVelocity += (gravity * timeStep);
        }

        if (std::abs(xPosition) > 0.001f || std::abs(xVelocity) > 0.01f) {
            xVelocity += (push * timeStep);
        }

    }


    void updatePosition()
    {
        float shiftX = (xVelocity * timeStep);
        float shiftY = (yVelocity * timeStep);
        
        xPosition += shiftX;
        yPosition += shiftY;
    }

    BallState processMovement()
    {
        triggerY = false;
        triggerX = false;

        updateVelocity();
        updatePosition();

        // --- COLLISION LOGIC ---

        // Check for Floor Collision (0.0)
        if (yPosition <= 0.0f)
        {
            yPosition = 0.0f;           // Snap to floor
            yVelocity *= -(1.0f - lossY); // Flip velocity and apply loss
            xVelocity *= (1.0f - lossX); // Apply loss

            // Optional: Stop the ball if it's just vibrating microscopically
            if (std::abs(yVelocity) < 0.01f)
            {
                yVelocity = 0.0f;
            }

            triggerY = true;
        }
        // Check for Ceiling Collision (1.0)
        else if (yPosition >= 1.0f)
        {
            yPosition = 1.0f;           // Snap to ceiling
            yVelocity *= -(1.0f - lossY); // Flip velocity and apply loss
            xVelocity *= (1.0f - lossX); // Apply loss

            // Optional: Stop the ball if it's just vibrating microscopically
            if (std::abs(yVelocity) < 0.01f)
            {
                yVelocity = 0.0f;
            }

            triggerY = true;
        }


        // Check for Left Collision (0.0)
        if (xPosition <= 0.0f)
        {
            xPosition = 0.0f;           // Snap to floor
            xVelocity *= -(1.0f - lossX); // Flip velocity and apply loss
            yVelocity *= (1.0f - lossY); // Flip velocity and apply loss

            // Optional: Stop the ball if it's just vibrating microscopically
            if (std::abs(xVelocity) < 0.01f)
            {
                xVelocity = 0.0f;
            }

            triggerX = true;
        }
        // Check for Right Collision (1.0)
        else if (xPosition >= 1.0f)
        {
            xPosition = 1.0f;           // Snap to ceiling
            xVelocity *= -(1.0f - lossX); // Flip velocity and apply loss
            yVelocity *= (1.0f - lossY); // Apply loss

            // Optional: Stop the ball if it's just vibrating microscopically
            if (std::abs(xVelocity) < 0.01f)
            {
                xVelocity = 0.0f;
            }

            triggerX = true;
        }

        return { xPosition, xVelocity , yPosition, yVelocity, maxVelocityX, maxVelocityY, triggerX, triggerY};
    }

private:

    float gravity{ -0.01f }; //We are saying that "up" is the positive direction y-direction
    float push{ 1.0f }; //Left or right push force to act as acceleration in the x-direction

    float lossX{ 0.01f };
    float lossY{ 0.01f };
    float xPosition{ 1.0f };
    float yPosition{ 0.0f };
    float xVelocity{ -1.0f };
    float yVelocity{ 1.0f };

    float maxVelocityX{ 0.0f };
    float maxVelocityY{ 0.0f };

    float mass{ 1.0f };
    float time{ 0.0f };
    int sample{ 0 };
    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000 };

    bool triggerY{ false };
    bool triggerX{ false };

};
