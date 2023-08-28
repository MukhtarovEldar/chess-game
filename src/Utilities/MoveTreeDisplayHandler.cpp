#include "../../include/Utilities/MoveTreeDisplayHandler.hpp"

namespace 
{
    std::string getLetterPrefix(MoveTree::Iterator iter_, int indentationLevel_)
    {
        // Base case
        if (indentationLevel_ == 1) 
        {
            return std::string(1, static_cast<char>( 
                'A' + 
                iter_.getNodeIdxAmongSiblings() - 
                ((iter_.getNbOfNodesAtCurrentLevel() % 2 == 0) ? 1 : 0)
            )) + ")";
        }
            
        // Go to subvariations that are indented by more than 1 level.
        // For these we use the sibling index of the ancestors to establish
        // the correct number and letter prefix.    
        const int currentNodeIdx = iter_.getNodeIdxAmongSiblings();
        iter_.goToParent();
        const auto parentLetterPrefix = getLetterPrefix(iter_, indentationLevel_ - 1);
        const auto numberSuffix = std::to_string(currentNodeIdx);
        
        if (indentationLevel_ > 2) return parentLetterPrefix.substr(0, parentLetterPrefix.size() - 1) + "," + numberSuffix + ")";
        return parentLetterPrefix.substr(0, parentLetterPrefix.size() - 1) + numberSuffix + ")";
    }
}

MoveTreeDisplayHandler::MoveTreeDisplayHandler(const MoveTree& tree_) 
: m_tree(tree_) 
{
}

void printMoves(std::vector<MoveInfo>& moveInfo_) 
{
    std::cout << "There are currently " << moveInfo_.size() << std::endl; 
    std::cout << "Moves to display: " << std::endl;
    for (const MoveInfo& move : moveInfo_) {
        std::cout << move.m_content << ", row : " << move.m_row << ", depth: " << move.m_indentLevel << endl;;
    }
    std::cout << std::endl;
}

void MoveTreeDisplayHandler::processNode(
    MoveTree::Iterator& iter_, 
    int level_, 
    int& row_, 
    bool isNewLineSubvariation_) 
{
    auto* move = iter_->m_move.get();
    if (move == nullptr) return;

    MoveInfo info;
    const int nodeDepth = iter_.getNodeLevel();
    info.m_content = parseMove(*move, nodeDepth / 2 + 1, nodeDepth % 2 != 0, isNewLineSubvariation_);
    info.m_indentLevel = level_;
    info.m_row = row_;
    info.m_movePtr = move;

    if (isNewLineSubvariation_)
    {
        info.m_letterPrefix = getLetterPrefix(iter_, level_);
    }

    m_moveInfos.push_back(info);
}

void MoveTreeDisplayHandler::processNodeRec(
    MoveTree::Iterator& iter_, 
    int level_, 
    int& row_, 
    bool isNewLineSubvariation_) 
{
    processNode(iter_, level_, row_, isNewLineSubvariation_);
    // If only 1 child go as usual
    // If more than one child, need to start with non-main line

    if (iter_->m_children.size() < 2)
    {
        if (iter_->m_children.size() == 0) return;
        iter_.goToChild(0);
        processNodeRec(iter_, level_, row_); 
        iter_.goToParent();
    }
    else
    {
        // process index 0.
        iter_.goToChild(0);
        processNode(iter_, level_, row_);
        iter_.goToParent();
        
        // then loop over index 1 to N-1
        for (size_t i = 1; i < iter_->m_children.size(); ++i) {
            ++row_;
            iter_.goToChild(i);
            processNodeRec(iter_, level_ + 1, row_, true); 
            iter_.goToParent();
        }

        // after the loop, goToGrandChild(0) and increase row if 
        // main line stretches after.
        if (iter_.goToGrandChild(0))
        {
            ++row_;
            processNodeRec(iter_, level_, row_);
        }
    }
}

std::vector<MoveInfo> MoveTreeDisplayHandler::generateMoveInfo() 
{
    if (m_tree.getNumberOfMoves() == 0) return {};

    m_moveInfos.clear();

    MoveTree::Iterator iter = m_tree.begin();
    int row = 0;

    for (size_t i = 0; i < iter->m_children.size(); ++i)
    {
        iter.goToChild(i);
        processNodeRec(iter, 0, row);
        iter.goToParent();
    }
    
    //printMoves(m_moveInfos);
    return m_moveInfos;
}

void printMoveInfos(const std::vector<MoveInfo>& moveInfos_) 
{
    int currentRow = -1; 

    for (const auto& info : moveInfos_) 
    {
        if (info.m_row != currentRow) 
        {
            std::cout << std::endl;
            currentRow = info.m_row;
        }

        for (int i = 0; i < info.m_indentLevel; ++i) {
            std::cout << " ";
        }

        if (info.m_letterPrefix.has_value()) {
            std::cout << "+--- " << info.m_letterPrefix.value() << " ";
        }

        std::cout << info.m_content;

        if (info.m_indentLevel == 0) std::cout << " ";
    }
    std::cout << std::endl;
}
