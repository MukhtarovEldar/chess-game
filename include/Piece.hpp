#pragma once
#include <utility>
#include <vector>
#include <tuple>
#include <iostream>
using namespace std;


enum class Team { WHITE, BLACK };
enum class PieceType { PAWN, ROOK, KNIGHT, BISHOP, KING, QUEEN, EMPTY };
enum class MoveType { NORMAL, CASTLE_KINGSIDE, CASTLE_QUEENSIDE, ENPASSANT, NEWPIECE, CAPTURE, INIT_SPECIAL };


typedef tuple<pair<int, int>, MoveType> moveType;
typedef vector<moveType> moveTypes;


class Piece {
    inline static Piece* lastPiece = nullptr; // Last moved piece
    string filename; // the filename for this piece
    Team team; // the team this piece plays for
    PieceType type; // Returns the type of this piece
    MoveType lastMove; // Returns the move type of this piece
    int xPos; int yPos; // X and Y positions
    bool moved = false; // Whether piece has moved or not

    public:
    Piece(Team, int, int, PieceType, string); // Constructor
    virtual ~Piece() {} // Virtual destructor

    Team getTeam() const { return team; };
    PieceType getType() const { return type; }
    string getFileName() const { return filename; }
    MoveType getLastMove() const { return lastMove; }
    void setLastMove(MoveType newMove) { lastMove = newMove; }

    static Piece* getLastMovedPiece() { return lastPiece; }
    static void setLastMovedPiece(Piece* piece) { lastPiece = piece; }

    bool hasMoved() const { return moved; }
    int getX() const { return xPos; }
    int getY() const { return yPos; }

    virtual moveTypes calcPossibleMoves(Piece*[8][8]) const = 0; // Pure virtual function
    moveTypes getHorizontalAndVerticalMovements(Piece*[8][8]) const;
    moveTypes getDiagonalMovements(Piece*[8][8]) const;
    void move(int x, int y, bool record = true) {
        if (record && (xPos != x || yPos != y)) moved = true;
        xPos = x;
        yPos = y;
    }
};
