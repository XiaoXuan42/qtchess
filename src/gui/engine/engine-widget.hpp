#ifndef ENGINE_WIDGET_HPP
#define ENGINE_WIDGET_HPP

#include <QVector>
#include <QWidget>
#include <memory>

#include "engine/engine.hpp"
#include "game/board.hpp"

namespace Ui {
class EngineWidget;
}

typedef std::unique_ptr<Engine> EnginePtr;

class Tree;
class EngineWidget : public QWidget {
    Q_OBJECT
public:
    explicit EngineWidget(EnginePtr pEngine, const QString &engineName, QWidget* parent = 0);
    ~EngineWidget();

    /*! \brief Overriden for view compactness. */
    virtual QSize sizeHint() const;
public slots:
    /*! \brief Sets current board position */
    void setBoard(const Board& board);
private slots:
    void onVariantParsed(VariantInfo);
    void onAnalyzeClicked();
    void onStopClicked();
public slots:
    void redraw();
    void reset();

private:
    QString m_engineName;
    Ui::EngineWidget* ui;
    EnginePtr m_engine;
    Board m_currentBoard;
    QVector<VariantInfo> m_variants;
};

#endif  // ENGINE_WIDGET_HPP
