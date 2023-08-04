#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../include/Components/Board.hpp"
#include "../include/Pieces/Piece.hpp"
#include "../include/Pieces/King.hpp"
#include "BoardPositionsUtil.hpp"

namespace 
{
    struct MoveFilterBoardFixture 
    {
        Board m_board;
        
        MoveFilterBoardFixture() = default;
        ~MoveFilterBoardFixture() = default;

        void initBoard(const std::string& fen_) 
        {
            m_board = Board(fen_);
            if (m_board.getTurn() != Team::WHITE) m_board.switchTurn();
            m_board.updateAllCurrentlyAvailableMoves();
        }
    };
}

BOOST_FIXTURE_TEST_SUITE(MoveFilteringTests, MoveFilterBoardFixture)

BOOST_AUTO_TEST_CASE(TestInitialNumberOfMovesAvailable)
{
    initBoard(testUtil::FEN_DEFAULT_POSITION);
    BOOST_CHECK_EQUAL(m_board.getAllCurrentlyAvailableMoves().size(), 20);
}

BOOST_AUTO_TEST_CASE(TestBlackInitialNumberOfMovesAvailable)
{
    initBoard(testUtil::FEN_DEFAULT_POSITION);
    m_board.switchTurn();
    m_board.updateAllCurrentlyAvailableMoves();
    BOOST_CHECK_EQUAL(m_board.getAllCurrentlyAvailableMoves().size(), 20);
}

BOOST_AUTO_TEST_CASE(TestRemovalOfIllegalMoves1)
{
    initBoard(testUtil::FEN_POSITION_WITH_ILLEGAL_MOVES_1);

    std::vector<Move> allMoves = m_board.getAllCurrentlyAvailableMoves();

    // Ensure that the illegal moves are filtered out
    auto& pBlackKing = m_board.getBoardTile({'e', 8});
    int rank = 0, file = 4;
    const coor2d initialPosition1{rank, file};
    Move illegalMove1({rank, file + 1}, initialPosition1, pBlackKing, MoveType::NORMAL);

    auto& pBlackPawn = m_board.getBoardTile({'f', 7});
    rank = 0, file = 5;
    const coor2d initialPosition2{rank, file};
    Move illegalMove2({rank + 1, file}, initialPosition2, pBlackPawn, MoveType::NORMAL);

    for (auto& move : allMoves)
    {
        BOOST_CHECK(move != illegalMove1 && move != illegalMove2);
    }
}

BOOST_AUTO_TEST_CASE(TestRemovalOfIllegalMoves2)
{
    initBoard(testUtil::FEN_POSITION_WITH_ILLEGAL_MOVES_2);

    std::vector<Move> allMoves = m_board.getAllCurrentlyAvailableMoves();

    // In this position king is in check, there is only one move available
    
    // Ensure that the illegal moves are filtered out
    auto& pBlackKing = m_board.getBoardTile({'e', 8});
    int rank = 0, file = 4;
    const coor2d initialPosition1{rank, file};
    Move illegalMove1({rank, file + 1}, initialPosition1, pBlackKing, MoveType::NORMAL);

    auto& pBlackPawn = m_board.getBoardTile({'f', 7});
    rank = 0, file = 5;
    const coor2d initialPosition2{rank, file};
    Move illegalMove2({rank + 1, file}, initialPosition2, pBlackPawn, MoveType::NORMAL);

    for (auto& move : allMoves)
    {
        BOOST_CHECK(move != illegalMove1 && move != illegalMove2);
    }
}

BOOST_AUTO_TEST_SUITE_END()