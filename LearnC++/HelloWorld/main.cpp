#include "io.h"
#include <iostream>

int main()
{
    std::cout << "We are going to add two numbers together. " << '\n';
    
    std::cout << "Enter the first number: " << '\n';
    int firstNumber{ readNumber()};

    std::cout << "Enter the second number number: " << '\n';
    int secondNumber{ readNumber() };

    writeAnswer(firstNumber, secondNumber);

    return 0;
}