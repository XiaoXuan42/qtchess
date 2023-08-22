#ifndef BOARDWIDGETSTATE_HPP
#define BOARDWIDGETSTATE_HPP
#include <QMouseEvent>

#include "board-widget.hpp"
#include "common.hpp"

class BoardWidgetState {
public:
    enum class MouseState { NORMAL, DRAGGING };
    BoardWidgetState()
        : m_mouseState(MouseState::NORMAL),
          m_selectedField(Coord2D<int>::invalidPos),
          m_draggedField(Coord2D<int>::invalidPos),
          m_dragOffset(Coord2D<int>::invalidPos),
          m_dragStart(Coord2D<int>::invalidPos) {}

    /* Every virtual method returns nullptr by default indicating that
       no state change occurs. */
    bool onMousePress(BoardWidget*, QMouseEvent*);
    bool onMouseMove(BoardWidget*, QMouseEvent*);
    bool onMouseRelease(BoardWidget*, QMouseEvent*);

protected:
    virtual bool _try_to_move(BoardWidget*, Move&);

public:
    MouseState m_mouseState;
    // drag fields are only valid if mouseState is DRAGGING
    Coord2D<int> m_selectedField;
    Coord2D<int> m_draggedField;
    Coord2D<int> m_dragOffset;
    Coord2D<int> m_dragStart;
};

#endif  // BOARDWIDGETSTATE_HPP
