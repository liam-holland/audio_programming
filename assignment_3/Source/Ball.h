/*
 ==============================================================================
    Ball.h
    Created: 4 Apr 2026 3:02:45pm
    Author:  Liam Holland
 ==============================================================================
*/

#pragma once
#include <cmath>
#include <JuceHeader.h>

/**
Creates a ball object.
The ball moves between a 4 walled square. The dimensions of the square is 1.0 x 1.0 (x,y)
If the ball hits a wall, it will bounce.
The user can choose the loss and friction the ball encounters when it hits a surface
*/
class Ball
{
public:
    Ball() = default;
    ~Ball() = default;

    /**
    Stores the variables associated with a ball
    */
    struct BallState
    {
        bool baseBall;
        float mass;
        float xAcceleration;
        float yAcceleration;
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

    /**
    * Set the type of the ball, the available options are "base" and "splash"
    * Only the first three balls are allowed to be "base"
    * @param _ballType The ball type
    */
    void setBallType(std::string _ballType)
    {
        if (_ballType == "base")
            baseBall = true;
        else if (_ballType == "splash")
            baseBall = false;
    }

    /**
    * Set the scalar acceleration of the ball in the X and Y direction.
    * @param _accelerationX Acceleration in the x-direction
    * @param _accelerationY Acceleration in the y-direction
    */
    void setAcceleration(float _accelerationX, float _accelerationY)
    {
        accelerationX = _accelerationX;
        accelerationY = _accelerationY;
    }

    /**
    * Set the start velocity of the ball.
    * @param _startVelocityX Start velocity of the ball in the x-direction
    * @param _startVelocityY Start velocity of the ball in the y-direction
    */
    void setStartVelocity(float _startVelocityX, float _startVelocityY)
    {
        xVelocity = _startVelocityX;
        yVelocity = _startVelocityY;
    }

    /**
    * Set the start position of the ball.
    * @param _startPositionX Start position of the ball in the x-direction
    * @param _startPositionY Start position of the ball in the y-direction
    */
    void setStartPosition(float _startPositionX, float _startPositionY)
    {
        xPosition = _startPositionX;
        yPosition = _startPositionY;
    }

    /**
    * Set the loss of the ball when it hits a surface.
    * @param _lossX Loss in x-direction
    * @param _lossY Loss in y-direction
    */
    void setLoss(float _lossX, float _lossY)
    {
        lossX = _lossX;
        lossY = _lossY;
    }

    void setMass(float _mass) { mass = _mass; }
    float getMass() { return mass; }

    /**
    * Prepare a ball to be initialised with the sample rate
    * @param newSampleRate The sample rate that is used in the project
    */
    void prepare(double newSampleRate)
    {
        sampleRate = static_cast<int>(newSampleRate);
    }

    /**
    Calculate the time step between samples, in seconds
    */
    void updateTimeStep(int _numSamples)
    {
        timeStep = (1.0f / (float)sampleRate) * (float)_numSamples;
    }

    void setCentreOfGravity(float _centreOfGravityX, float _centreOfGravityY)
    {
        centreOfGravityX = _centreOfGravityX;
        centreOfGravityY = _centreOfGravityY;
    }

    void gravitationalForceX()
    {
        float gravPosition = xPosition - centreOfGravityX;
        accelerationX = (gravPosition > 0) ? -std::abs(accelerationX) : std::abs(accelerationX);
    }

    void gravitationalForceY()
    {
        float gravPosition = yPosition - centreOfGravityY;
        accelerationY = (gravPosition > 0) ? -std::abs(accelerationY) : std::abs(accelerationY);
    }

    void updateVelocity()
    {
        gravitationalForceX();
        gravitationalForceY();
        yVelocityPrev = yVelocity;
        yVelocity = juce::jlimit(-maxVelocityY, maxVelocityY, yVelocity + accelerationY * timeStep);
        xVelocity = juce::jlimit(-maxVelocityX, maxVelocityX, xVelocity + accelerationX * timeStep);
    }

    void updatePosition()
    {
        xPosition += (xVelocity * timeStep);
        yPosition += (yVelocity * timeStep);
    }

