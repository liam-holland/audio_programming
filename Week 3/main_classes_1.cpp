#include <algorithm>
#include <iostream>
// ✅ Removed unordered_map
#include <vector>   // ✅ Added vector
#include <random>
#include <pstl/utils.h>


struct CatInfo {
    std::string name;
    float size;
};

// ==================================================
// DEFINITION OF THE CAT CLASS
class Cat
{
public:

    void addCat(std::string nameOfCat, float sizeOfCat)
    {
        cats.push_back(CatInfo{nameOfCat, sizeOfCat});  // push_back instead of key assignment
    }

    static void printCats() {
        for (size_t i = 0; i < cats.size(); ++i) {
            std::cout << "cat" << (i+1) << ": "
                      << cats[i].name << ", " << cats[i].size << " kg\n";
        }
    }

    // Access the dictionary
    static const std::vector<CatInfo>& getCats()
    {
        return cats;
    }


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

    //Set the size of the cat
    void setSize(float newSize)
    {
        size = newSize;
        std::cout << size << '\n';
    }

    // Return the size of the cat
    float getSize()
    {
        return size;
    }

    void meow()
    {
        if ( size < 10) {
            std::cout << name << ": MEOW!" << '\n';
        }
        else
        {
            std::cout << name << ": meow!" << '\n';
        }
    }

    /**
    Set the name and size of the cat
    */
    Cat(std::string name , float size)
    {
        std::cout << name << " has been born!" << '\n';
        addCat(name, size);
    }

private:
    std::string name{};
    float size{};
    static std::vector<CatInfo> cats;
};

// Define static member vector
std::vector<CatInfo> Cat::cats;



// ==================================================

int main() {

    // Create a random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(1, 20);

    // create instances of the Cat class
    Cat catA("Liam", dist(gen));
    Cat catB("Lindsey", dist(gen));
    Cat catC("Garfield", dist(gen));

    // Print all cats
    Cat::printCats();

    //Create a vector that references cats. We are going to use our own structure "CatInfo"

    const std::vector<CatInfo>& cats { Cat::getCats() };

    if ( cats.empty() ) {
        std::cout << "No cats found!" << '\n';
    }

    CatInfo largestCat = cats[0];   // assume at least one cat exists

    for (int i = 1; i < cats.size(); ++i) {
        if (cats[i].size > largestCat.size)
        {
            largestCat = cats[i];
        }
    }

    std::cout << "Largest cat: "
          << largestCat.name << ", "
          << largestCat.size << " kg\n";

        return 0;
    }

