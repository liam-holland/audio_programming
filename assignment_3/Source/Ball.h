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
    * 
    * @param _ballType The ball type
    */
    void setBallType(std::string _ballType)
    {
        if (_ballType == "base")
        {
            baseBall = true;
        }
        else if (_ballType == "splash")
        {
            baseBall = false;
        }
    }

    /**
    * Set the scalar acceleration of the ball in the X and Y direction.
    * The centre of gravity variable, which can be set will decide the direction of acceleration
    * 
    * @param _accelerationX Acceleration in the x-direction
    * @param _accelerationY Acceleration in the y-direction
    */
    void setAcceleration(float _accelerationX, float _accelerationY )
    {
        accelerationX = _accelerationX;
        accelerationY = _accelerationY;
        
    }

    /**
    * Set the start velocity of the ball. Once this is set, this variable cannot be changed until the ball re-created
    * 
    * @param __startVelocityX Start velocity of the ball in the x-direction
    * @param __startVelocityY Start velocity of the ball in the y-direction
    */
    void setStartVelocity(float _startVelocityX , float _startVelocityY)
    {
        xVelocity = _startVelocityX;
        yVelocity = _startVelocityY;
    }

    /**
    * Set the start position of the ball. Once this is set, this variable cannot be changed until the ball re-created
    * 
    * @param _startPositionX Start position of the ball in the x-direction
    * @param _startPositionY Start position of the ball in the y-direction
    */
    void setStartPosition(float _startPositionX, float _startPositionY)
    {
        xPosition = _startPositionX;
        yPosition = _startPositionY;
    }

    /**
    * Set the loss of the ball when it hits a surface. When the ball hits the left, right wall it will lose velocity
    * in the X direction. When the ball hits the floor, ceiling, it will lose velocity in the Y direction
    * 
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
    * 
    * @param _mass Mass of ball
    */
    void setMass(float _mass)
    {
        mass = _mass;
    }

    /**
    * Return the mass of the ball
    * 
    * @return Mass of the ball
    */
    float getMass()
    {
        return mass;
    }

    /**
    Solve the maximum max velocity of the ball given the starting conditions
    */
    //void solveMaxVelocity()
    //{
    //    // The maximum possible distance is 1.0 (from 0.0 to 1.0 or vice versa)
    //    float maxDist = 1.0f;

    //    // Use absolute values to ensure I never sqrt a negative number
    //    maxVelocityX = std::sqrt( std::pow(xVelocity, 2) + 2 * std::abs(accelerationX) * maxDist);
    //    maxVelocityY = std::sqrt (std::pow(yVelocity, 2) + 2 * std::abs(accelerationY) * maxDist);
    //}

    /**
    * Prepare a ball to be initialised with the sample rate
    * 
    * @param newSampleRate The sample rate that is used in the project
    */
    void prepare(double newSampleRate)
    {
        sampleRate = static_cast<int>(newSampleRate);
        updateTimeStep();
    }

    /**
    Calculate the time step between samples, in seconds
    */
    void updateTimeStep()
    {
        timeStep = 1.0f / sampleRate;
    }

    /**
    * Set the centre of gravity in the X and Y direction. This will determine the "force" applied to the ball
    * if the accerlation value is greater than 0.0
    * 
    * @param _centreOfGravityX The centre of gravity in the x-direction
    * @param _centreOfGravityY The centre of gravity in the y-direction
    */
    void setCentreOfGravity( float _centreOfGravityX, float _centreOfGravityY)
    {
        centreOfGravityX = _centreOfGravityX;
        centreOfGravityY = _centreOfGravityY;
    }

    /**
    Determine the sign of the acceleration in the x - direction.
    
    The ball will move towards the centre of gravity if acceleration is greater than 0.0
    */
    void gravitationalForceX()
    {
        float gravPosition = xPosition - centreOfGravityX;

        if (gravPosition > 0)
        {
            accelerationX *= -1;
        }
        else if (gravPosition <= 0)
        {
            accelerationX *= 1;
        }
    }

    /**
    Determine the sign of the acceleration in the y - direction.
    
    The ball will move towards the centre of gravity if acceleration is greater than 0.0
    */
    void gravitationalForceY()
    {
        float gravPosition = yPosition - centreOfGravityY;

        if (gravPosition > 0)
        {
            accelerationY *= -1;
        }
        else if (gravPosition <= 0)
        {
            accelerationY *= 1;
        }
    }

    /**
    Update the velocity of the ball, based off the force that is applied
    */
    void updateVelocity()
    {
        gravitationalForceX();

        yVelocityPrev = yVelocity;

        yVelocity = juce::jlimit( -maxVelocityY, maxVelocityY, yVelocity +  accelerationY * timeStep);
        xVelocity = juce::jlimit(-maxVelocityX, maxVelocityX, xVelocity + accelerationX * timeStep);

    }

    /**
    Update the position of the ball, based off the velocity 
    */
    void updatePosition()
    {
        float shiftX = (xVelocity * timeStep);
        float shiftY = (yVelocity * timeStep);
        
        xPosition += shiftX;
        yPosition += shiftY;
    }

    /**
    * Process the movement of the ball for one step
    * 
    * @return BallState - will return the current state of all the variables associated with the ball
    */
    BallState processMovement()
    {
        // Reset the triggers to false
        triggerY = false;
        triggerX = false;

        // Perform the velocity and position calculations
        updateVelocity();
        updatePosition();

        // --- COLLISION LOGIC ---

        // Check for Floor Collision (0.0)
        if (yPosition <= 0.0f)
        {
            if (std::abs(yVelocity) >= 0.001f)
            {
                yPosition = 0.0f;                // Snap to floor
                yVelocity *= -(1.0f - lossY);    // Flip velocity and apply loss
                xVelocity *= (1.0f - frictionX); // Apply loss

                // Set of the y-direction trigger
                triggerY = true;
                triggerYCount++;
            }
            else // If the velocity is too small, then make it zero
            {
                yVelocity = 0.0f;
            }            
        }

        // Check for Ceiling Collision (1.0)
        if (yPosition >= 1.0f)
        {
            
            if (std::abs(yVelocity) >= 0.001f)
            {
                yPosition = 1.0f;               // Snap to ceiling
                yVelocity *= -(1.0f - lossY);   // Flip velocity and apply loss
                xVelocity *= (1.0f - frictionX);// Apply loss

                triggerY = true;
                triggerYCount++;
            }
            else 
            {
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

        // Kill splash balls, if they have bounced more than 3 times
        killSplash();
        // Destroy the ball, if the velocity has been set to zero 5 times
        zeroVelocityAddition();

        // Retrun the state of the ball
        return getState();
    }

    BallState getState()
    {
        return { baseBall, mass, accelerationX, accelerationY , xPosition, xVelocity , yPosition, yVelocity, maxVelocityX, maxVelocityY, triggerX, triggerY, exists };
    }

    /**
    Use this function to kill the ball if the velocity has been set to zero more than 5 times in a row
    */
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

    /**
    Kill the splash balls if they have hit the floor or ceiling more than 3 times
    */
    void killSplash()
    {
        if (triggerYCount > 3 && !baseBall)
        {
            exists = false;
        }
    }

    /**
    Set a ball to exist
    */
    void create()
    {
        exists = true;
    }

    /**
    Return the existance of a ball 
    */
    bool getExists()
    {
        return exists;
    }

    /**
    * Set the existance of a ball
    * 
    * @param _exists The existance of a ball
    */
    void setExisits( bool _exists)
    {
        exists = _exists;
    }


private:


    int sampleRate{ 48000 };
    float timeStep{ 1.0f / 48000 };
    float time{ 0.0f };

    // Cannot change once the ball exits
    // Velocity and position are set up before the ball is set as existing
    // Once the ball exists, they are determined from other parameters
    float xVelocity{ 1.0f };
    float yVelocity{ -1.0f };

    float xPosition{ 1.0f };
    float yPosition{ 0.0f };

    float frictionY{ 0.001f };
    float frictionX{ 0.001f };
    
    // Precompute the max velocity of ball
    float maxVelocityX{ 10.0f };
    float maxVelocityY{ 10.0f };

    // Can change throughout the balls lifetime

    float accelerationX{ 0.01f }; //We are saying that "up" is the positive direction y-direction
    float accelerationY{ -9.81 }; //Left or right accelerationX force to act as acceleration in the x-direction

    float lossX{ 0.01f };
    float lossY{ 0.01f };

    float centreOfGravityX{ 0.5 };
    float centreOfGravityY{ 0.5 };

    float mass{ 1.0f };

    // Existance and type of ball
    bool exists{ false };
    bool baseBall{ true };


    // Helper variables

    float xVelocityPrev{ 1.0f};
    float yVelocityPrev{ -1.0f };

    int zeroVelocityCount{ 0 };

    bool triggerY{ false };
    bool triggerX{ false };

    int triggerYCount{ 0 };

};
