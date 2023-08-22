#ifndef GAME_TREE_WIDGET_HPP
#define GAME_TREE_WIDGET_HPP
#include <QWebEngineView>
#include <QWidget>

#include "game/tree.hpp"
#include "game/state.hpp"

class HtmlMoveTreeBuilder;
class TreeHtml {
public:
    /*! \brief Returns html representation of the tree */
    static QString html(const Tree*);

private:
    static void traverse(HtmlMoveTreeBuilder& builder, Move lastMove, Board &board,
                         const TreeNode* node, const Tree* tree);
};

class MoveTreeWidget : public QWebEngineView {
    Q_OBJECT
public:
    explicit MoveTreeWidget(QWidget* parent = nullptr);

    /*! \brief Sets tree that will be rendered by this widget */
    void setState(const State *state) { m_state = state; }

    /*! \brief Returns satisfactory size */
    virtual QSize sizeHint() const;

public slots:
    /*! \brief Issues redraw */
    void redraw();
private slots:
    void onMoveClicked(const QUrl&);
    void onMoveHovered(const QString&);
signals:
    void moveSelected(size_t);

private:
    const State *m_state;
    /*!< Currently hovered node uid or 0. */
    size_t m_hoveredMoveUid;
    /*!< Uid used for action */
    size_t m_actionMoveUid;
};

#endif  // GAME_TREE_WIDGET_HPP
