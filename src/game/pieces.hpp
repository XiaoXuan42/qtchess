#ifndef PIECES_HPP
#define PIECES_HPP
#include <QString>

#include "game/player.hpp"

class Piece {
public:
    enum class Type {
        None = -1,
        Pawn = 0,
        Knight = 1,
        Bishop = 2,
        Rook = 3,
        Queen = 4,
        King = 5
    };

    /*! \brief returns one-character string piece representation */
    QString symbolString() const;

    /*! \brief Constructs piece from symbol. */
    explicit Piece(QChar symbol);

    /*! \brief constructs piece owned by given player */
    explicit Piece(Piece::Type type = Piece::Type::None,
                   Player owner = Player::none());

    /*! \brief checks whether this is a none-piece */
    inline bool isNone() const { return mType == Type::None; }

    /*! \brief checks whether piece is a pawn */
    inline bool isPawn() const { return mType == Type::Pawn; }

    /*! \brief checks whether piece is a knight */
    inline bool isKnight() const { return mType == Type::Knight; }

    /*! \brief checks whether piece is a bishop */
    inline bool isBishop() const { return mType == Type::Bishop; }

    /*! \brief checks whether piece is a rook */
    inline bool isRook() const { return mType == Type::Rook; }

    /*! \brief checks whether piece is a queen */
    inline bool isQueen() const { return mType == Type::Queen; }

    /*! \brief checks whether piece is a king */
    inline bool isKing() const { return mType == Type::King; }

    /*! \brief returns piece owner */
    inline Player owner() const { return mOwner; }

    /*! \brief returns piece type */
    inline Piece::Type type() const { return mType; }

    bool inline operator==(const Piece& piece) const {
        return mType == piece.type() && mOwner == piece.owner();
    }

    bool inline operator!=(const Piece& piece) const {
        return !(*this == piece);
    }

    bool inline operator<(const Piece& piece) const {
        return static_cast<int>(mType) < static_cast<int>(piece.type());
    }

private:
    Piece::Type mType;
    Player mOwner;
};

#if 0
enum Piece {
    PIECE_NONE   = -1, /* empty piece */
    PIECE_PAWN   = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK   = 3,
    PIECE_KING   = 4,
    PIECE_QUEEN  = 5
};
#endif

#endif
