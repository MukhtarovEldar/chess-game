#pragma once
#include <SFML/Graphics.hpp>
#include "../GameThread.hpp"
#include <string>

using namespace std;
using namespace sf;

inline constexpr int TOP_PANEL_HEIGHT = 50;
inline constexpr int PANEL_WIDTH = 200;
inline constexpr int VARIATION_HEIGHT = 50;
inline constexpr int INNER_MARGIN = 15;

class MoveSelectionPanel {
    RenderWindow& m_window;
    Text m_title;
    RectangleShape m_panel;
    RectangleShape m_variationsPanel;
    RectangleShape m_topRect;
    vector<RectangleShape> m_variationRectangles;
    vector<Text> m_variationTexts;
    int m_height;
    int m_selectionIndex = 0;
    int m_numberOfVariations = 0;

    public:
    MoveSelectionPanel(RenderWindow& window): m_window(window) {};

    void handleTitleText();
    void handlePanelRectangle();
    void handleSubPanels();
    void handleVariations(vector<std::string>&);
    void drawMoveSelectionPanel(vector<std::string>&);
    void goToNextVariation();
    void goToPreviousVariation();
};