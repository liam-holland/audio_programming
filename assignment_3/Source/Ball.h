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
    * Set the velocity loss of the ball when it hits a surface.
    * @param _lossX Loss in x-direction
    * @param _lossY Loss in y-direction
    */
    void setLoss(float _lossX, float _lossY)
    {
        lossX = _lossX;
        lossY = _lossY;
    }

    /**
    * Set the mass of the ball.
    * @param _mass
    */
    void setMass(float _mass) { mass = _mass; }

    /**
    * Get the mass of the ball.
    * @return _mass
    */
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

    /**
    Set the centre of gravity in the X and Y coordinates.
    Must be between 0 and 1
    */
    void setCentreOfGravity(float _centreOfGravityX, float _centreOfGravityY)
    {
        centreOfGravityX = _centreOfGravityX;
        centreOfGravityY = _centreOfGravityY;
    }

    /**
    Calculate the acceleration due to gravity in the x-direction
    */
    void gravitationalForceX()
    {
        float gravPosition = xPosition - centreOfGravityX;
        accelerationX = (gravPosition > 0) ? -std::abs(accelerationX) : std::abs(accelerationX);
    }

    /**
    Calculate the acceleration due to gravity in the y-direction
    */
    void gravitationalForceY()
    {
        float gravPosition = yPosition - centreOfGravityY;
        accelerationY = (gravPosition > 0) ? -std::abs(accelerationY) : std::abs(accelerationY);
    }

    /**
    Update the velocity of the ball based on the forces applied
    */
    void updateVelocity()
    {
        gravitationalForceX();
        gravitationalForceY();

        yVelocity = juce::jlimit(-maxVelocityY, maxVelocityY, yVelocity + accelerationY * timeStep);
        xVelocity = juce::jlimit(-maxVelocityX, maxVelocityX, xVelocity + accelerationX * timeStep);
    }

    /**
    Update the position of the ball, based on the velocity
    */
    void updatePosition()
    {
        xPosition += (xVelocity * timeStep);
        yPosition += (yVelocity * timeStep);
    }


    /**
    Process the movement of the ball. This will also apply triggers if the ball collides
    with a surface. This also kills the ball if it is lower than the kinetic energy
    boundary of 0.01

    @param _numSamples Number of samples of the buffer
    */
    BallState processMovement(int _numSamples)
    {

        float kineticEnergyY = 0.5f * mass * (std::abs(yVelocity) * std::abs(yVelocity));
        
        if (kineticEnergyY < kineticEnergyBoundary)
        {
            if (baseBall)
            {
                DBG("Ball Killed!");
            }

            exists = false;
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
        return getState();

    }

    BallState getState()
    {
        return { baseBall, mass, accelerationX, accelerationY, xPosition, xVelocity, yPosition, yVelocity, maxVelocityX, maxVelocityY, triggerX, triggerY, exists };
    }

    /**
    Kills any splash balls afte they collide three times. This is so that we don't run
    out of memory 
    */
    void killSplash()
    {
        if (triggerYCount > 3 && !baseBall)
            exists = false;
    }

    /**
    Create a ball, and initialse some base variables
    */
    void create()
    {
        exists = true;
        triggerYCount = 0;
        triggerX = false;
        triggerY = false;
    }

    /**
    Set the mass loss of the ball
    */
    void setMassLoss( float _massLoss)
    {
        massLoss = _massLoss;
    }

    /**
    Return the existance state of the ball
    */
    bool getExists() { return exists; }

    /**
    Set the existance state of the ball
    */
    void setExisits(bool _exists) { exists = _exists; }


private:
    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000.0f };
    float xVelocity{ 1.0f };
    float yVelocity{ -1.0f };
    float xPosition{ 1.0f };
    float yPosition{ 0.5f };
    float frictionY{ 0.0f };
    float frictionX{ 0.0f };
    float maxVelocityX{ 10.0f };
    float maxVelocityY{ 10.0f };
    float accelerationX{ 0.0f };
    float accelerationY{ 0.0f };
    float lossX{ 0.00f };
    float lossY{ 0.00f };
    float massLoss{ 0.0 };
    float centreOfGravityX{ 0.5f };
    float centreOfGravityY{ 0.0f };
    float mass{ 1.0f };
    bool exists{ false };
    bool baseBall{ true };
    bool triggerY{ false };
    bool triggerX{ false };
    int triggerYCount{ 0 };
    float kineticEnergyBoundary{ 0.01 };

};