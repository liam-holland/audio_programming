// FIND THE ERRORS!
// Try and make this code run correctly.
// When correct, it should output the numbers 3, 5 and 6 from the myArray using the for loop

// remember - to run this code:
// 1) create a new project in Xcode or Visual Studio
// 2) find the main.cpp and delete all of the default code
// 3) paste this code in its place and build+run

#include <iostream>
#include <vector>

int main(int argc, const char* argv[]) {

    // set up a new array with three elements
    std::vector myArray{ 3, 5, 6 };

    // loop through the elements, printing each one on a new line
    for (int i{ 0 } ; i < 3 ; ++i)
    {
        std::cout << myArray[i] << '\n';
    }


    return 0;
}
