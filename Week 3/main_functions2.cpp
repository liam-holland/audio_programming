/******************
 Find the errors!

 This code generates semi-random sentences. The code here will likely look confusing and difficult.
 You only need to focus on whether the functions are well formatted though.

 Look at the error messages and warnings that Xcode or VS is showing.
 Look back at previous examples of functions to check that they:
 a) are accepting the correct type of arguments (if any)
 b) are returning the right type of data (if any)
 c) the calls to the function are properly formatted.

 There are four functions here:
 randomNoun() - this should pick one random word from the list and return it. This function
 randomAction() - similar, but chooses a random action string.
 randomSentence() - generates one random sentence using the above functions
 makeSomeJunkSentences(int numberToMake) - generates any number of sentences, depending on the number passed in

 There are FOUR errors here. As a hint, none of them are in the main() function!

 ******************/

#include <iostream>     // for std::cin, std::cout, etc
#include <time.h>       // for seeding the random function


// pick a random noun and return it as a std::string
std::string randomNoun()
{
    // define an array of 8 nouns
    std::string nouns[8] = {"person", "frog", "house", "car", "horse", "cat", "drum", "computer"};
    int index = rand() % 8;                         // pick an integer between 0 and 7
    std::string wordToOutput = nouns[index];        // get that element from the list

    return wordToOutput;
}


// pick a random action and return it as a std::string
std::string randomAction()
{
    // define an array of 8 actions
    std::string actions[8] = {"running into", "sitting on", "fighting with", "playing with", "eating", "afraid of", "mystified by", "easily amused by"};
    int index = rand() % 8;                         // pick an integer between 0 and 7
    std::string wordToOutput = actions[index];      // get that element from the list
    return wordToOutput;                            // return the word
}

// generate a sentence using the randomNoun() and randomAction() functions
void randomSentence()
{
    std::string sentence = "The ";
    sentence += randomNoun();             // we use the "+=" here to keep adding text onto the end of our string.
    sentence += " is ";
    sentence += randomAction();
    sentence += " the ";
    sentence += randomNoun();
    std::cout << sentence << "\n";      // print the whole sentence
}

// make any number of sentences!
void makeSomeJunkSentences( int numberToMake )
{
    for (int i=0; i<numberToMake; i++)
    {
        randomSentence();
    }
}



int main() {

    // this line uses the time to set the random seed - this makes sure that the rand() function is actually different each time.
    srand( static_cast<int>(time(nullptr)) );

    // call the function to make 3 random sentences
    makeSomeJunkSentences(3);

    return 0;
}
