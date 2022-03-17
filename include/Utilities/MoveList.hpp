#pragma once
#include <list>
#include <iterator>
#include <SFML/Graphics.hpp>
#include "../Components/Board.hpp"
#include "Move.hpp"
#include "PieceTransition.hpp"
using namespace sf;


class MoveList {
    list<Move> m_moves;
    list<Move>::iterator m_moveIterator = m_moves.begin();
    PieceTransition& m_transitioningPiece;
    Board& game;

    void applyMove(Move&, bool, bool, vector<Arrow>&); // called inside GameThread ?
    void applyMove(bool, vector<Arrow>&); 
    void undoMove(bool, vector<Arrow>&);

    public:
    MoveList(Board&, PieceTransition&);
    PieceTransition& getTransitioningPiece() { return m_transitioningPiece; }
    int getIteratorIndex() { return std::distance(m_moves.begin(), m_moveIterator); }
    int getMoveListSize() { return m_moves.size(); }
    void highlightLastMove(RenderWindow&) const;
    void reset() { m_moves.clear(); m_moveIterator = m_moves.begin(); };
    void goToPreviousMove(bool, vector<Arrow>&);
    void goToNextMove(bool, vector<Arrow>&);
    void goToCurrentMove(vector<Arrow>& arrowList) { while(hasMovesAfter()) goToNextMove(false, arrowList); }
    void goToInitialMove(vector<Arrow>& arrowList) { while(hasMovesBefore()) goToPreviousMove(false, arrowList); }
    void addMove(Move&, vector<Arrow>& arrowList);
    bool hasMovesBefore() const { return m_moveIterator != m_moves.end(); }
    bool hasMovesAfter() const { return m_moveIterator != m_moves.begin(); }
};
