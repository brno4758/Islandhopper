#ifndef PLAYER_H
#define PLAYER_H
#include "grid.h"

#define WALLS   4
class Player
{
public:
    Player(Cell* startingCell) : score_(0), wallCount_(WALLS), currCell_(startingCell){};

    //Increments the current players score_ value by the value passed as argument
    void increment_score(short value) {score_ += value;}

    //Decrements the current players score_ value by the value passed as argument
    void decrement_score(short value) {score_ -= value;}

    //Returns the current players current score_
    short get_score() const {return score_;}

    //Returns the current players current number of walls available
    short get_wall_count() const {return wallCount_;}

    //Returns pointer to the current players Cell in the Grid
    Cell*& get_curr_cell() {return currCell_;}

    //Sets the current players currCell_ pointer to the Cell* passed as argument
    void set_curr_cell(Cell* cell) {currCell_ = cell;}

    //Increments the current players number of available walls
    void increment_wall_count() {wallCount_++;}

    //Decrements the current players number of available walls
    void decrement_wall_count() {wallCount_--;}

protected:
    short score_;
    short wallCount_;
    Cell* currCell_;
};

#endif // PLAYER_H
