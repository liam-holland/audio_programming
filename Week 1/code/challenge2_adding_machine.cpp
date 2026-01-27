
// import the iostream library for cout and cin
#include <iostream>

// import the time library for our random number seeding
#include <ctime>

int main(int argc, const char * argv[]) {

    // this line "reseeds" our random number functions so that they are different every time
    srand (time(NULL));
    
    int valueA = rand() % 100; // create a random integer between 0 and 99
    int valueB = rand() % 100; // create a random integer between 0 and 99
    
    // ask the user to input a number
    std::cout << "What is " << valueA << " plus " << valueB << "?\n";
    
    // TODO:
    // use std:: cin to get the user's answer and write it to a new int variable called userAnswer
    
    // TODO:
    // create a new int variable that contains the ACTUAL answer, called actualAnswer
    
    // TODO:
    // use an if statement to see if the user has inputted the correct answer or not
    // if they have, then print something congratulatory
    // if they have not, then print something commiseratory
    
    
    
    return 0;
}


