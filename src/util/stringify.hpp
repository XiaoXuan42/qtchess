#ifndef STRINGIFY_HPP
#define STRINGIFY_HPP

#include <QString>

#include "common.hpp"

class Move;
class Board;
class Stringify {
public:
    static QString fileString(int file);
    static QString rankString(int rank);
    static int charToRank(QChar rank);
    static int charToFile(QChar file);
    static QString squareString(const Coord2D<int> &coord);
    static QString longAlgebraicNotationString(const Move &move);
    static QString algebraicNotationString(const Board &board,
                                           const Move &move);

    static Move longAlgebraicNotationToMove(const QString &lan);
};

#endif