    BallState processMovement(int _numSamples)
    {
        DBG(mass);
        
        if (mass < massLowerBoundary)
        {
            exists = false;
            justDied = true;
            return getState();
        }

        updateTimeStep(_numSamples);
        triggerY = false;
        triggerX = false;

        updateVelocity();
        updatePosition();

        // --- COLLISION LOGIC ---
        // Floor
        if (yPosition <= 0.0f)
        {
            mass *= (1.0f - massLoss);
            if (std::abs(yVelocity) >= 0.02f)
            {
                yPosition = 0.0f;
                yVelocity *= -(1.0f - lossY);
                xVelocity *= (1.0f - frictionX);
                triggerY = true;
                triggerYCount++;

            }
            else
            {
                yVelocity = 0.0f;
                yPosition = 0.0f;
            }
        }
        // Ceiling
        if (yPosition >= 1.0f)
        {
            mass *= (1.0f - massLoss);
            if (std::abs(yVelocity) >= 0.02f)
            {
                yPosition = 1.0f;
                yVelocity *= -(1.0f - lossY);
                xVelocity *= (1.0f - frictionX);
                triggerY = true;
                triggerYCount++;

            }
            else
            {
                yVelocity = 0.0f;
                yPosition = 1.0f;
            }
        }


        // Left
        if (xPosition <= 0.0f)
        {
            if (std::abs(xVelocity) >= 0.001f)
            {
                xPosition = 0.0f;
                xVelocity *= -(1.0f - lossX);
                yVelocity *= (1.0f - frictionY);
                triggerX = true;
            }
            else
            {
                xVelocity = 0.0f;
            }
        }
        // Right
        if (xPosition >= 1.0f)
        {
            if (std::abs(xVelocity) >= 0.001f)
            {
                xPosition = 1.0f;
                xVelocity *= -(1.0f - lossX);
                yVelocity *= (1.0f - frictionY);
                triggerX = true;
            }
            else
            {
                xVelocity = 0.0f;
            }
        }

        killSplash();
        zeroVelocityAddition();
        return getState();
    }

    BallState getState()
    {
        return { baseBall, mass, accelerationX, accelerationY, xPosition, xVelocity, yPosition, yVelocity, maxVelocityX, maxVelocityY, triggerX, triggerY, exists };
    }

    void zeroVelocityAddition()
    {
        float threshold = 0.001f;
        if (std::abs(yVelocity) <= threshold && std::abs(yVelocityPrev) <= threshold)
            zeroVelocityCount++;
        else
            zeroVelocityCount = 0;

        if (zeroVelocityCount > 5)
        {
            exists = false;
            zeroVelocityCount = 0;
        }
    }

    void killSplash()
    {
        if (triggerYCount > 3 && !baseBall)
            exists = false;
    }

    void create()
    {
        exists = true;
        triggerYCount = 0;
        zeroVelocityCount = 0;
        triggerX = false;
        triggerY = false;
    }

    bool getExists() { return exists; }
    void setExisits(bool _exists) { exists = _exists; }
    bool getJustDied() { return justDied; }

private:
    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000.0f };
    float time{ 0.0f };
    float xVelocity{ 1.0f };
    float yVelocity{ -1.0f };
    float xPosition{ 1.0f };
    float yPosition{ 0.5f };
    float frictionY{ 0.001f };
    float frictionX{ 0.001f };
    float maxVelocityX{ 10.0f };
    float maxVelocityY{ 10.0f };
    float accelerationX{ 0.01f };
    float accelerationY{ -9.81f };
    float lossX{ 0.01f };
    float lossY{ 0.01f };
    float centreOfGravityX{ 0.5f };
    float centreOfGravityY{ 0.5f };
    float mass{ 1.0f };
    bool exists{ false };
    bool baseBall{ true };
    float xVelocityPrev{ 1.0f };
    float yVelocityPrev{ -1.0f };
    int zeroVelocityCount{ 0 };
    bool triggerY{ false };
    bool triggerX{ false };
    int triggerYCount{ 0 };

    float massLoss{ 0.1 };
    float massLowerBoundary{ 0.01 };
    bool justDied = false;
};