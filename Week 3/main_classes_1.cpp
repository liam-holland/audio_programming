#include <iostream>

// ==================================================
// DEFINITION OF THE CAT CLASS
class Cat
{
public:
    // public member variables and functions go below this line
    

    // set the name
    void setName(std::string newName)
    {
        name = newName;
    }

    // return the name
    std::string getName()
    {
        return name;
    }

    
private:
    // private member variables and functions go below this line
    std::string name = "";          // store the name of the cat
};




// ==================================================

int main(int argc, const char * argv[]) {
    
    // create a single instance of the Cat class
    Cat catA;
    
    // use the setName method defined above to set the name variable
    catA.setName("Albert");

    
    return 0;
}
