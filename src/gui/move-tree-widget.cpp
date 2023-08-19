#include "gui/move-tree-widget.hpp"
#include "util/html-move-tree-builder.hpp"
#include "settings/settings-factory.hpp"
#include <QWebEnginePage>
#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QShortcut>

void TreeHtml::traverse(HtmlMoveTreeBuilder& builder, Move lastMove,
                        const TreeNode* node, const Tree* tree)
{
    if (lastMove != Move::NullMove) {
        const Board& board = node->parent()->board();
        QString notation = board.algebraicNotationString(lastMove);
        QString number = QString::number(board.fullMoveCount());

        if (board.currentPlayer().isWhite())
            builder.addMoveNumber(number + ". ");
        // It is a variant.
        else if (node->parent()->next() != node)
            builder.addMoveNumber(number + "... ");
        // It is a position where we are starting from black move.
        else if (board.currentPlayer().isBlack() && !node->parent()->parent())
            builder.addMoveNumber(number + "... ");

        builder.addMove(notation, node->uid(), node->uid() == tree->currentNode()->uid());

        if (!node->annotation().isEmpty())
            builder.addAnnotation(node->annotation());

        HtmlMoveTreeBuilder childBuilder;
        for (const Move& next : node->nonMainMoves())
            traverse(childBuilder, next, node->next(next), tree);

        if (!childBuilder.isEmpty())
            builder.addVariant(childBuilder.html());
    }

    if (node->hasNeighbours())
        traverse(builder, node->nextMove(), node->next(), tree);
}

QString TreeHtml::html(const Tree* tree)
{
    HtmlMoveTreeBuilder builder;
    traverse(builder, Move::NullMove, tree->rootNode(), tree);
    return builder.htmlWithStyle();
}


MoveTreeWidget::MoveTreeWidget(QWidget* parent)
    : QWebEngineView(parent)
    , m_tree(nullptr)
    , m_hoveredMoveUid(0)
    , m_actionMoveUid(0)
{
    QObject::connect(&SettingsFactory::html(), &HtmlSettings::changed,
                     this, &MoveTreeWidget::redraw);

    QObject::connect(page(), &QWebEnginePage::linkHovered, this, &MoveTreeWidget::onMoveHovered);
    // TODO: implement this
    //QObject::connect(this, &QWebView::linkClicked, this, &MoveTreeWidget::onMoveClicked);

    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(onMovePrev()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(onMoveNext()));
}

void MoveTreeWidget::setTree(Tree* tree)
{
    m_tree = tree;
    redraw();
}

QSize MoveTreeWidget::sizeHint() const
{
    return QSize(250, 100);
}

void MoveTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    m_actionMoveUid = m_hoveredMoveUid;
    // Don't care if no node has been hovered.
    if (!m_actionMoveUid)
        return;

    QMenu menu;
    menu.addAction("Annotate", this, SLOT(onAnnotate()));
    menu.addSeparator();
    menu.addAction("Promote up", this, SLOT(onPromoteUp()));
    menu.addAction("Promote to mainline", this, SLOT(onPromoteToMainline()));
    menu.addSeparator();
    menu.addAction("Remove", this, SLOT(onRemove()));
    menu.addAction("Remove annotation", this, SLOT(onRemoveAnnotation()));
    menu.exec(event->globalPos());

    redraw();
}

void MoveTreeWidget::redraw()
{
    setHtml(TreeHtml::html(m_tree));
}

void MoveTreeWidget::onMovePrev()
{
    // FIXME: Ugly casts!
    if (m_tree->currentNode()->parent())
        m_tree->setCurrent(const_cast<TreeNode*>(m_tree->currentNode()->parent()));
}

void MoveTreeWidget::onMoveNext()
{
    // FIXME: Ugly casts!
    if (m_tree->currentNode()->next())
        m_tree->setCurrent(const_cast<TreeNode*>(m_tree->currentNode()->next()));
}

void MoveTreeWidget::onAnnotate()
{
    QString annotation = QInputDialog::getMultiLineText(this, tr("Edit annotation"), tr("Text:"));
    m_tree->annotate(TreeNode::fromUid(m_actionMoveUid), annotation);
}

void MoveTreeWidget::onRemoveAnnotation()
{
    m_tree->annotate(TreeNode::fromUid(m_actionMoveUid), QString());
}

void MoveTreeWidget::onPromoteUp()
{
    m_tree->promote(TreeNode::fromUid(m_actionMoveUid));
}

void MoveTreeWidget::onPromoteToMainline()
{
    m_tree->promoteToMainline(TreeNode::fromUid(m_actionMoveUid));
}

void MoveTreeWidget::onRemove()
{
    m_tree->remove(TreeNode::fromUid(m_actionMoveUid));
}

void MoveTreeWidget::onMoveClicked(const QUrl&)
{
    emit moveSelected(m_hoveredMoveUid);
}

void MoveTreeWidget::onMoveHovered(const QString& url)
{
    if (url.isEmpty())
        m_hoveredMoveUid = 0;
    else
        m_hoveredMoveUid = url.toULongLong();
}
