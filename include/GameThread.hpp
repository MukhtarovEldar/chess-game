#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ressources/RessourceManager.hpp"
#include "Utilities/PieceTransition.hpp"
#include "Components/MenuButton.hpp"
#include "Components/Board.hpp"
#include "Utilities/Arrow.hpp"

using namespace sf;

class SidePanel; // Forward declaration

// Constants
constexpr uint32_t MENUBAR_HEIGHT = 30;
constexpr uint32_t NUMBER_BUTTONS = 6;
constexpr uint32_t WINDOW_SIZE = 640;
constexpr uint32_t CELL_SIZE = WINDOW_SIZE / 8;
constexpr uint32_t BUTTON_POS = WINDOW_SIZE / NUMBER_BUTTONS;
constexpr uint32_t PANEL_SIZE = 640;
constexpr uint32_t BORDER_SIZE = 10;
constexpr uint32_t SOUTH_PANEL_HEIGHT = MENUBAR_HEIGHT;
constexpr uint32_t MAIN_PANEL_HEIGHT = PANEL_SIZE - SOUTH_PANEL_HEIGHT;
constexpr float SPRITE_SCALE = 0.6;
constexpr float SPRITE_SIZE = 128;
constexpr float BUTTON_SIZE = 40;

// Utility function
inline RectangleShape createSquare() {
    return RectangleShape({CELL_SIZE, CELL_SIZE});
}

class GameThread {
    inline const static string iconsPath = "../assets/icons/";
    inline const static string audioPath = "../assets/sounds/";

    inline static Board game;
    inline static RenderWindow window = {VideoMode(WINDOW_SIZE + PANEL_SIZE, WINDOW_SIZE + MENUBAR_HEIGHT),
                                        "Chess Game", Style::Titlebar | Style::Close};
    inline static vector<MenuButton> menuBar;
    inline static vector<Move> possibleMoves;

    static void initializeBoard();
    static void initializeMenuBar();
    static void drawSidePanel(SidePanel&);
    static void drawCaptureCircles(Piece*);
    static void highlightHoveredSquare(Piece*, coor2d&);
    static void drawMenuBar();
    static void drawPieces();
    static void drawDraggedPiece(Piece*, coor2d&);
    static void drawTransitioningPiece(PieceTransition&);
    static void drawAllArrows(vector<Arrow>&, Arrow&);
    static void drawEndResults();
    static void drawKingCheckCircle();

    inline static bool isFlipped = false;

    public:
    GameThread() = delete; // Delete constructor
    static void startGame();

    static int getTileXPos(coor2d& pos) { return pos.first / CELL_SIZE; }
    static int getTileYPos(coor2d& pos) {
        if (pos.second < MENUBAR_HEIGHT) return -1;
        return (pos.second - MENUBAR_HEIGHT) / CELL_SIZE;
    }
    static int getWindowXPos(int i) { return i * CELL_SIZE; }
    static int getWindowYPos(int j) { return j * CELL_SIZE + MENUBAR_HEIGHT; }

    static string getIconPath(const string& filename) { return iconsPath + filename; }
    static string getAudioPath(const string& filename) { return audioPath + filename; }
    static void setTransitioningPiece(Piece*, int,int, PieceTransition&);

    static void flipBoard() { isFlipped = !isFlipped; }
    static bool boardFlipped() { return isFlipped; }
};
