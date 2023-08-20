#include "pieces.hpp"

Piece::Piece(QChar symbol) {
    bool isBlack = symbol.isLower();
    QChar lower = symbol.toLower();

    if (lower == 'p')
        mType = Piece::Type::Pawn;
    else if (lower == 'n')
        mType = Piece::Type::Knight;
    else if (lower == 'b')
        mType = Piece::Type::Bishop;
    else if (lower == 'r')
        mType = Piece::Type::Rook;
    else if (lower == 'q')
        mType = Piece::Type::Queen;
    else if (lower == 'k')
        mType = Piece::Type::King;
    else
        mType = Piece::Type::None;

    if (mType == Piece::Type::None)
        mOwner = Player::none();
    else
        mOwner = isBlack ? Player::black() : Player::white();
}

Piece::Piece(Piece::Type type, Player owner) : mType(type), mOwner(owner) {}

QString Piece::symbolString() const {
    switch (mType) {
        case Piece::Type::Pawn:
            return "P";
        case Piece::Type::Knight:
            return "N";
        case Piece::Type::Bishop:
            return "B";
        case Piece::Type::Rook:
            return "R";
        case Piece::Type::Queen:
            return "Q";
        case Piece::Type::King:
            return "K";
        default:
            return "";
    }
}
