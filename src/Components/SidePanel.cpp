#include "../../include/Components/SidePanel.hpp"

SidePanel::SidePanel(RenderWindow& window_, MoveList& moveList_, MoveTree& moveTree_, bool& b_)
: m_window(window_), m_moveList(moveList_), 
m_moveTree(moveTree_), m_showMoveSelectionPanel(b_) {}; 

void SidePanel::addMove(Move& move_) {
    // get the text coordinates information for a Move Box
    int moveListSize = m_moveList.getMoveListSize();
    int moveNumber = (moveListSize / 2) + 1;
    bool showNumber = moveListSize % 2 != 0;
    string text = parseMove(move_, moveNumber, showNumber);

    // construct the Move Box
    MoveBox moveBox(m_nextPos, text); // Make the text box
    moveBox.handleText(); // Create the Text, and pass the font ressource
    checkOutOfBounds(moveBox, 0); // check if object's width goes out of bounds and update
    m_nextPos.first += (moveBox.getScaledWidth()); // increment for next move box

    moveBoxes.emplace_back(moveBox);
    ++moveBoxCounter;
}

pair<char,int> SidePanel::findLetterCoord(coor2d target_) const {
    char letter = letters.at(target_.first);
    return make_pair(letter, 8-target_.second);
}

string SidePanel::parseMove(Move& move_, int moveNumber_, bool showNumber_, bool showDots_) const {
    string text = (showNumber_)
        ? to_string(moveNumber_) + "." 
        : (showDots_)? to_string(moveNumber_-1) + "..." : "";
    MoveType moveType = move_.getMoveType();

    // side cases
    if (moveType == MoveType::CASTLE_KINGSIDE) {
        return text + "O-O";
    }
    if (moveType == MoveType::CASTLE_QUEENSIDE) {
        return text + "O-O-O";
    }

    coor2d coord = move_.getTarget();
    pair<char, int> letterCoord = findLetterCoord(coord);
    string letterCoordString = (1, letterCoord.first) + to_string(letterCoord.second);;

    switch (move_.getSelectedPiece()->getType()) {
        case PieceType::PAWN:
            // TODO fix promotion
            if(moveType == MoveType::CAPTURE || moveType == MoveType::ENPASSANT){
                text += string(1, letters.at(coord.first-1)) + "x";
                return text + letterCoordString;
            } break;
        case PieceType::KNIGHT: text += "N"; break;
        case PieceType::BISHOP: text += "B"; break;
        case PieceType::ROOK: text += "R"; break;
        case PieceType::QUEEN: text += "Q"; break;
        case PieceType::KING: text += "k"; break;
    }
    return text + string((moveType == MoveType::CAPTURE)? "x" : "") + letterCoordString;
}

void SidePanel::goToNextRow(int height_) {
     m_nextPos = {BORDER_SIZE + 10, m_nextPos.second + height_ + 20};
}

void SidePanel::checkOutOfBounds(MoveBox& moveBox_, int offset_) {
    int newPos = WINDOW_SIZE + offset_ + m_nextPos.first + moveBox_.getTextBounds().width;
    if (newPos >= WINDOW_SIZE + PANEL_SIZE - BORDER_SIZE) {
        goToNextRow(moveBox_.getTextBounds().height); // change next position 
        moveBox_.setPosition(m_nextPos); // update the new position
    }
}

