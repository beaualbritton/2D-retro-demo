//
// Rehashing my code from M1AP1, using for later validation
//
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

//helper function
bool has_space(string pString);

char get_char_from_user() {
    // NOTE: a char is not restricted to an alphabet letter

    /*
     *input for "Character" is defined as a *string* of length 1.
     *even though the return value is char. So, I declared
     *two inputs, charInput will be returned, but stringInput
     *is what is being fed the input and being evaluated.
     *
     *Since a string is just a collection of chars, when input is validated
     *to be no longer than one, that means stringInput contains 1 char
     *at stringInput[0] (index 0).
     */
    string stringInput;
    char charInput;
    getline(cin,stringInput);
    while(stringInput.empty() || stringInput.length()>1) {
        if(stringInput.empty()) {
            cout<<"No input. ";
        }
        else if(stringInput.length()>1) {
            cout <<"Invalid input. ";
        }
        cout << "Enter a single character:";
        getline(cin,stringInput);
    }
    charInput = stringInput[0];
    return charInput;
}

string get_word_from_user() {
    // TODO: Complete the function
    /*
     *A word is defined in the readme as an input string with *no whitespace*
     */
    string wordInput;
    getline(cin,wordInput);

    //In the following while conditonal, has_space() is a helper function. See method body
    while(wordInput.empty() || has_space(wordInput)) {

        //Since the while conditional is either empty input or space containing
        //There has to be another check using an if statement for proper printing.
        if(wordInput.empty()) {
            cout<<"No input. Enter a single word:";
        }
        else if(has_space(wordInput)) {
            cout <<"Invalid input. Enter a single word:";
        }
        getline(cin,wordInput);
    }

    return wordInput;
}
bool has_space(string pString){
    /*
     * Helper function for get_word_from_user() to identify
     * if there are any whitespaces found in the given string.
     *
     * Iterates through each index of input pString (any arbitrary string) seeing if
     * said index contains a space using std::isspace()
     */
    for(int i = 0; i<pString.length(); ++i) {
        if(isspace(pString[i]))
            return true;
    }
    return false;
}

string get_sentence_from_user() {
    // NOTE: a sentence does NOT need punctuation or syntax validation
    /*
     * Checks for valid (nonempty) sentence input. Given the example output
     * on readme.md I figured that this was relatively simple. There is no
     * distinction between whitespaces or punctuation, so a sentence (in this case)
     * is a nonempty string.
     */
    string sentenceInput;
    getline(cin,sentenceInput);
    while(sentenceInput.empty()) {
        cout<<"No input. Enter a setence:";
        getline(cin,sentenceInput);
    }
    return sentenceInput;
}

int get_int_from_user() {
    /* boy oh boy.. was stuck on this for a while
     *
     *  This method uses the stringstream class to read input strings
     *  and convert into the appropriate integer type.
     *
     */

    string integerInput;
    int input;
    stringstream stringStream;
    bool validated;

    /*
     * While loop checking if "validated"
     * This boolean represents whether or not a valid integer has been
     * reached. This will loop until one is input.
     */
    while(!validated) {
        getline(cin,integerInput);
        //assigning string input to the string stream
        stringStream << integerInput;

        //reading from the string for proper input. stringStream >> input will return
        //false if there is an incorrect input for type int
        if(stringStream >>input) {
            //using this char to see any additional characters appended
            //to an input, i.e. (123 abc) for example output in readme.md
            char overflow_test;

            //then reading from stringstream to see if those appended characters exist
            if(stringStream>>overflow_test) {
                cout << "Invalid input. ";
            }
            //else, then there is no problem with the input string, therefore validated
            else{
                validated = true;
            }
        }
        //Do not validate if the given input is empty.
        else if (integerInput.empty())
            cout<<"No input. ";
        //if stringstream>>input is false, this will loop back
        else
            cout<<"Invalid input.";

        //stringStream.clear() wouldn't work on its own. So I had to use
        //both of these statements. Found this solution on stack overflow
        stringStream.clear();
        stringStream.str("");
    }
    return input;
}

double get_float_from_user() {

    // NOTE: a float does not need to have a decimal part (meaning an integer is a valid float)

    //string and floats for input
    string floatInput;
    float input;
    stringstream stringStream;
    bool validated;

    //Very similar loop to get_int_from_user(). Checks to see if valid. Keeps looping if not
    while(!validated) {
        //However, this time it uses floats.
        getline(cin,floatInput);
        //Copying float input into stream
        stringStream << floatInput;
        //Checking this time for proper float type input
        if(stringStream >>input) {
            //Now checking to see if there is any additional characters
            //that should not be in the input, handling it as appropriate
            char overflow_test;
            if(stringStream>>overflow_test) {
                cout << "Invalid input. ";
            }
            else{
                validated = true;
            }
        }
        //Not valid if empty,
        else if (floatInput.empty())
            cout<<"No input. ";
        else
            cout<<"Invalid input. ";

        //Same problem in get_int_from_user(), stringStream requires both of these to be called
        //so that strinstream is not in a "bad" state"
        stringStream.clear();
        stringStream.str("");
    }
    return input;
}