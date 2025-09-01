//
// Created by beau on 10/6/2024.
//

#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"

//Class enemy is-a entity, inherits parent class
class enemy:public entity
{
	public:
	//Constructor for child class, will use default parent
	enemy();
	//Overriding generateEntity() for polymorphism
	void generateEntity() override;

	//Actions to be called in move_against()
	string attackAgainst(entity &pTarget);
	string defendAgainst(entity &pTarget);

	//This will be a move taken against a pTarget refernce
	string move_against(entity &pTarget);

protected:

	//For new attacking methods
	float fPower;

};



#endif //ENEMY_H
