#ifndef GAME_TREE_WIDGET_HPP
#define GAME_TREE_WIDGET_HPP
#include "game-tree.hpp"
#include <QWidget>
#include <QWebView>
#include <QList>
#include <QMap>

class GameTreeWidget : public QWebView
{
    Q_OBJECT
public:
    explicit GameTreeWidget(QWidget *parent = 0);

    void redraw();
    void setGameTree(GameTree* Tree);

    virtual QSize sizeHint() const override {
        return QSize(100, 100);
    }
protected:
    virtual void contextMenuEvent(QContextMenuEvent *);
private slots:
    void onMoveClick(const QUrl&);
    void onLinkHover(const QString& Url);
signals:
    void positionSelected(GameTreeNode*);

private:
    GameTree* mTree;
    // Currently hovered node by the mouse. Nullptr if no node
    // is hovered.
    GameTreeNode* mHoveredNode;
};

#endif // GAME_TREE_WIDGET_HPP
