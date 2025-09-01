//
// Created by beau on 10/6/2024.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <vector>

#include "entity.h"


class player {

public:
    //player constructor loads playerEntity with information found in entity-data
    player();

    //Sets the playerEntity to a new entity reference
    void setPlayerEntity(entity &pEntity);
    //Returns reference of playerEntity
    entity& getPlayerEntity();

    //makePlayer is incorporated in player() constructor
    void makePlayer();
    //Saves player data to separate csv
    void save();

protected:
    //Entity that the player controls
    entity playerEntity;
};



#endif //PLAYER_H
