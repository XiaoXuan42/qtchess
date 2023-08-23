#ifndef GAME_TREE_WIDGET_HPP
#define GAME_TREE_WIDGET_HPP
#include <QWebEngineView>
#include <QWidget>

#include "game/state.hpp"
#include "game/tree.hpp"

class HtmlMoveTreeBuilder;
class TreeHtml {
public:
    /*! \brief Returns html representation of the tree */
    static QString html(const Tree *);

private:
    static void traverse(HtmlMoveTreeBuilder &builder, Move lastMove,
                         Board &board, const TreeNode *node, const Tree *tree);
};

class MoveTreeWebPage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit MoveTreeWebPage(QWidget *parent = nullptr)
        : QWebEnginePage(parent) {}

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type,
                                 bool isMainFrame) override {
        if (type == QWebEnginePage::NavigationTypeLinkClicked) {
            if (url.scheme() == "uid") {
                size_t uid = std::stoull(url.toString().toStdString().substr(6));
                emit clicked(uid);
            }
            return false;
        }
        return true;
    }

signals:
    void clicked(size_t uid);
};

class MoveTreeWidget : public QWebEngineView {
    Q_OBJECT
public:
    explicit MoveTreeWidget(QWidget *parent = nullptr);

    /*! \brief Sets tree that will be rendered by this widget */
    void setState(const State *state) { m_state = state; }

    /*! \brief Returns satisfactory size */
    virtual QSize sizeHint() const;

public slots:
    /*! \brief Issues redraw */
    void redraw();
private slots:
    void onMoveClicked(size_t);
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
