#pragma once
#include "Piece.h"


// Represents a king
struct King : public Piece {
    King(Team, int, int); // Constructor
    void calcPossibleMoves(Piece*[8][8]) const override; 
};
