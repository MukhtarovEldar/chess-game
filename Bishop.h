#pragma once
#include "Piece.h"


// Represents a bishop
struct Bishop : public Piece {
    Bishop(Team, int, int); // Constructor
    void calcPossibleMoves(Piece*[8][8]) const override; 
};
