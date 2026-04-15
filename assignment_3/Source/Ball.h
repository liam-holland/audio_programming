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
        bool exists;
    };

    void setAcceleration(float _accelerationX, float _accelerationY )
    {
        accelerationX = _accelerationX;
        accelerationY = _accelerationY;
        
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
        maxVelocityX = std::sqrt( std::pow(xVelocity, 2) + 2 * std::abs(accelerationX) * maxDist);
        maxVelocityY = std::sqrt (std::pow(yVelocity, 2) + 2 * std::abs(accelerationY) * maxDist);
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

        yVelocityPrev = yVelocity;

        yVelocity += accelerationY * timeStep;
        xVelocity += accelerationX * timeStep;

        // Inside updateVelocity()
        //if (std::abs(yPosition) > 0.001f || std::abs(yVelocity) > 0.01f) {
        //    yVelocity += (accelerationY * timeStep);
        //}

        //if (std::abs(xPosition) > 0.001f || std::abs(xVelocity) > 0.01f) {
        //    xVelocity += (accelerationX * timeStep);
        //}

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
            if (std::abs(yVelocity) >= 0.001f)
            {
                yPosition = 0.0f;           // Snap to floor
                yVelocity *= -(1.0f - lossY); // Flip velocity and apply loss
                xVelocity *= (1.0f - frictionX); // Apply loss

                triggerY = true;
            }
            else
            {
                yVelocity = 0.0f;
            }            
        }

        // Check for Ceiling Collision (1.0)
        if (yPosition >= 1.0f)
        {
            
            if (std::abs(yVelocity) >= 0.001f)
            {
                yPosition = 1.0f;           // Snap to ceiling
                yVelocity *= -(1.0f - lossY); // Flip velocity and apply loss
                xVelocity *= (1.0f - frictionX); // Apply loss

                triggerY = true;
            }
            else {
                yVelocity = 0.0f;
            }
            

        }


        // Check for Left Collision (0.0)
        if (xPosition <= 0.0f)
        {

            // Optional: Stop the ball if it's just vibrating microscopically
            if (std::abs(xVelocity) >= 0.001f)
            {
                xPosition = 0.0f;           // Snap to floor
                xVelocity *= -(1.0f - lossX); // Flip velocity and apply loss
                yVelocity *= (1.0f - frictionY); // Flip velocity and apply loss

                triggerX = true;
            }

            else {
                xVelocity = 0.0f;
            }
            

        }
        // Check for Right Collision (1.0)
        if (xPosition >= 1.0f)
        {
            if (std::abs(xVelocity) >= 0.001f)
            {
                xPosition = 1.0f;           // Snap to ceiling
                xVelocity *= -(1.0f - lossX); // Flip velocity and apply loss
                yVelocity *= (1.0f - frictionY); // Apply loss

                triggerX = true;
            }

            else {
                xVelocity = 0.0f;
            }
            
        }

        zeroVelocityAddition();

        return { xPosition, xVelocity , yPosition, yVelocity, maxVelocityX, maxVelocityY, triggerX, triggerY, exists};
    }

    void zeroVelocityAddition()
    {
        if ( std::abs(yVelocity) <= 0.0f && std::abs(yVelocityPrev) <= 0.0f)
        {
            zeroVelocityCount++;
        }

        else
        {
            zeroVelocityCount = 0;
        }

        if (zeroVelocityCount > 5)
        {
            exists = false;
        }
    }


    void create()
    {
        exists = true;
    }

    bool getExists()
    {
        return exists;
    }


private:

    float accelerationX{ 0.01f }; //We are saying that "up" is the positive direction y-direction
    float accelerationY{ -9.81 }; //Left or right accelerationX force to act as acceleration in the x-direction

    float lossX{ 0.01f };
    float lossY{ 0.01f };

    float xPosition{ 1.0f };
    float yPosition{ 0.0f };

    float xVelocity{ 1.0f};
    float yVelocity{ -1.0f };

    float xVelocityPrev{ 1.0f};
    float yVelocityPrev{ -1.0f };

    int zeroVelocityCount{ 0 };

    float frictionY{ 0.001f };
    float frictionX{ 0.001f };

    float maxVelocityX{ 0.0f };
    float maxVelocityY{ 0.0f };

    float mass{ 1.0f };
    float time{ 0.0f };
    int sample{ 0 };
    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000 };

    bool triggerY{ false };
    bool triggerX{ false };

    bool exists{ false };
};
