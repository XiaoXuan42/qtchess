#ifndef COMMON_HPP
#define COMMON_HPP
#include <QVector>
#include <algorithm>

template <typename T>
struct Coord2D {
    T x, y;

    Coord2D() : x(0), y(0) {}
    Coord2D(T x, T y) : x(x), y(y) {}

    static inline Coord2D<T> invalidPos = Coord2D<T>(-1, -1);

    Coord2D operator-(const Coord2D& rhs) {
        return Coord2D(x - rhs.x, y - rhs.y);
    }

    Coord2D operator+(const Coord2D& rhs) {
        return Coord2D(x + rhs.x, y + rhs.y);
    }

    bool operator==(const Coord2D& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Coord2D& rhs) const {
        return x != rhs.x || y != rhs.y;
    }

    bool operator<(const Coord2D& rhs) const {
        return x == rhs.x ? y < rhs.y : x < rhs.x;
    }
};

typedef QVector<Coord2D<int>> CoordsVector;

#endif  // COMMON_HPP
