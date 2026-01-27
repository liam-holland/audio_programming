#include "io.h"
#include <iostream>

int readNumber()
{
    int x{};
    std::cin >> x;

    return x;
}

void writeAnswer(int x, int y)
{
    std::cout << "Total: " << x + y << '\n';
}