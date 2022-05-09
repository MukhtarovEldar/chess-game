#pragma once
#include "Piece.hpp"

// Represents a knight
struct Knight: public Piece
{
    Knight(Team, int, int); // Constructor
    vector<Move> calcPossibleMoves(Board&) const override;
};
