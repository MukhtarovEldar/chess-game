#pragma once

#include "../Components/Board.hpp"
#include "Move.hpp"
#include "MoveTreeDisplayHandler.hpp"
#include "PieceTransition.hpp"
#include "MoveTree.hpp"

#include <list>
#include <functional>
#include <iterator>
#include <SFML/Graphics.hpp>

using namespace sf;

class MoveList
{
public:
    explicit MoveList(Board&);

    MoveTree::Iterator getNewIterator() { return m_moves.begin(); }
    MoveTree::Iterator& getIterator() { return m_moveIterator; }
    const MoveTree& getMoves() const { return m_moves; }
    MoveTreeDisplayHandler& getMoveTreeDisplayHandler() { return m_moveTreeDisplayHandler; }
    PieceTransition& getTransitioningPiece() { return m_transitioningPiece; }
    int getIteratorIndex() { return 0; }
    int getMoveListSize() const { return m_moves.getNumberOfMoves(); }

    void reset() { m_moves.clear(); m_moveIterator = m_moves.begin(); };
    bool goToPreviousMove(bool, vector<Arrow>&);
    bool goToNextMove(bool, const std::optional<size_t>&, vector<Arrow>&);
    void goToCurrentMove(vector<Arrow>& arrowList) { while (goToNextMove(false, std::nullopt, arrowList)); }
    void goToInitialMove(vector<Arrow>& arrowList) { while (goToPreviousMove(false, arrowList)); }
    void addMove(shared_ptr<Move>&, vector<Arrow>& arrowList);

    bool isTransitionningPiece() { return m_transitioningPiece.getIsTransitioning(); }
    void setTransitioningPieceArrived() { m_transitioningPiece.setHasArrived(); }

private:
    MoveTree m_moves;
    MoveTreeDisplayHandler m_moveTreeDisplayHandler{m_moves};
    MoveTree::Iterator m_moveIterator = m_moves.begin();
    Board& game;
    PieceTransition m_transitioningPiece;

    std::function<void(std::shared_ptr<Piece>&, const coor2d&, bool)> m_transitionPieceCallback;

    void applyMove(shared_ptr<Move>&, bool, bool, vector<Arrow>&);
    void applyMove(bool, vector<Arrow>&);
    void undoMove(bool, vector<Arrow>&);

    void setTransitioningPiece(
        bool, shared_ptr<Piece>&, int, int, int, int,
        shared_ptr<Piece>&, int, int
    );

    void setSecondTransitioningPiece(
        shared_ptr<Piece>&, int, int,
        int, int
    );
};
