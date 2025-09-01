//
// Created by beau on 9/15/2024.
//

#ifndef GAME_H
#define GAME_H

#include <string>
#include <iostream>
using namespace std;

class entity {
    /*
     * An entity is an object that represents a living thing or 'creature'
     *
     * Entities have the fields
     * health
     * level
     * name
     * description
     * alignment
     */
public:
    //constructors

    entity();
    entity(float pHealth, float pExperience, string pName, string pDescription, int pAlignment);

    //Method declaration for later implementation in game.cpp
    //Using descriptive method names and method parameters.
    float getHealth();
    void setHealth(float pHealth);

    float getBaseHealth();
    void setBaseHealth(float pHealth);

    int getExperience();
    void setExperience(int pExperience);
    void addExperience(int pExperience);

    string getName();
    void setName(string pName);
    string getDescription();
    void setDescription(string pDescription);

    int getAlignment();
    void setAlignment(int pAlignment);

    void lowerHealth(int pAttackPower);
    //Virtual method to later be overriden
    virtual void generateEntity();
    //Base generate entity for specificity
    void generateEntity(float pHealth, float pExperience, string pName, string pDescription, int pAlignment);


    string getStatus();
    void setStatus(string pStatus);

    //Later methods to be implemented by child classes
    string defendAgainst(entity &pTarget);
    string attackAgainst(entity &pTarget, int pLowerBy);

    //friend for overloaded operator
    friend ostream& operator << (ostream& out, entity& entity) {
        /*
         * Overloaded operator that reads a creature and it's information to the given output stream.
         *
         * Declared in the header file because game.cpp would not recognize "friend" keyword in any context
         */
        out << "Name: " << entity.getName() << endl;
        out << "Description: " << entity.getDescription() << endl;
        out << "Health: " << entity.getHealth() << endl;
        out << "Experience: " << entity.getExperience() << endl;
        out << "Alignment: " << entity.getAlignment() << endl;
        return out;
    }

protected:

    /*
     *  Private fields for entity class. Self explanatory,
     *  Health represents the entity's total health, and is any float greater than zero
     *  Level is the entity level. This was not implemented properly.
     *  Name is the name of the entity/creature.
     *  Description is a sentence describing the entity/creature.
     *
     *  All of these have related values in data/entity-data/creatureinfo.csv
     */
    float fHealth;
    float fBaseHealth;
    int fLevel;
    int fExperience;
    string fName;
    string fDescription;
    //alignment is represented by -1 (evil) 0 (neutral) 1 (good)
    int fAlignment;
    string fStatus;


};

#endif //GAME_H
