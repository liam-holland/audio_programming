#include <iostream>
#include <string>


int main()
{
    std::cout << "Enter your full name: ";
    std::string name{};
    std::getline(std::cin >> std::ws, name);
    //std::cout << name << '\n';

    int lengthOfName{ static_cast<int>(name.length()) };


    std::cout << "Enter your age: ";
    int age{};
    std::cin >> age;
    //std::cout << age << '\n';

    std::cout << "Your age + length of name is: " << age + lengthOfName << '\n';



    return 0;
}