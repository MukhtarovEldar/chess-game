#include "../../include/Components/SidePanel.hpp"

void SidePanel::addMove(Move& move) {
    // get the text coordinates information for a Move Box
    int moveListSize = m_moveList.getMoveListSize();
    int moveNumber = (moveListSize / 2) + 1;
    bool showNumber = moveListSize % 2 != 0;
    string text = parseMove(move, moveNumber, showNumber);

    // construct the Move Box
    MoveBox moveBox(m_nextPos, text); // Make the text box
    moveBox.handleText(); // Create the Text, and pass the font ressource
    checkOutOfBounds(moveBox); // check if object's width goes out of bounds and update
    m_nextPos.first += (moveBox.getScaledWidth()); // increment for next move box

    moveBoxes.emplace_back(moveBox);
    ++moveBoxCounter;
}

pair<char,int> SidePanel::findLetterCoord(coor2d target) const {
    char letter = letters.at(target.first);
    return make_pair(letter, 8-target.second);
}

string SidePanel::parseMove(Move& move, int moveNumber, bool showNumber, bool showDots) const {
    string text = (showNumber)
        ? to_string(moveNumber) + ". " 
        : (showDots)? to_string(moveNumber-1) + "... " : " ";
    MoveType moveType = move.getMoveType();

    // side cases
    if (moveType == MoveType::CASTLE_KINGSIDE) {
        return text + "0-0";
    }
    if (moveType == MoveType::CASTLE_QUEENSIDE) {
        return text + "0-0-0";
    }

    coor2d coord = move.getTarget();
    pair<char, int> letterCoord = findLetterCoord(coord);
    string letterCoordString = (1, letterCoord.first) + to_string(letterCoord.second);;

    switch (move.getSelectedPiece()->getType()) {
        case PieceType::PAWN:
            // TODO fix promotion
            if(moveType == MoveType::CAPTURE || moveType == MoveType::ENPASSANT){
                text += string(1, letters.at(coord.first-1)) + "x";
                return text + letterCoordString;
            }
            break;
        case PieceType::KNIGHT:
            text += "N";
            break;
        case PieceType::BISHOP:
            text += "B";
            break;
        case PieceType::ROOK:
            text += "R";
            break;
        case PieceType::QUEEN:
            text += "Q";
            break;
        case PieceType::KING:
            text += "k";
            break;
    }
    return text + string((moveType == MoveType::CAPTURE)? "x" : "") + letterCoordString;
}

void SidePanel::goToNextRow(int height) {
     m_nextPos = {BORDER_SIZE + 10, m_nextPos.second + height + 20};
}

void SidePanel::checkOutOfBounds(MoveBox& moveBox) {
    int newPos = WINDOW_SIZE + m_nextPos.first + moveBox.getTextBounds().width;
    if (newPos >= WINDOW_SIZE + PANEL_SIZE - BORDER_SIZE) {
        goToNextRow(moveBox.getTextBounds().height); // change next position 
        moveBox.setPosition(m_nextPos); // update the new position
    }
}

void SidePanel::handleMoveBoxClicked(coor2d& mousePos) const{
    int newMoveIndex = 0;

    for(auto& moveBox : moveBoxes) {
        float width = moveBox.getScaledWidth();
        float height = moveBox.getScaledHeight();
        int xPos = moveBox.getPosition().first;
        int yPos = moveBox.getPosition().second;

        int x = mousePos.first - WINDOW_SIZE; 
        int y = mousePos.second - MENUBAR_HEIGHT;
        
        if ((x >= xPos && x < xPos + width) && 
            (y >= yPos && y < yPos + height)) {
            int currMoveIndex = m_moveList.getMoveListSize() - m_moveList.getIteratorIndex() -1;
            vector<Arrow> temp{}; // for testing 
            if (newMoveIndex > currMoveIndex) {
                while (newMoveIndex > currMoveIndex) {
                    m_moveList.goToNextMove(false, temp);
                    --newMoveIndex;
                }
            } else if (newMoveIndex < currMoveIndex) {
                while (newMoveIndex < currMoveIndex) {
                    m_moveList.goToPreviousMove(false, temp);
                    ++newMoveIndex;
                }
            } 
            break;
        }
        ++newMoveIndex;
    }
}

void SidePanel::drawMoves(coor2d& mousePos) const {
    if (moveBoxes.size() == 0) return; // no moves added yet, return

    int counter = 0;
    for (auto& moveBox : moveBoxes) {
        moveBox.handleRectangle();
        
        if (!m_showMoveSelectionPanel) {
            // Change the color of the Move Box if it is howered
            if (moveBox.isHowered(mousePos)) moveBox.setIsSelected(); 
            else moveBox.setDefault();
        }

        // Change the color of te Move Box if it is represents the current move
        int currMoveIndex = m_moveList.getMoveListSize() - m_moveList.getIteratorIndex() -1;
        if (counter == currMoveIndex) moveBox.setIsCurrentMove();

        ++counter;
        m_window.draw(moveBox.getRectangle());
        m_window.draw(moveBox.getTextsf());
    }
    // resetNextPos();
}

