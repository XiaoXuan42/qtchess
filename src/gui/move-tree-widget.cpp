#include "gui/move-tree-widget.hpp"

#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMenu>
#include <QShortcut>
#include <QWebEnginePage>

#include "settings/settings-factory.hpp"
#include "util/html-move-tree-builder.hpp"
#include "util/stringify.hpp"


void TreeHtml::traverse(HtmlMoveTreeBuilder& builder, Move lastMove,
                        Board& board, const TreeNode* node, const Tree* tree) {
    // lastMove is not made by board
    if (lastMove != Move::NullMove) {
        QString notation = Stringify::algebraicNotationString(board, lastMove);
        QString number = QString::number(board.fullMoveCount());

        if (board.currentPlayer().isWhite())
            builder.addMoveNumber(number + ". ");
        // It is a variant.
        else if (node->parent()->next() != node)
            builder.addMoveNumber(number + "... ");
        // It is a position where we are starting from black move.
        else if (board.currentPlayer().isBlack() && !node->parent()->parent())
            builder.addMoveNumber(number + "... ");

        builder.addMove(notation, node->uid(),
                        node->uid() == tree->currentNode()->uid());

        HtmlMoveTreeBuilder childBuilder;
        for (const Move& next : node->nonMainMoves()) {
            Board newBoard = board;
            // TODO: warning here for invalid move
            newBoard.makeMove(lastMove);
            if (newBoard.makeMove(next)) {
                traverse(childBuilder, next, newBoard, node->next(next), tree);
            }
        }

        if (!childBuilder.isEmpty()) builder.addVariant(childBuilder.html());
    }

    if (node->hasNeighbours()) {
        // TODO: warning here for invalid move
        board.makeMove(lastMove);
        traverse(builder, node->nextMove(), board, node->next(), tree);
    }
}

QString TreeHtml::html(const Tree* tree) {
    if (tree == nullptr) {
        return "";
    }
    HtmlMoveTreeBuilder builder;
    Board initBoard = *tree->rootNode()->getBoard();
    traverse(builder, Move::NullMove, initBoard, tree->rootNode(), tree);
    return builder.htmlWithStyle();
}

MoveTreeWidget::MoveTreeWidget(QWidget* parent)
    : QWebEngineView(parent),
      m_state(nullptr),
      m_hoveredMoveUid(0),
      m_actionMoveUid(0) {
    auto* clickPage = new MoveTreeWebPage(this);
    setPage(clickPage);
    QObject::connect(&SettingsFactory::html(), &HtmlSettings::changed, this,
                     &MoveTreeWidget::redraw);
    QObject::connect(clickPage, &MoveTreeWebPage::clicked, this,
                     &MoveTreeWidget::onMoveClicked);
}

QSize MoveTreeWidget::sizeHint() const { return QSize(250, 100); }

void MoveTreeWidget::redraw() { setHtml(TreeHtml::html(m_state->getTree())); }

void MoveTreeWidget::onMoveClicked(size_t uid) {
    emit moveSelected(uid);
}
