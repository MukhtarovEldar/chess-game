#include "../include/MoveList.hpp"
#include "../include/PieceTransition.hpp"
#include "../include/GameThread.hpp"

MoveList::MoveList(Board& board, PieceTransition& p): game(board), m_transitioningPiece(p) {}

void MoveList::highlightLastMove(RenderWindow& window) const {
    Move& move = *m_moveIterator;

    RectangleShape squareBefore = createSquare();
    RectangleShape squareAfter = createSquare();

    Color colorInit = ((move.getInit().first+move.getInit().second) % 2)
                    ? Color(170, 162, 58): Color(205, 210, 106);
    Color colorTarget = ((move.getTarget().first+move.getTarget().second) % 2)
                    ? Color(170, 162, 58): Color(205, 210, 106);
    squareBefore.setFillColor(colorInit);
    squareAfter.setFillColor(colorTarget);

    squareBefore.setPosition(GameThread::getWindowXPos(move.getInit().first),
        GameThread::getWindowYPos(move.getInit().second));
    squareAfter.setPosition(GameThread::getWindowXPos(move.getTarget().first), 
        GameThread::getWindowYPos(move.getTarget().second));

    window.draw(squareBefore);
    window.draw(squareAfter);
}

void MoveList::goToPreviousMove(bool enableTransition) {
    if (hasMovesBefore()) {
        undoMove(enableTransition);
        ++m_moveIterator; // Go to previous move
    }
}

void MoveList::goToNextMove(bool enableTransition) { 
    if (hasMovesAfter()) {
        --m_moveIterator; // Go to previous move
        applyMove(enableTransition);
    }
}

void MoveList::addMove(Move& move) {
    applyMove(move, true, true);
    m_moveIterator = m_moves.begin();
}

void MoveList::applyMove(Move& move, bool addToList, bool enableTransition) {
    const int castleRow = (game.getTurn() == Team::WHITE)? 7: 0;
    Piece* oldPiece = nullptr;
    Piece* selectedPiece = move.getSelectedPiece();
    Piece* capturedPiece = move.getCapturedPiece();

    int prevX = move.getInit().first;
    int prevY = move.getInit().second;
    int x = move.getTarget().first;
    int y = move.getTarget().second;

    // Set the current tile of the piece null. Necessary for navigating back to current move through goToNextMove()
    game.setBoardTile(prevX, prevY, nullptr); 

    // TODO smooth piece transition for castle 
    switch (move.getMoveType()) {
        case MoveType::NORMAL:
            if (addToList) {
                m_moves.emplace_front(move);
                game.setBoardTile(x, y, selectedPiece);
            }
            // soundMove.play();
            break;

        case MoveType::CAPTURE:
            oldPiece = game.getBoardTile(x, y);
            if (addToList) {
                game.setBoardTile(x, y, selectedPiece);
                m_moves.emplace_front(Move(move, oldPiece));
            }
            // soundCapture.play();
            break;

        case MoveType::ENPASSANT:
            oldPiece = game.getBoardTile(capturedPiece->getY(), capturedPiece->getX()); // The position of the captured pawn
            game.setBoardTile(capturedPiece->getY(), capturedPiece->getX(), nullptr);
            if (addToList) {
                game.setBoardTile(x, y, selectedPiece);
                m_moves.emplace_front(Move(move, oldPiece));
            }
            break;

        case MoveType::CASTLE_KINGSIDE:
            oldPiece = game.getBoardTile(7, castleRow);
            game.setBoardTile(5, castleRow, game.getBoardTile(7, castleRow));
            game.setBoardTile(7, castleRow, nullptr);
            game.setBoardTile(6, castleRow, selectedPiece);
            if (addToList){
                coor2d target = make_pair(6, castleRow);
                move.setTarget(target);
                m_moves.emplace_front(Move(move, oldPiece));
            }    
            break;

        case MoveType::CASTLE_QUEENSIDE:
            oldPiece = game.getBoardTile(7, castleRow);
            game.setBoardTile(3, castleRow, game.getBoardTile(0, castleRow));
            game.setBoardTile(0, castleRow, nullptr);
            game.setBoardTile(2, castleRow, selectedPiece);
            if (addToList) {
                coor2d target = make_pair(2, castleRow);
                move.setTarget(target);
                m_moves.emplace_front(Move(move, oldPiece));
            }
            break;

        case MoveType::INIT_SPECIAL:
            if (addToList) {
                game.setBoardTile(x, y, selectedPiece);
                m_moves.emplace_front(move);
            }
            break;

        case MoveType::NEWPIECE:
            // Possible leaking memory here actually ? 
            oldPiece = game.getBoardTile(x, y);
            Queen* queen = new Queen(game.getTurn(), y, x);
            queen->setLastMovedPiece(selectedPiece->getLastMovedPiece());
            selectedPiece = queen;
            if (addToList) {
                game.setBoardTile(x, y, queen);
                m_moves.emplace_front(Move(move, oldPiece));
            }
            break;
    }
    if(!addToList && selectedPiece != nullptr) {
        if(enableTransition) GameThread::setTransitioningPiece(selectedPiece,
            x * CELL_SIZE, y * CELL_SIZE, getTransitioningPiece()); 
        else game.setBoardTile(x, y, selectedPiece);
    } 
}

void MoveList::applyMove(bool enableTransition) {
    Move& m = *m_moveIterator;
    applyMove(m, false, enableTransition);
}

void MoveList::undoMove(bool enableTransition) {
    Move& m = *m_moveIterator;
    Piece* captured = m.getCapturedPiece();
    int x = m.getTarget().first;
    int y = m.getTarget().second;

    int castleRow = (m.getSelectedPiece()->getTeam() == Team::WHITE)? 7: 0;

    // TODO smooth transition for castle 
    switch (m.getMoveType()) {
        case MoveType::NORMAL:
            game.setBoardTile(x, y, nullptr);
            break;
        case MoveType::CAPTURE:
            game.setBoardTile(x, y, captured);
            break;
        case MoveType::ENPASSANT:
            game.setBoardTile(x, y, nullptr);
            game.setBoardTile(captured->getY(), captured->getX(), captured);
            break;
        case MoveType::CASTLE_KINGSIDE:
            game.setBoardTile(7, castleRow, captured);
            game.setBoardTile(6, castleRow, nullptr);
            game.setBoardTile(5, castleRow, nullptr);
            break;
        case MoveType::CASTLE_QUEENSIDE:
            game.setBoardTile(0, castleRow, captured);
            game.setBoardTile(2, castleRow, nullptr);
            game.setBoardTile(3, castleRow, nullptr);
            break;
        case MoveType::INIT_SPECIAL:
            game.setBoardTile(x, y, nullptr);
            break;
        case MoveType::NEWPIECE:
            // Possible leaking memory here actually ? 
            game.setBoardTile(x, y, captured);
            Pawn* pawn = new Pawn(game.getTurn(), y, x);
            m.setSelectedPiece(pawn);
            break;
    }
    if(enableTransition) GameThread::setTransitioningPiece(
        m.getSelectedPiece(), m.getInit().first * CELL_SIZE, 
        m.getInit().second * CELL_SIZE, getTransitioningPiece()); 
    else 
        game.setBoardTile(m.getInit().first, m.getInit().second, m.getSelectedPiece());
        
}
