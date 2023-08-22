#include "util/stringify.hpp"

#include "game/board.hpp"

QString Stringify::fileString(int file) { return QString(char(file + 'a')); }

QString Stringify::rankString(int rank) {
    return QString(char(7 - rank + '1'));
}

int Stringify::charToRank(QChar rank) { return '8' - rank.toLatin1(); }

int Stringify::charToFile(QChar file) { return file.toLatin1() - 'a'; }

QString Stringify::squareString(const Coord2D<int> &coord) {
    return fileString(coord.x) + rankString(coord.y);
}

QString Stringify::longAlgebraicNotationString(const Move &move) {
    return squareString(move.from()) + squareString(move.to()) +
           Piece(move.promotionPiece()).symbolString().toLower();
}

QString Stringify::algebraicNotationString(const Board &board,
                                           const Move &move) {
    Piece piece = board.pieceAt(move.from());
    MoveType moveType;
    BoardState gameState;

    if (!board.isLegal(move, &gameState, &moveType)) return "invalid move";

    QString check = gameState.IsCheck ? "+" : "";

    switch (moveType) {
        case MoveType::MOVE_CASTLE_SHORT:
            return "O-O" + check;
        case MoveType::MOVE_CASTLE_LONG:
            return "O-O-O" + check;
        case MoveType::MOVE_ENPASSANT_CAPTURE:
            return fileString(move.from().x) + "x" + squareString(move.to()) +
                   check;
        case MoveType::MOVE_PROMOTION:
            return squareString(move.to()) + "=" +
                   Piece(move.promotionPiece()).symbolString() + check;
        default:
            break;
    }

    // String that helps avoid move ambiguity
    QString uniqueFrom = "";

    // Now have to take care of the notational ambiguity (e.g. one of two
    // knights on the same file jump to a position reachable for both of them)
    if (!piece.isKing() && !piece.isPawn()) {
        CoordsVector attackingPieces;
        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                Coord2D<int> current(file, rank);

                if (current == move.from() || board.pieceAt(current) != piece)
                    continue;
                if (board.getAttackedCoords(piece, piece.owner(), current)
                        .contains(move.to()))
                    attackingPieces.push_back(current);
            }
        }
        int attackersInSameRank = 0;
        int attackersInSameFile = 0;

        for (const auto &item : attackingPieces) {
            if (item.x == move.from().x) ++attackersInSameFile;
            if (item.y == move.from().y) ++attackersInSameRank;
        }

        if (attackersInSameRank) uniqueFrom += fileString(move.from().x);
        if (attackersInSameFile) uniqueFrom += rankString(move.from().y);

        // Same diagonal
        if (!attackersInSameRank && !attackersInSameFile &&
            attackingPieces.size())
            uniqueFrom += fileString(move.from().x);
    }
    // Capture
    if (!board.pieceAt(move.to()).isNone())
        return (piece.isPawn() ? fileString(move.from().x)
                               : piece.symbolString()) +
               uniqueFrom + "x" + squareString(move.to()) + check;

    // Normal move
    return (piece.isPawn() ? "" : piece.symbolString()) + uniqueFrom +
           squareString(move.to()) + check;
}

Move Stringify::longAlgebraicNotationToMove(const QString &lan) {
    Move move;

    move.From.x = charToFile(lan.at(0));
    move.From.y = charToRank(lan.at(1));
    move.To.x = charToFile(lan.at(2));
    move.To.y = charToRank(lan.at(3));

    // This is a promotion
    if (lan.length() > 4) {
        QChar piece = lan.at(4);

        if (piece == 'b')
            move.PromotionPiece = Piece::Type::Bishop;
        else if (piece == 'r')
            move.PromotionPiece = Piece::Type::Rook;
        else if (piece == 'q')
            move.PromotionPiece = Piece::Type::Queen;
        else
            // Default promotion is funny.
            move.PromotionPiece = Piece::Type::Knight;
    }

    return move;
}
