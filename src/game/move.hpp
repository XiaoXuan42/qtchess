#ifndef MOVE_HPP
#define MOVE_HPP
#include "common.hpp"
#include "pieces.hpp"

struct Move {
    // Null move
    static const Move NullMove;

    Coord2D<int> From;
    Coord2D<int> To;
    Piece::Type PromotionPiece;

    Move() {}
    Move(Coord2D<int> From, Coord2D<int> To,
         Piece::Type PromotionTo = Piece::Type::None)
        : From(From), To(To), PromotionPiece(PromotionTo) {}

    bool operator==(const Move& Move) const {
        return Move.From == From && Move.To == To &&
               Move.PromotionPiece == PromotionPiece;
    }

    bool operator!=(const Move& move) const { return !(*this == move); }

    bool operator<(const Move& move) const {
        return from() == move.from() ? to() < move.to() : from() < move.from();
    }

    Coord2D<int> from() const { return From; }

    Coord2D<int> to() const { return To; }

    Piece::Type promotionPiece() const { return PromotionPiece; }
};

#endif  // MOVE_HPP
