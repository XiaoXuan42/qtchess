#ifndef BOARDWIDGET_HPP
#define BOARDWIDGET_HPP

#include <QWidget>

#include "game/state.hpp"
#include "settings/settings-factory.hpp"

class BoardSettings;
class BoardWidgetState;
class BoardWidgetStateDragging;
class BoardWidgetStateNormal;
class PieceSet;

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget* parent = nullptr,
                         BoardSettings& settings = SettingsFactory::board());

    /* Emits move signal */
    void emitMove(Move move);
    /* Handles settings change */
    void update();
    /* Checks if given (x, y) is in some square of the board, if so,
       it initializes rank, file variables */
    bool isFieldAt(double x, double y, int* file, int* rank) const;
    const State& getGameState() const { return *m_gameState; }
    void setGameState(const State* pState) { m_gameState = pState; }

protected:
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
signals:
    void moveMade(Move);
public slots:
    /* Reverses board view */
    void flip();
    /* Redraws entire board */
    void redraw();

private:
    bool isValid() const {
        return m_gameState != nullptr && m_boardState != nullptr &&
               m_pieceSet != nullptr;
    }
    /* Ensures valid piece set */
    void ensureValidPieceSet();
    /* Draws board contents */
    void draw(QPainter& ctx);
    /* Draws piece at x,y in the canvas using SVG renderer */
    void drawPiece(QPainter& ctx, QRectF dest, Piece piece);
    /* Draws piece at given (rank, file) */
    void drawPiece(QPainter& ctx, int rank, int file);
    /* Draws floating / dragging piece if any */
    void drawDraggedPiece(QPainter& ctx);
    /* Draws single square at (rank, file) of the chessboard */
    void drawField(QPainter& ctx, int rank, int file);
    /* Draws selection border */
    void drawSelection(QPainter& ctx);
    /* Draws border with files and ranks */
    void drawBorder(QPainter& ctx);
    /* Helper function to determine given rank offset */
    int getRankOffset(int rank) const;
    /* Helper function to determine given file offset */
    int getFileOffset(int file) const;
    /* Returns absolute coordinate depending on board rotation. */
    int absolute(int coord) const;

private:
    BoardSettings& m_settings;
    BoardWidgetState* m_boardState;
    PieceSet* m_pieceSet;
    const State* m_gameState;
    /* Geometry properties */
    bool m_flipped;
    int m_width;  /* width of the drawing area */
    int m_height; /* height of the drawing area */
    int m_fieldSize;
    /* Beginning of the chessboard squares */
    int m_firstFieldX;
    int m_firstFieldY;
};

#endif  // BOARDWIDGET_HPP
