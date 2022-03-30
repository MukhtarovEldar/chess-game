#pragma once

#include <SFML/Graphics.hpp>
#include "../Utilities/Move.hpp"
#include "../Pieces/Piece.hpp"
#include "../GameThread.hpp"
#include "../Utilities/MoveList.hpp"
#include "../Utilities/MoveTree.hpp"
#include "MoveBox.hpp"

using namespace std;
using namespace sf;

inline constexpr int INIT_WIDTH = BORDER_SIZE + 10;
inline constexpr int INIT_HEIGHT = 10;
inline const string letters = "abcdefgh";
inline vector<MoveBox> moveBoxes;


class SidePanel {
    RenderWindow& m_window;
    MoveList& m_moveList;
    MoveTree& m_moveTree;
    coor2d m_nextPos = {BORDER_SIZE + 10, 10};
    int moveBoxCounter = 0;
    int m_row = 0;
    bool& m_showMoveSelectionPanel;

    public:
    SidePanel(RenderWindow&, MoveList&, MoveTree&, bool&);

    pair<char,int> findLetterCoord(coor2d) const;
    string parseMove(Move&, int, bool, bool = false) const;
    void resetNextPos() { m_nextPos = {BORDER_SIZE + 10, 10}; }
    void goToNextRow(int height);
    void addMove(Move&); 
    void drawMoves(coor2d&);
    coor2d drawMove(Move&, int, int, coor2d);
    void checkOutOfBounds(MoveBox&, int);
    void handleMoveBoxClicked(coor2d&) const;
    void drawFromNode(MoveTreeNode*&, int, int, coor2d);
};