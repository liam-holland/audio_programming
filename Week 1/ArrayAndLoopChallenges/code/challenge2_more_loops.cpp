// Adapt this code so that:
// 1) it loops 10 times instead of 3 times
// 2) it prints out the iteration number on each loop (0, 1, 2, 3, 4 ... 9)
// 3) then change the code so that instead of {0, 1, 2, 3...}, it prints the first 10 square numbers, starting from 1: (1, 4, 9, 16 ...)
// 4) change it so that it asks the user how many square numbers it should output, then outputs that amount.

// note that you won't need to use arrays for this! You can work directly with the i variable inside the loop

#include <iostream>

int main(int argc, const char * argv[]) {
    
    // a simple for loop
    for (int i=0; i<3; i++)
    {
        std::cout << "Looping!" << "\n";
    }
    
    
    return 0;
}
