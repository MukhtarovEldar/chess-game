#include "../../../include/Logic/Pieces/Bishop.hpp"

// Index-based coordinates constructor
Bishop::Bishop(Team team_, int file_, int rank_):
    Piece(team_, file_, rank_, PieceType::BISHOP, "b")
{
}

// Real coordinates constructor 
Bishop::Bishop(Team team_, coor2dChar& coords_):
    Piece(team_, coords_.first, coords_.second, PieceType::BISHOP, "b")
{
}

std::vector<Move> Bishop::calcPossibleMoves(Board& board_) const
{
    std::vector<Move> moves;
    addDiagonalMovements(board_, moves);
    return moves;
}
