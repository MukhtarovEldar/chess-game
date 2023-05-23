#include "../include/GameThread.hpp"
#include "../include/Components/MenuButton.hpp"
#include "../include/Ressources/RessourceManager.hpp"
#include "../include/Utilities/PieceTransition.hpp"
#include "../include/Utilities/Move.hpp"
#include "../include/Components/SidePanel.hpp"
#include "../include/Components/MoveSelectionPanel.hpp"
#include "../include/Utilities/DrawableSf.hpp"
#include "./Ressources/Shader.cpp"
#include "../include/UIManager.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;

namespace 
{
void checkIfMoveMakesKingChecked(
    const shared_ptr<Move>& move,
    bool& kingChecked_,
    bool& noMovesAvailable_)
{
    kingChecked_ = move ? move->kingIsChecked() : false;
    noMovesAvailable_ = move ? move->hasNoMovesAvailable() : false;
}
} // anonymous namespace

void GameThread::startGame()
{
    ui::UIManager uiManager(board, treeIterator, moveList);

    // Parameters to handle a piece being dragged
    ui::DragState dragState;
    ui::ClickState clickState;
    ui::ArrowsInfo arrowsInfo;

    possibleMoves = board.calculateAllMoves();

    // Additional board state variables
    shared_ptr<Piece> pLastMove;

    // Sounds for piece movement
    SoundBuffer bufferMove;
    if (!bufferMove.loadFromFile(RessourceManager::getAudioPath("move.wav"))) return;
    Sound soundMove;
    soundMove.setBuffer(bufferMove);

    SoundBuffer bufferCapture;
    if (!bufferCapture.loadFromFile(RessourceManager::getAudioPath("captures.wav"))) return;
    Sound soundCapture;
    soundCapture.setBuffer(bufferCapture);

    // This is the main loop (a.k.a game loop) this ensures that the program does not terminate until we exit
    Event event;
    while (uiManager.getWindow().isOpen())
    {
        uiManager.clearWindow();

        // We use a while loop for the pending events in case there were multiple events occured
        while (uiManager.getWindow().pollEvent(event))
        {
            if (event.type == Event::Closed) uiManager.getWindow().close();
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    clickState.mousePos = {event.mouseButton.x, event.mouseButton.y};

                    // Allow user to make moves only if they're at the current live position,
                    // and if the click is on the chess board
                    int yPos = ui::getTileYPos(clickState.mousePos);
                    if (yPos < 0) continue;

                    // Do not register click if Moveselection panel is activated
                    // and the mouse is not within the panel's bounds
                    if (uiManager.ignoreInputWhenSelectionPanelIsActive(clickState.mousePos)) continue;

                    int xPos = isFlipped? 7-ui::getTileXPos(clickState.mousePos): ui::getTileXPos(clickState.mousePos);
                    if (isFlipped) yPos = 7-yPos;
                    auto pPieceAtCurrentMousePos = board.getBoardTile(xPos, yPos);

                    // If piece is not null and has the right color
                    if (pPieceAtCurrentMousePos && pPieceAtCurrentMousePos->getTeam() == board.getTurn())
                    {
                        // Unselect clicked piece
                        if (pPieceAtCurrentMousePos == clickState.pSelectedPiece)
                        {
                            clickState.pSelectedPiece.reset();
                            clickState.pieceIsClicked = false;
                            continue;
                        }

                        clickState.pSelectedPiece = pPieceAtCurrentMousePos;
                        clickState.pieceIsClicked = false;

                        dragState.pieceIsMoving = true;
                        dragState.lastXPos = isFlipped? 7-ui::getTileXPos(clickState.mousePos): ui::getTileXPos(clickState.mousePos); 
                        dragState.lastYPos = yPos;

                        // Set the tile on the board where the piece is selected to null
                        board.resetBoardTile(dragState.lastXPos, dragState.lastYPos, false);
                    }
                }
                if (event.mouseButton.button == Mouse::Right)
                {
                    if (!dragState.pieceIsMoving)
                    {
                        clickState.rightClickAnchor = {event.mouseButton.x, event.mouseButton.y};
                        clickState.isRightClicking = true;

                        arrowsInfo.currArrow.setOrigin(clickState.rightClickAnchor);
                        arrowsInfo.currArrow.setDestination(clickState.rightClickAnchor);
                    }
                }
            }

            const bool aPieceIsHandled = (dragState.pieceIsMoving || clickState.pieceIsClicked || clickState.isRightClicking);

            // Dragging a piece around
            if (event.type == Event::MouseMoved && aPieceIsHandled)
            {
                // Update the position of the piece that is being moved
                Vector2i mousePosition = Mouse::getPosition(uiManager.getWindow());
                clickState.mousePos = {mousePosition.x, mousePosition.y};

                if (clickState.isRightClicking)
                {
                    arrowsInfo.currArrow.setDestination(clickState.mousePos);
                    arrowsInfo.currArrow.updateArrow(); // Update the type and rotation
                }
            }

            // Mouse button released
            if (event.type == Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    // Handle menu bar buttons
                    if (clickState.mousePos.second < ui::g_MENUBAR_HEIGHT)
                    {
                        for (auto& menuButton: uiManager.getMenuBar()) 
                        {
                            if (!menuButton.isMouseHovered(clickState.mousePos)) continue;
                            menuButton.doMouseClick(board, moveList);
                            if (!menuButton.isBoardReset()) continue;
                            
                            clickState.pSelectedPiece.reset();
                            clickState.mousePos = {0, 0};

                            arrowsInfo.arrows.clear();
                            refreshMoves();
                        }
                    }

                    // Handle Side Panel Move Box buttons click
                    uiManager.handleSidePanelMoveBox(clickState.mousePos);
                    
                    // ^^^ Possible bug here when moveboxe and moveselection panel overlap

                    if (!clickState.pSelectedPiece) continue;

                    // If clicked and mouse remained on the same square
                    int xPos = isFlipped? 7-ui::getTileXPos(clickState.mousePos): ui::getTileXPos(clickState.mousePos);
                    int yPos = isFlipped? 7-ui::getTileYPos(clickState.mousePos): ui::getTileYPos(clickState.mousePos);
                    if (xPos == clickState.pSelectedPiece->getY() && yPos == clickState.pSelectedPiece->getX())
                    {
                        if (!clickState.pieceIsClicked)
                        {
                            // Put the piece back to it's square; it's not moving
                            board.setBoardTile(dragState.lastXPos, dragState.lastYPos, clickState.pSelectedPiece, false);
                            dragState.pieceIsMoving = false;
                        }
                        clickState.pieceIsClicked = !clickState.pieceIsClicked;
                        continue;
                    }

                    // Try to match moves
                    Move* pSelectedMove = nullptr;
                    for (auto& move: possibleMoves)
                    {
                        if (move.getSelectedPiece() == clickState.pSelectedPiece)
                        {
                            if (move.getTarget().first == yPos && move.getTarget().second == xPos)
                            {
                                pSelectedMove = &move;
                                break;
                            }
                        }
                    }

                    // If move is not allowed, place piece back, else apply the move
                    if (pSelectedMove == nullptr)
                    {
                        board.setBoardTile(dragState.lastXPos, dragState.lastYPos, clickState.pSelectedPiece, false); // Cancel the move
                    }
                    else
                    {
                        MoveType type = pSelectedMove->getMoveType();
                        shared_ptr<Move> pMove = make_shared<Move>(make_pair(xPos, yPos), make_pair(dragState.lastXPos, dragState.lastYPos), clickState.pSelectedPiece, type);

                        pMove->setCapturedPiece(pLastMove);
                        pMove->setMoveArrows(arrowsInfo.arrows);
                        moveList.addMove(pMove, arrowsInfo.arrows);

                        pLastMove = clickState.pSelectedPiece;
                        pLastMove->setLastMove(type);
                        Piece::setLastMovedPiece(pLastMove);

                        board.switchTurn();
                        refreshMoves();
                        
                        noMovesAvailable = possibleMoves.empty();
                        if (noMovesAvailable) pMove->setNoMovesAvailable();

                        if (board.kingIsChecked())
                        {
                            kingChecked = true;
                            pMove->setChecked();
                        }
                        else
                        {
                            kingChecked = false;
                            if (type == MoveType::CAPTURE || type == MoveType::ENPASSANT) soundCapture.play();
                            else if (type == MoveType::NORMAL || type == MoveType::INIT_SPECIAL) soundMove.play();
                        }

                        // moveTree.insertNode(pMove, treeIterator);
                        // moveTree.printTree();
                        arrowsInfo.arrows.clear();
                    }

                    clickState.pSelectedPiece.reset();
                    clickState.pieceIsClicked = false;
                    clickState.mousePos = {0, 0};
                    dragState.pieceIsMoving = false;
                }
                if (event.mouseButton.button == Mouse::Right)
                {
                    if (clickState.pSelectedPiece && dragState.pieceIsMoving)
                    {
                        // Reset the piece back
                        board.setBoardTile(dragState.lastXPos, dragState.lastYPos, clickState.pSelectedPiece, false);
                        clickState.pSelectedPiece.reset();
                        dragState.pieceIsMoving = false;
                    }
                    else if (clickState.isRightClicking)
                    {
                        // add arrow to arrow list to be drawn
                        if (arrowsInfo.currArrow.isDrawable())
                        {
                            if (!arrowsInfo.currArrow.removeArrow(arrowsInfo.arrows)) 
                            {
                                arrowsInfo.arrows.push_back(arrowsInfo.currArrow);
                            }
                        }
                        clickState.isRightClicking = false;
                        clickState.rightClickAnchor = {0, 0};
                        arrowsInfo.currArrow.resetParameters();
                    }
                }
            }

            if (event.type == Event::KeyPressed)
            {
                handleKeyPressed(event, uiManager.getMoveSelectionPanel(), arrowsInfo.arrows, uiManager.showMoveSelectionPanel());
            }
        }
        
        uiManager.draw(
            clickState, 
            dragState, 
            arrowsInfo, 
            kingChecked, 
            possibleMoves, 
            noMovesAvailable, 
            kingChecked);
            
        highlightLastMove(uiManager.getWindow());
        
        uiManager.display();
    }
}

