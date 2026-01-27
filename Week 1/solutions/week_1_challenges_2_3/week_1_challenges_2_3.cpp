// To Do:
//
// 1) Use the numberOfSamples variable to set the vector size to 10 samples.
// 2) Create a for loop that goes from zero to the number of samples.
// 3) Assign a random value to each element in the samples vector between 0 and 9999. You can use the rand() function for this - see the pdf worksheet.
// 4) Add a std::cout to print the value of each element in the vector, with each value on a new line.
// 5) Adapt the code so that the random number is a floating point number in the range 0 and 1 (hint: don't change the random number itself, but multiply or divide the generated number in order to scale it into the correct range.
// 6) Adapt this code again so that the random number is in the range -1 to 1 (the usual range for an audio signal.

#include <iostream>
#include <vector>
#include <ctime>

int main() {

    int numberOfSamples{ 10 };

    // set up a vector to store the sample data
    std::vector<float> samples(numberOfSamples);

    // set up the for loop below and use it to populate the samples vector
    // 
    // Get a different random number each time the program runs
    srand(time(0));
    int randomNumber{};

    for (int i{ 0 }; i < numberOfSamples; ++i)
        {   
        randomNumber = rand() % 10000;
        samples[i] = ((randomNumber / 10000.0f) - 0.5) * 2 ;
        std::cout << samples[i] << '\n';
        }

    std::cout << "\n ** Done! ** \n\n";

    return 0;
}
