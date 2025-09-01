//
// Created by beau on 10/6/2024.
//

#include "enemy.h"

#include <complex>
#include <fstream>
#include <memory>

/*
 *Enemy base constructor, calls parent constructor, then overriden generateEntity();
 *sets fPower to the enemies experience divided by 10
 */

enemy::enemy(){
    this->generateEntity();
    fPower = (this->fExperience) / 10;

}

//Calls generate entity parent class with specific filepath in mind,
/*
 * This void type method creates a random entity using data/entity-data/enemy_creatureinfo.csv.
 *
 * Firstly, the amount of creatures in the csv file is hardcoded as a constant value for easier looping.
 *
 * the creatureinfo.csv file is loaded into an ifStream file, and a random line is chosen to generate a creature,
 * these values are either read in from getline() or the extractor operator >> and assigned to "this" object via
 * previously defined get and set methods.
 *
 * This overrides the entity.cpp implementation as those two either return a specific entity or a null value (see parent class for idea)
 */
void enemy::generateEntity() {
    const int NUM_CREATURES = 52;
    ifstream fileIn;
    fileIn.open("../game/entity-data/enemy_creatureinfo.csv");
    string line,name,description = "nil";
    float health,experience = -1.0;
    int alignment = -2;

    if(fileIn) {
        srand(time(NULL));
        int randomNumber = rand()%NUM_CREATURES;
        char comma = ',';

        for(int i = 0 ; i<randomNumber; ++i) {
            getline(fileIn,line,'\n');
        }

        //Reading input from filein
        getline(fileIn, name, comma);
        getline(fileIn, description, comma);
        fileIn>>health;
        fileIn>>comma;
        fileIn>>experience;
        fileIn>>comma;
        fileIn>>alignment;
        //Setting input with read values
        this->setName(name);
        this->setDescription(description);
        this->setHealth(health);
        this->setExperience(experience);
        this->setAlignment(alignment);
    }
    else {
        std::cout << "error";
    }
}
/*
 * Helper method for attackAgainst()
 */
string attack(entity &pTarget,int pLowerBy) {
    //Lowers reference entity's health by specific int value
    pTarget.setHealth(pTarget.getHealth()-pLowerBy);
    return "" + to_string(pLowerBy) + " damage!";
}
/*
 * attackAgainst(entity &pTarget) takes in an entity reference to attack.
 *
 * Firstly, it checks if the status of the entity is set to "Defending" or not (Enemy cannot attack defending target),
 * if so, simulates a random dice roll from a d20, and does dammage according to if it's a critical hit,failure, or normal attack
 *
 * Otherwise, a target is defending, so enemy will try to break the defense with a 1/10 chance of success.
 */
string enemy::attackAgainst(entity &pTarget) {
    //random seed at current time for later rand use
    srand(time(NULL));
    string str;
    str = this->getName() + " is attacking!\n";
    if(pTarget.getStatus() != "Defending") {
        //Target is not defending

        //Simulate rolling a 20 sided dice
        int random = rand()%20;

        //If 20 was rolled or if the player character is prone returns 0 so !0 is 1
        if(random == 0 || pTarget.getStatus() == "Prone") {
            //If 20, roll a critical success
            str = str + "\nCritical Hit!\n" + attack(pTarget,fPower* (rand()%2)+1);

        }
        else if (random == 1 ) {
            //If 1, (1%20 = 1) roll a critical fail
            str = str + "\nCritical Failure!\n"+attack(pTarget,fPower*.1);
        }
        else {
            //Normal attack
            str = str +this->getName() + " Attacks!\n"+ attack(pTarget,fPower);
        }
        this->setStatus("Attacking");
    }
    else {
        //Target is defending, 1/10 chance that an attack will break defense, damaging an enemy and leaving them prone

        if(rand()%10 == 0 ) {
            //Defense has been broken
            str = str + pTarget.attackAgainst(pTarget, (rand()%100)/100);
            this->setStatus("Attacking");
            pTarget.setStatus("Prone");
        }
        else
            //Still kicking
            str = str +  "Defended attack succesfully.";
    }
    return str;
}
/*
 * defendAgainst(entity &pTarget) takes in an entity reference to defend against.
 *
 * There is a 1% chance that a defense can be broken randomly, which causes the creature to be exposed.
 * Instead, there is a successful change to the entity's status set as "Defending"
 *
 * Overrides parent method
 */
string enemy::defendAgainst(entity &pTarget) {
    srand(time(NULL));
    //1% chance defense fails
    string str;
    str = this->getName() + " is defending!";
    if(rand()%100 == 0) {
        str + "\nFailed to defend! Creature is now exposed";
        this->setStatus("Prone");
    }
    else {
        this->setStatus("Defending");
    }
    return str;
}

/*
 * bool move_against(entity &pTarget) uses the two methods attackAgainst() and defendAgainst()
 * with various conditionals to decide the 'optimal' move for the enemy,
 *
 * returns true if enemy attacks, and false otherwise (Defense)
 */
string enemy::move_against(entity &pTarget) {
    srand(time(NULL));
    string str = "";
    //If the target is prone, always attack
    if(pTarget.getStatus()=="Prone"){
           str + attackAgainst(pTarget);
       return str;
    }
    //Else check health if under half of base health
    if(this->getHealth() < this->getBaseHealth()/2) {
        //Then 50/50 chance of attacking/defending
        if(rand()%2 == 0) {
            str = this->attackAgainst(pTarget);
            return str;
        }
        else {
            str = this->defendAgainst(pTarget);
            return str;
        }
    }

    //Else 75/25 chance to attack or defend
    if(rand()% 4 != 0 ) {
        str = this->attackAgainst(pTarget);
        return str;
    }
    else {
        str = this->defendAgainst(pTarget);
        return str;
    }
    return str;
}



