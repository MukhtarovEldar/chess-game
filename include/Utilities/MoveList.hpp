#pragma once
#include <list>
#include <iterator>
#include <SFML/Graphics.hpp>
#include "../Components/Board.hpp"
#include "Move.hpp"
#include "PieceTransition.hpp"
#include "MoveTree.hpp"
using namespace sf;


class MoveList
{
public:
    MoveList(Board&, PieceTransition&);

    MoveTree::Iterator getNewIterator() { return m_moves.begin(); }
    MoveTree::Iterator getIterator() { return m_moveIterator; }
    MoveTree getMoves() const { return m_moves; }
    PieceTransition& getTransitioningPiece() const { return m_transitioningPiece; }
    int getIteratorIndex() { return 0; }
    int getMoveListSize() const { return m_moves.getNumberOfMoves(); }

    void highlightLastMove(RenderWindow&);
    void reset() { m_moves.clear(); m_moveIterator = m_moves.begin(); };
    bool goToPreviousMove(bool, vector<Arrow>&);
    bool goToNextMove(bool, vector<Arrow>&);
    void goToCurrentMove(vector<Arrow>& arrowList) { while (goToNextMove(false, arrowList)); }
    void goToInitialMove(vector<Arrow>& arrowList) { while (goToPreviousMove(false, arrowList)); }
    void addMove(shared_ptr<Move>&, vector<Arrow>& arrowList);

private:
    MoveTree m_moves;
    MoveTree::Iterator m_moveIterator = m_moves.begin();
    PieceTransition& m_transitioningPiece;
    Board& game;

    void applyMove(shared_ptr<Move>&, bool, bool, vector<Arrow>&);
    void applyMove(bool, vector<Arrow>&);
    void undoMove(bool, vector<Arrow>&);
};
