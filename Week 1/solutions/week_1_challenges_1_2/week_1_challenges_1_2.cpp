
// import the iostream library for cout and cin
#include <iostream>

// import the time library for our random number seeding
#include <ctime>

//import the cmath library
#include <cmath>

int main(int argc, const char* argv[]) {

    // this line "reseeds" our random number functions so that they are different every time
    srand(time(NULL));

    int valueA = rand() % 100; // create a random integer between 0 and 99
    int valueB = rand() % 100; // create a random integer between 0 and 99

    // ask the user to input a number
    std::cout << "What is " << valueA << " plus " << valueB << ": ";

    // TODO:
    // use std:: cin to get the user's answer and write it to a new int variable called userAnswer
    int userAnswer{};
    std::cin >> userAnswer;

    // TODO:
    // create a new int variable that contains the ACTUAL answer, called actualAnswer
    int actualAnswer{ valueA + valueB };

    // TODO:
    // use an if statement to see if the user has inputted the correct answer or not
    // if they have, then print something congratulatory
    // if they have not, then print something commiseratory

    if (userAnswer == actualAnswer)
        std::cout << "Congtaulations! That is the correct answer \n";
    else
        std::cout << "Oops, you are off by a value of " << abs(actualAnswer - userAnswer) << '\n';



    return 0;
}


