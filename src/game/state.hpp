#ifndef STATE_HPP
#define STATE_HPP


#include <game/tree.hpp>
#include <game/board.hpp>

class State {
public:
    State() {
        setTree(std::make_unique<Tree>());
    }
    bool setTree(std::unique_ptr<Tree> tree);
    const Board& getBoard() const { return m_board; }
    const Tree* getTree() const { return m_pTree.get(); }
    bool addMove(const Move &move);
    void setByTreeNode(TreeNode *node);
    void reset(const Board &board);
    void reset();

private:
    std::unique_ptr<Tree> m_pTree;
    Board m_board;
};

#endif
