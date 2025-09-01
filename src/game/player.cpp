//
// Created by beau on 10/6/2024.
//

#include "player.h"
#include <fstream>
#include "input.h"


/*
 * A Player has-a entity.
 * Player represents (you) the user as it controls a character in the game
 * This character is an entity type and uses the appropriate class methods
 */
player::player() {

    makePlayer();
}
entity makeNewCharacter(){
    // Prompt and get name from the user
    cout<<"Create your new character! "<< endl;


    /*
     *Name, description, health and experience inputs. All pretty self explanatory.
     *Uses input.h methods to validate input properly
     */
    cout << "Enter your character's name: ";
    string name = get_sentence_from_user();

    cout << "Character Name: " << name << endl;
    cout << "Enter a description for your character: ";
    string description = get_sentence_from_user();
    cout << "Character Description: " << description << endl;

    cout << "Enter your character's health: ";
    float health = get_float_from_user();
    cout << "Character Health: " << health << endl;

    cout << "Enter your character's experience: ";
    float experience = get_float_from_user();
    cout << "Character Experience: " << experience << endl;

    cout << "Enter your character's alignment:";
    int alignment = get_int_from_user();
    cout << "Character Alignment: " << alignment << endl<<endl<<endl;

    //Returns a newly created entity with all validated input fields
    entity entityToReturn = entity(health, experience, name, description, alignment);
    return entityToReturn;
}

/*
 * makePlayer() method does a few things. Fi
 *
 * Firstly, it reads in player data found in data/entity-data/playinfo.csv.
 * if there is data that can be found, continue and see if it is empty or not. If the data cannot
 * be found "Error" is printed with cout.
 *
 * Then, if data can be found, it reads it and sees if its empty, or not. If so it
 * either prompts the use to continue or make a new character. If the player continues,
 * it reads save data and loads an appropriate entity matching the input from playerinfo.csv
 *
 * Or, all else, a new character is created as a failproof. (couldn't read data or misinput)
 *
 */
void player::makePlayer() {

    string line;
    ifstream fileIn;
    fileIn.open("../data/entity-data/playerinfo.csv");

    //Valid data or not
    if(fileIn) {
        getline(fileIn,line);
        //Reading empty line
        if(!line.compare("")){
            cout<<"No saved data found. Create a new character!"<<endl;
            entity entity = makeNewCharacter();
            setPlayerEntity(entity);
        }
        else {
            //Save data found
            cout<<endl<<"Save data found. Would you like to (c)ontinue or make a (n)ew character?" <<endl<<endl;
            cout <<"Enter c or n:"<<endl;
            char choice = get_char_from_user();
            if(choice == 'c') {
                //Loading save data
                string name, description;
                float health,experience;
                int alignment;
                char comma = ',';
                //Reading values in from fileIn and assigning them to variables via >> operator
                getline(fileIn, name, comma);
                getline(fileIn, description, comma);
                fileIn>>health;
                fileIn>>comma;
                fileIn>>experience;
                fileIn>>comma;
                fileIn>>alignment;

                //Creating a new entity and sending it through setPlayerEntity() method
                entity entityToCreate = entity(health,experience,name, description,alignment);
                setPlayerEntity(entityToCreate);

            }
            else{
                //Create new character
                entity entityToCreate = makeNewCharacter();
                setPlayerEntity(entityToCreate);
            }

        }
    }
    else {
        //Bad data
        cout << "ERROR";
    }
}
void player::save() {
    //Takes current player entity and outputs it to a file found in entity/data
    ofstream fileOut;
    fileOut.open("../data/entity-data/playerinfo.csv");

    //If file can be opened
    if(fileOut) {
        //Read these values to the output file: name,description,health,experience,alignment
        fileOut <<"saved data:"<<endl
                << this->playerEntity.getName() <<','
                << this->playerEntity.getDescription() <<','
                << this->playerEntity.getHealth() <<','
                << this->playerEntity.getExperience() <<','
                << this->playerEntity.getAlignment();
        //If successful
        cout <<endl <<"Saved!"<<endl;
    }
    else {
        //Otherwise
        cout<<"Error saving data!";
    }

    fileOut.close();

}
/*
 * set method for playerEntity field (has-a entity relationshi)
 *
 * sets the current player entity to a reference entity passed in as a parameter
 */
void player::setPlayerEntity(entity &pEntity) {
    this->playerEntity = pEntity;
}
//Returns a refrence of the current playerEntity
entity& player::getPlayerEntity() {
    return this->playerEntity;
}


