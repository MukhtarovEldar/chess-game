#include "../include/ChessGame.hpp"
#include <algorithm>

void ChessGame::freeMemory() {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            board[i][j] = nullptr;
    for (Piece* piece: whitePieces) delete piece;
    for (Piece* piece: blackPieces) delete piece;
    whitePieces.clear();
    blackPieces.clear();
}

void ChessGame::reset() {
    freeMemory(); // Free up memory

    // Set the kings
    blackKing = new King(Team::BLACK, 0, 4);
    whiteKing = new King(Team::WHITE, 7, 4);

    // Set first row (black)
    board[0][0] = new Rook(Team::BLACK, 0, 0);
    board[0][1] = new Knight(Team::BLACK, 0, 1);
    board[0][2] = new Bishop(Team::BLACK, 0, 2);
    board[0][3] = new Queen(Team::BLACK, 0, 3);
    board[0][4] = blackKing;
    board[0][5] = new Bishop(Team::BLACK, 0, 5);
    board[0][6] = new Knight(Team::BLACK, 0, 6);
    board[0][7] = new Rook(Team::BLACK, 0, 7);

    // Set last row (white)
    board[7][0] = new Rook(Team::WHITE, 7, 0);
    board[7][1] = new Knight(Team::WHITE, 7, 1);
    board[7][2] = new Bishop(Team::WHITE, 7, 2);
    board[7][3] = new Queen(Team::WHITE, 7, 3);
    board[7][4] = whiteKing;
    board[7][5] = new Bishop(Team::WHITE, 7, 5);
    board[7][6] = new Knight(Team::WHITE, 7, 6);
    board[7][7] = new Rook(Team::WHITE, 7, 7);

    // Fill in white and black pawns
    for (int col = 0; col < 8; ++col) {
        board[1][col] = new Pawn(Team::BLACK, 1, col); // Black pawn on row 1
        board[6][col] = new Pawn(Team::WHITE, 6, col); // White pawn on row 6
    }

    // Add every black piece to the list of black pieces
    for (int row = 0; row < 2; ++row)
        for (int col = 0; col < 8; ++col)
            blackPieces.push_back(board[row][col]);
    
    // Add every white piece to the list of white pieces
    for (int row = 6; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            whitePieces.push_back(board[row][col]);
}

void ChessGame::setBoardTile(int x, int y, Piece* piece) {
    // Set up the piece
    board[y][x] = piece;
    if (piece != nullptr) piece->move(y, x); 
}

void ChessGame::applyMove(moveType* selectedMove, int xPos, int yPos, Piece* selectedPiece, Piece* lastMove, int CELL_SIZE) {
    const int castleRow = (getTurn() == Team::WHITE)? 7: 0;     

    switch (get<1>(*selectedMove)) {
        case MoveType::NORMAL:
            setBoardTile(xPos/CELL_SIZE, yPos/CELL_SIZE, selectedPiece);
            // soundMove.play();
            break;
        case MoveType::CAPTURE:
            setBoardTile(xPos/CELL_SIZE, yPos/CELL_SIZE, selectedPiece);
            // soundCapture.play();
            break;
        case MoveType::ENPASSANT:
            setBoardTile(xPos/CELL_SIZE, yPos/CELL_SIZE, selectedPiece);
            Pawn::setLastPawn(nullptr);
            setBoardTile(lastMove->getY(), lastMove->getX(), nullptr);
            break;
        case MoveType::CASTLE_KINGSIDE:
            setBoardTile(5, castleRow, getBoardTile(7, castleRow));
            setBoardTile(7, castleRow, nullptr);
            setBoardTile(6, castleRow, selectedPiece);
            break;
        case MoveType::CASTLE_QUEENSIDE:
            setBoardTile(3, castleRow, getBoardTile(0, castleRow));
            setBoardTile(0, castleRow, nullptr);
            setBoardTile(2, castleRow, selectedPiece);
            break;
        case MoveType::INIT_SPECIAL:
            setBoardTile(xPos/CELL_SIZE, yPos/CELL_SIZE, selectedPiece);
            break;
        case MoveType::NEWPIECE:
            selectedPiece->move(-1, -1); // Deleted
            Queen* queen = new Queen(getTurn(), yPos/CELL_SIZE, xPos/CELL_SIZE);
            setBoardTile(xPos/CELL_SIZE, yPos/CELL_SIZE, queen);
            addPiece(queen);
            break;
    }
}



