// MULTIPLY FUNCTION CHALLENGE

#include <iostream>


// CREATE A MULTPLY FUNCTION HERE THAT CAN TAKE TWO FLOATS AND RETURN ONE FLOAT
// ...

float multiply(float a, float b)
{
      return a * b;
}


int main(int argc, const char * argv[]) {


    float a = 12.56;
    float b = 3.1415;

    float c = multiply(a, b);

    // print the output
    std::cout << a << " * " << b << " = " << c << "\n\n";

    return 0;
}