void GameThread::setTransitioningPiece(
    bool isUndo_, shared_ptr<Piece>& p_, int initialX_, int initialY_,
    int xTarget_, int yTarget_, shared_ptr<Piece>& captured_,
    int capturedXPos_, int capturedYPos_, PieceTransition& trans_
)
{
    trans_.resetPieces();
    trans_.setTransitioningPiece(p_);
    trans_.setDestination({ui::getWindowXPos(xTarget_), ui::getWindowYPos(yTarget_)});
    trans_.setCurrPos({ui::getWindowXPos(initialX_), ui::getWindowYPos(initialY_)});
    trans_.setCapturedPiece(captured_, ui::getWindowXPos(capturedXPos_), ui::getWindowYPos(capturedYPos_));
    trans_.setUndo(isUndo_);
    trans_.setIsTransitioning(true);
    trans_.setIncrement();
}

void GameThread::setSecondTransitioningPiece(
    shared_ptr<Piece>& p_, int initialX_, int initialY_,
    int xTarget_, int yTarget_, PieceTransition& trans_
)
{
    trans_.setSecondTransitioningPiece(p_);
    trans_.setSecondDestination({ui::getWindowXPos(xTarget_), ui::getWindowYPos(yTarget_)});
    trans_.setSecondCurrPos({ui::getWindowXPos(initialX_), ui::getWindowYPos(initialY_)});
    trans_.setSecondIsTransitioning(true);
    trans_.setSecondIncrement();
}

