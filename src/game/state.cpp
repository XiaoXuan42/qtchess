#include <game/state.hpp>

bool State::setTree(std::unique_ptr<Tree> tree) {
    if (!tree) {
        return false;
    }
    m_pTree = std::move(tree);
    m_board = *(m_pTree->getBoard());
    return true;
}

bool State::addMove(const Move &move) {
    if (!m_board.makeMove(move)) {
        return false;
    }
    if (m_pTree) {
        m_pTree->addMove(move);
    }
    return true;
}

void State::setByTreeNode(TreeNode *node) {
    // NOTE: assert current tree owns this node
    m_pTree->setCurrent(node);
    m_board = *(m_pTree->getBoard());
}

void State::reset(const Board &board) {
    setTree(std::make_unique<Tree>(board));
}

void State::reset() { reset(Board()); }
