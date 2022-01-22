#pragma once
#include <list>
#include "Piece.hpp"
#include "Pawn.hpp"
#include "Rook.hpp"
#include "Knight.hpp"
#include "Bishop.hpp"
#include "King.hpp"
#include "Queen.hpp"
#include "Move.hpp"

constexpr uint32_t WINDOW_SIZE = 640;
constexpr uint32_t CELL_SIZE = WINDOW_SIZE / 8;

class Board {
    Piece* m_board[8][8];
    Team m_turn; // which player's turn
    bool m_isFlipped = false;
    vector<Piece*> m_whitePieces;
    vector<Piece*> m_blackPieces;
    King* m_whiteKing;
    King* m_blackKing;
    void freeMemory();

    public:
    Board(): m_turn(Team::WHITE) { reset(); } // Constructor
    ~Board() { freeMemory(); } // Destructor

    void reset(); // Resets the board
    Piece* getBoardTile(int x, int y) const { return m_board[y][x]; }
    void setBoardTile(int, int, Piece*, bool record = true);

    bool isFlipped() { return m_isFlipped; };

    Team getTurn() const { return m_turn; }
    void switchTurn() { m_turn = (m_turn == Team::WHITE)? Team::BLACK: Team::WHITE; }

    bool kingIsChecked() { 
        return (m_turn == Team::WHITE)? m_whiteKing->isChecked(m_board): m_blackKing->isChecked(m_board); 
    }

    moveTypes possibleMovesFor(Piece* piece) { return piece->calcPossibleMoves(m_board); }

    void addPiece(Piece* piece) { 
        (piece->getTeam() == Team::WHITE) ? m_whitePieces.push_back(piece) : m_blackPieces.push_back(piece); 
    }
    void flipBoard();
};