void GameThread::highlightLastMove(RenderWindow& window_)
{
    shared_ptr<Move> move = treeIterator->m_move;
    if (!move) return;
    
    RectangleShape squareBefore = ui::createSquare();
    RectangleShape squareAfter = ui::createSquare();

    Color colorInit = ((move->getInit().first+move->getInit().second) % 2)
                    ? Color(170, 162, 58): Color(205, 210, 106);
    Color colorTarget = ((move->getTarget().first+move->getTarget().second) % 2)
                    ? Color(170, 162, 58): Color(205, 210, 106);
    squareBefore.setFillColor(colorInit);
    squareAfter.setFillColor(colorTarget);

    squareBefore.setPosition(
        ui::getWindowXPos(GameThread::boardFlipped()? 7-move->getInit().first: move->getInit().first),
        ui::getWindowYPos(GameThread::boardFlipped()? 7-move->getInit().second: move->getInit().second)
    );
    squareAfter.setPosition(
        ui::getWindowXPos(GameThread::boardFlipped()? 7-move->getTarget().first: move->getTarget().first),
        ui::getWindowYPos(GameThread::boardFlipped()? 7-move->getTarget().second: move->getTarget().second)
    );

    window_.draw(squareBefore);
    window_.draw(squareAfter);
}


void GameThread::handleKeyPressed(
    const Event& event_, MoveSelectionPanel& moveSelectionPanel_,
    vector<Arrow>& arrowList_, bool& showMoveSelectionPanel_
)
{
    // If a piece is already moving, make it arrive
    if (transitioningPiece.getIsTransitioning())
        transitioningPiece.setHasArrived(board);

    shared_ptr<Move> move;
    switch (event_.key.code)
    {
        case Keyboard::Left:
            moveList.goToPreviousMove(true, arrowList_);
            // moveTree.goToPreviousNode(treeIterator);
            move = treeIterator.get()->m_move;
            checkIfMoveMakesKingChecked(move, kingChecked, noMovesAvailable);
            break;
        case Keyboard::Right:
            if (treeIterator.get()->childNumber > 1)
            {
                if (showMoveSelectionPanel_)
                {
                    moveList.goToNextMove(true, arrowList_);
                    // moveTree.goToNextNode(moveSelectionPanel_.getSelection(), treeIterator);
                    move = treeIterator.get()->m_move;
                    checkIfMoveMakesKingChecked(move, kingChecked, noMovesAvailable);
                }
                showMoveSelectionPanel_ = !showMoveSelectionPanel_;
                return;
            }
            moveList.goToNextMove(true, arrowList_);
            // moveTree.goToNextNode(0, treeIterator);
            move = treeIterator.get()->m_move;
            checkIfMoveMakesKingChecked(move, kingChecked, noMovesAvailable);
            break;
        case Keyboard::LControl:
            flipBoard();
            break;
        case Keyboard::Up:
            showMoveSelectionPanel_?
                moveSelectionPanel_.goToPreviousVariation(): moveList.goToCurrentMove(arrowList_);
            break;
        case Keyboard::Down:
            showMoveSelectionPanel_?
                moveSelectionPanel_.goToNextVariation(): moveList.goToInitialMove(arrowList_);
            break;
        case Keyboard::Enter:
            if (showMoveSelectionPanel_)
            {
                moveList.goToNextMove(true, arrowList_);
                // moveTree.goToNextNode(moveSelectionPanel_.getSelection(), treeIterator);
                kingChecked = treeIterator.get()->m_move->kingIsChecked();
                showMoveSelectionPanel_ = false; // Close the panel display
            }
            break;
        default: break; // Avoid pattern matching not exhaustive warning
    }
}