void SidePanel::handleMoveBoxClicked(coor2d& mousePos_) const{
    int newMoveIndex = 0;

    for(auto& moveBox : moveBoxes) {
        float width = moveBox.getScaledWidth();
        float height = moveBox.getScaledHeight();
        int xPos = moveBox.getPosition().first;
        int yPos = moveBox.getPosition().second;

        int x = mousePos_.first - WINDOW_SIZE; 
        int y = mousePos_.second - MENUBAR_HEIGHT;
        
        if ((x >= xPos && x < xPos + width) && (y >= yPos && y < yPos + height)) {
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

void SidePanel::drawSquareBracket(coor2d& nextPos_, int offset_, bool open_) const {
    nextPos_.first += offset_; 

    shared_ptr<Font> font =  RessourceManager::getFont("Arial.ttf");
    Text textsf;
    textsf.setString(open_ ? "[" : "]");
    textsf.setFont(*font);
    textsf.setCharacterSize(28);
    textsf.setStyle(Text::Bold);
    textsf.setFillColor({240, 248, 255});

    Vector2f recSize(textsf.getGlobalBounds().width, textsf.getCharacterSize());
    RectangleShape rect;
    rect.setPosition(WINDOW_SIZE + nextPos_.first, MENUBAR_HEIGHT + nextPos_.second);
    rect.setSize(recSize);
    rect.setFillColor({50, 50, 50});

    float positionalShift = ((BOX_HORIZONTAL_SCALE - 1.f) * rect.getLocalBounds().width)/2.f;
    rect.setScale(BOX_HORIZONTAL_SCALE, BOX_VERTICAL_SCALE);
    textsf.setPosition(WINDOW_SIZE + nextPos_.first + positionalShift,
                         MENUBAR_HEIGHT + nextPos_.second - 4);

    m_window.draw(rect);
    m_window.draw(textsf);
    nextPos_.first -= offset_;
}

void SidePanel::drawFromNode(MoveTreeNode*& node_, int level_, int offset_, coor2d nextPos_, coor2d& mousePos) {
    // base case leaf
    if (node_->m_move.get() == nullptr && node_->m_parent != nullptr) {
        // close the open bracket for sub variation
        if (offset_ != 0) drawSquareBracket(nextPos_, offset_ + 10 , false);
        return;
    }

    // draw the node
    if (node_->m_move.get() != nullptr) {
        nextPos_ = drawMove(*(node_->m_move), level_, offset_, nextPos_, mousePos);
    }   

    // reccursive step
    for (int i = 0; i < node_->childNumber; ++i) {
        if (i == 0) drawFromNode(node_->m_children.at(0), level_+1, offset_, nextPos_, mousePos);
        else {
            ++m_row;
            nextPos_ = {INIT_WIDTH, INIT_HEIGHT + (m_row * ROW_HEIGHT)};
            drawSquareBracket(nextPos_, offset_ + HORIZONTAL_OFFSET - 10, true);
            drawFromNode(node_->m_children.at(i), level_+1,
                         offset_+HORIZONTAL_OFFSET, nextPos_, mousePos);  
        }
    }
}

coor2d SidePanel::drawMove(Move& move_, int level_, int offset_, coor2d nextPos_, coor2d& mousePos_) {
    // iterate through all the move list from begining to end
    // get the text coordinates information for a Move Box
    int moveNumber = (level_ / 2) + 1;
    bool showNumber = level_ % 2 != 0;
    string text = parseMove(move_, moveNumber, showNumber);

    // construct the Move Box
    nextPos_.first += offset_; 
    MoveBox moveBox(nextPos_, text); // Make the text box
    moveBox.handleText(); // Create the Text, and pass the font ressource

    // checkOutOfBounds(moveBox, offset); // check if object's width goes out of bounds and update
    // m_nextPos.first += (moveBox.getScaledWidth()); // increment for next move box

    moveBox.handleRectangle();
    if (!m_showMoveSelectionPanel) {
        // Change the color of the Move Box if it is howered
        if (moveBox.isHowered(mousePos_)) moveBox.setIsSelected(); 
        else moveBox.setDefault();
    }
    m_window.draw(moveBox.getRectangle());
    m_window.draw(moveBox.getTextsf());

    return {nextPos_.first += (moveBox.getScaledWidth()-offset_), nextPos_.second};
}

void SidePanel::drawMoves(coor2d& mousePos_) {
    MoveTreeNode* root = m_moveTree.getRoot();
    drawFromNode(root, 0, 0, {INIT_WIDTH, INIT_HEIGHT}, mousePos_); 
    m_row = 0; // reset to 0 for next iteration 
    // if (moveBoxes.size() == 0) return; // no moves added yet, return

    // int counter = 0;
    // for (auto& moveBox : moveBoxes) {
    //     moveBox.handleRectangle();
        
    //     if (!m_showMoveSelectionPanel) {
    //         // Change the color of the Move Box if it is howered
    //         if (moveBox.isHowered(mousePos_)) moveBox.setIsSelected(); 
    //         else moveBox.setDefault();
    //     }

    //     // Change the color of te Move Box if it is represents the current move
    //     int currMoveIndex = m_moveList.getMoveListSize() - m_moveList.getIteratorIndex() -1;
    //     if (counter == currMoveIndex) moveBox.setIsCurrentMove();

    //     ++counter;
    //     m_window.draw(moveBox.getRectangle());
    //     m_window.draw(moveBox.getTextsf());
    // }
    // resetNextPos();
}

