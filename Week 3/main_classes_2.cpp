/* FIND THE ERRORS!

 This program sets up a race between two instances of the Person class
 One of the Persons runs, while the other walks, but the walker has a randomised head start

 The class is not set up properly however, and is not being called correctly in the main() code, leading to errors

 Look at the error messages and try and fix the code so that it runs, and a winner is declared

 */

#include <iostream>
#include <ctime>

class Person
{
public:

    void walk()
    {
        position += walkingSpeed;
    }

    void run()
    {
        position += runningSpeed;
    }

    float getPosition()
    {
        return position;
    }

    void setPosition(float newPos)
    {
        position = newPos;

    }


private:
    float position = 0.0f;
    float walkingSpeed = 1.0;
    float runningSpeed = 3.0;
};



int main() {

    // seed the rand() function (using <ctime> library)
    srand(time(nullptr));

    Person ali;
    Person barbara;

    // Ali has a randomised head start (between 40 and 80 metres)
    float headStart = (rand() %100) * 0.4 + 40;
    ali.setPosition(headStart);

    float finishLine = 100.0f;

    // but barbara is running not walking
    bool racing = true;
    while (racing)
    {
        ali.walk();
        barbara.run();

        // if they both cross the line on this step
        if (ali.getPosition() > finishLine && barbara.getPosition() > finishLine)
        {
            std::cout << "It's a tie!\n";
            racing = false;
        }

        else if (ali.getPosition() > finishLine && barbara.getPosition() < finishLine)
        {
            std::cout << "Alison wins!\n";
            racing = false;
        }
        else if (barbara.getPosition() > finishLine && ali.getPosition() < finishLine)
        {
            std::cout << "Barbara wins!\n";
            racing = false;
        }

    }

    std::cout << "A: " << ali.getPosition() << "\n";
    std::cout << "B: " << barbara.getPosition() << "\n\n";

    return 0;
}
