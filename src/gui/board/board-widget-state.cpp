#include "gui/board/board-widget-state.hpp"

#include <cmath>

#include "common.hpp"
#include "gui/board/board-widget.hpp"
#include "gui/promotion-dialog.hpp"
#include <iostream>

bool BoardWidgetState::_try_to_move(BoardWidget* Board, Move& move) {
    MoveType moveType = MoveType::MOVE_NONSPECIAL;
    if (Board->getGameState().getBoard().isLegal(move, nullptr, &moveType)) {
        if (moveType == MoveType::MOVE_PROMOTION) {
            PromotionDialog dialog(Board->parentWidget());
            dialog.exec();
            move.PromotionPiece = dialog.selectedPieceType();
        }
        return true;
    }
    return false;
}

bool BoardWidgetState::onMouseRelease(BoardWidget* Board, QMouseEvent* Event) {
    Coord2D<int> Field;

    if (m_mouseState != MouseState::DRAGGING) {
        return false;
    }
    if (Event->button() != Qt::LeftButton) {
        return false;
    }

    bool bRedraw = true;
    if (Board->isFieldAt(Event->position().x(), Event->position().y(), &Field.x,
                         &Field.y)) {
        Move move(m_draggedField, Field);
        m_selectedField = Field;
        if (_try_to_move(Board, move)) {
            Board->emitMove(move);
            bRedraw = false;
        }
    }

    m_mouseState = MouseState::NORMAL;
    m_draggedField = Coord2D<int>::invalidPos;
    return bRedraw;
}

bool BoardWidgetState::onMouseMove(BoardWidget* Board, QMouseEvent* Event) {
    if (m_mouseState != MouseState::DRAGGING) {
        return false;
    }
    m_dragOffset.x = Event->position().x() - m_dragStart.x;
    m_dragOffset.y = Event->position().y() - m_dragStart.y;
    return true;
}

bool BoardWidgetState::onMousePress(BoardWidget* Board, QMouseEvent* Event) {
    if (m_mouseState != MouseState::NORMAL) {
        return false;
    }
    Coord2D<int> Field;

    bool bRedraw = true;
    if (Board->isFieldAt(Event->position().x(), Event->position().y(), &Field.x,
                         &Field.y)) {
        if (m_selectedField == Field) {
            m_selectedField = Coord2D<int>::invalidPos;
        } else {
            Move move(m_selectedField, Field);
            if (m_selectedField != Coord2D<int>::invalidPos && _try_to_move(Board, move)) {
                m_selectedField = Field;
                Board->emitMove(move);
                bRedraw = false;
            } else {
                m_selectedField = Field;
                m_mouseState = MouseState::DRAGGING;
                m_draggedField = Field;
                m_dragOffset = Coord2D<int>(0, 0);
                m_dragStart =
                    Coord2D<int>(Event->position().x(), Event->position().y());
            }
        }
    }
    return bRedraw;
}
