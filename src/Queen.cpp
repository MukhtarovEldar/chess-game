#include "../include/Queen.hpp"
#include <vector>

Queen::Queen(Team team, int x, int y): Piece(team, x, y, PieceType::QUEEN, "q") {}

vector<Move> Queen::calcPossibleMoves(Piece* board[8][8]) const {
    // Combine horizontal, vertical and diagonal movements
    vector<Move> moves = getHorizontalAndVerticalMovements(board);
    for (auto& move: getDiagonalMovements(board))
        moves.push_back(move);
    return moves;
}
