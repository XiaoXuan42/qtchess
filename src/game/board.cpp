#include "board.hpp"

#include <QDebug>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "util/stringify.hpp"

const BoardState Board::InitialGameState = {
    .ShortCastlingRight = {{Player::white(), true}, {Player::black(), true}},
    .LongCastlingRight = {{Player::white(), true}, {Player::black(), true}},
    .IsCheck = false,
    .WhoIsPlaying = Player::white(),
    .EnPassantPlayer = Player::none(),
    .EnPassantCoords = {-1, -1},
    .HalfMoveClock = 0,
    .FullMoveCounter = 1};

const Coord2D<int> Board::A1 = Coord2D<int>(0, 7);
const Coord2D<int> Board::B1 = Coord2D<int>(1, 7);
const Coord2D<int> Board::C1 = Coord2D<int>(2, 7);
const Coord2D<int> Board::D1 = Coord2D<int>(3, 7);
const Coord2D<int> Board::F1 = Coord2D<int>(5, 7);
const Coord2D<int> Board::G1 = Coord2D<int>(6, 7);
const Coord2D<int> Board::H1 = Coord2D<int>(7, 7);

const Coord2D<int> Board::F8 = Coord2D<int>(5, 0);
const Coord2D<int> Board::G8 = Coord2D<int>(6, 0);
const Coord2D<int> Board::C8 = Coord2D<int>(2, 0);
const Coord2D<int> Board::D8 = Coord2D<int>(3, 0);
const Coord2D<int> Board::A8 = Coord2D<int>(0, 0);
const Coord2D<int> Board::H8 = Coord2D<int>(7, 0);
const Coord2D<int> Board::B8 = Coord2D<int>(1, 0);

Board::Board()
    : m_state(Board::InitialGameState),
      m_position(Position::defaultPosition()) {}

bool Board::setFen(QString fen) {
    QStringList tokens = fen.split(' ');

    if (tokens.size() != 6) return false;

    // Start with empty position
    Position position = Position::emptyPosition();
    BoardState state;

    // And no castling rights.
    state.LongCastlingRight[Player::white()] = false;
    state.LongCastlingRight[Player::black()] = false;
    state.ShortCastlingRight[Player::white()] = false;
    state.ShortCastlingRight[Player::black()] = false;

    QString positionStr = tokens[0];
    QString sideToMove = tokens[1];
    QString castling = tokens[2];
    QString enpassant = tokens[3];
    QString halfMoves = tokens[4];
    QString fullMoves = tokens[5];
    int processedSquares = 0;

    for (int rank = 0, i = 0; rank < 8; ++rank, ++i) {
        for (int file = 0; file < 8; ++file, ++i) {
            if (positionStr[i].isDigit()) {
                int skip = positionStr[i].digitValue() - 1;
                file += skip;
                processedSquares += skip + 1;
            } else {
                ++processedSquares;
                Piece piece(positionStr[i]);

                // Invalid piece symbol
                if (piece.isNone()) return false;

                position.setPieceAt(file, rank, piece);
            }
        }
    }

    if (processedSquares != 64) return false;

    if (sideToMove == "b")
        state.WhoIsPlaying = Player::black();
    else if (sideToMove == "w")
        state.WhoIsPlaying = Player::white();
    else
        return false;

    for (int i = 0; i < castling.length(); ++i) {
        if (castling[i] == '-') break;
        Player player =
            castling[i].isLower() ? Player::black() : Player::white();

        if (castling[i].toLower() == 'k')
            state.ShortCastlingRight[player] = true;
        else if (castling[i].toLower() == 'q')
            state.LongCastlingRight[player] = true;
        else
            return false;
    }

    if (enpassant != "-") {
        if (enpassant.length() < 2) return false;

        int file = Stringify::charToFile(enpassant[0]);
        int rank = Stringify::charToRank(enpassant[1]);

        if (!isLegalCoord(file, rank)) return false;

        state.EnPassantCoords = {file, rank};
        state.EnPassantPlayer = state.WhoIsPlaying.opponent();
    }

    int numHalfMoves = halfMoves.toInt();
    int numFullMoves = fullMoves.toInt();

    if (numHalfMoves < 0 || numFullMoves < 1) return false;

    state.HalfMoveClock = numHalfMoves;
    state.FullMoveCounter = numFullMoves;

    m_position = position;
    m_state = state;

    return true;
}

QString Board::toFen() const {
    QString fen;

    for (int rank = 0; rank < 8; ++rank) {
        QVector<char> fileString;

        for (int file = 0; file < 8; ++file) {
            const Piece& piece = pieceAt(file, rank);

            if (!piece.isNone()) {
                if (piece.owner().isBlack())
                    fileString.append(
                        piece.symbolString().toLower().at(0).toLatin1());
                else
                    fileString.append(
                        piece.symbolString().toUpper().at(0).toLatin1());
            } else {
                // Last element is a number, and it is still empty, this means
                // there is more empty squares.
                if (fileString.size() && isdigit(fileString.last()))
                    ++fileString.last();
                else
                    fileString.append('1');
            }
        }

        for (char element : fileString) fen += element;

        fen += '/';
    }
    // Remove last slash
    fen.chop(1);

    // Side to move
    fen += (currentPlayer().isWhite() ? " w " : " b ");

    // Castling rights
    if (!hasShortCastlingRights(Player::white()) &&
        !hasShortCastlingRights(Player::black()) &&
        !hasLongCastlingRights(Player::black()) &&
        !hasLongCastlingRights(Player::white()))
        fen += " - ";
    else {
        fen += hasShortCastlingRights(Player::white()) ? "K" : "";
        fen += hasLongCastlingRights(Player::white()) ? "Q" : "";
        fen += hasShortCastlingRights(Player::black()) ? "k" : "";
        fen += hasLongCastlingRights(Player::black()) ? "q" : "";
    }

    // En passant
    if (isLegalCoord(m_state.EnPassantCoords.x, m_state.EnPassantCoords.y))
        fen += QString(" %1 ").arg(
            Stringify::squareString(m_state.EnPassantCoords));
    else
        fen += " - ";
    // Move clocks
    fen += QString("%1 %2").arg(QString::number(m_state.HalfMoveClock),
                                QString::number(m_state.FullMoveCounter));

    return fen;
}

int Board::fullMoveCount() const { return m_state.FullMoveCounter; }

const Piece& Board::pieceAt(const Coord2D<int>& coord) const {
    return pieceAt(coord.x, coord.y);
}

const Piece& Board::pieceAt(int x, int y) const {
    assert(isLegalCoord(x, y));
    return m_position.pieceAt(x, y);
}

Player Board::owner(const Coord2D<int>& coord) const {
    return owner(coord.x, coord.y);
}

Player Board::owner(int x, int y) const {
    assert(isLegalCoord(x, y));
    return m_position.pieceAt(x, y).owner();
}

bool Board::makeMove(Move move) {
    if (move == move.NullMove) {
        return true;
    }
    BoardState nextState;
    MoveType type = MoveType::MOVE_NONSPECIAL;
    if (!isLegal(move, &nextState, &type)) return false;
    movePieces(move, type);
    m_state = nextState;
    // Made the move
    return true;
}

bool Board::isCheck() const {
    return countChecksFor(Player::white()) > 0 ||
           countChecksFor(Player::black()) > 0;
}

bool Board::isCheckmate() const {
    // TODO: stub
    return false;
}

bool Board::hasShortCastlingRights(const Player& player) const {
    return m_state.ShortCastlingRight.at(player);
}

bool Board::hasLongCastlingRights(const Player& player) const {
    return m_state.LongCastlingRight.at(player);
}

void Board::movePieces(Move move, MoveType type) {
    int sx = move.From.x, sy = move.From.y;
    int dx = move.To.x, dy = move.To.y;

    if (type == MoveType::MOVE_CASTLE_SHORT) {
        m_position.setPieceAt(dx - 1, dy, m_position.pieceAt(dx + 1, dy));
        m_position.setPieceAt(dx + 1, dy, Piece());
    } else if (type == MoveType::MOVE_CASTLE_LONG) {
        m_position.setPieceAt(dx + 1, dy, m_position.pieceAt(dx - 2, dy));
        m_position.setPieceAt(dx - 2, dy, Piece());
    } else if (type == MoveType::MOVE_ENPASSANT_CAPTURE) {
        int y =
            m_state.EnPassantCoords.y + (currentPlayer().isWhite() ? +1 : -1);
        int x = m_state.EnPassantCoords.x;
        m_position.setPieceAt(x, y, Piece());
    }

    m_position.setPieceAt(dx, dy, m_position.pieceAt(sx, sy));
    m_position.setPieceAt(sx, sy, Piece());

    if (type == MoveType::MOVE_PROMOTION)
        m_position.setPieceAt(dx, dy,
                              Piece(move.PromotionPiece, currentPlayer()));
}

bool Board::canCastle(MoveType castleType) const {
    static std::map<Player, CoordsVector> shortKingWalkSquares = {
        {Player::white(), {F1, G1}}, {Player::black(), {F8, G8}}};
    static std::map<Player, CoordsVector> longKingWalkSquares = {
        {Player::white(), {D1, C1}}, {Player::black(), {D8, C8}}};

    if (isCheck()) return false;

    int numKingWalkUnderAttack = 0;
    Player current = currentPlayer();
    Player next = current.opponent();

    if (castleType == MoveType::MOVE_CASTLE_SHORT &&
        hasShortCastlingRights(current)) {
        for (const auto& step : shortKingWalkSquares[current])
            numKingWalkUnderAttack += countAttacksFor(step, next);
    } else if (castleType == MoveType::MOVE_CASTLE_LONG &&
               hasLongCastlingRights(current)) {
        for (const auto& step : longKingWalkSquares[current])
            numKingWalkUnderAttack += countAttacksFor(step, next);
    } else
        return false;

    return numKingWalkUnderAttack == 0;
}

bool Board::isInCheckAfterTheMove(Player victim, Move move,
                                  MoveType type) const {
    Position backup = m_position;
    // Yes, I call a non-const method, but after this I restore saved
    // state.
    const_cast<Board*>(this)->movePieces(move, type);
    int numChecks = countChecksFor(victim);
    // Restore board position
    m_position = backup;
    return numChecks > 0;
}

bool Board::isLegal(Move move, BoardState* retState, MoveType* retType) const {
    BoardState NextState = m_state;

    // Cannot take your own piece
    if (owner(move.To) == currentPlayer()) return false;
    // Cannot move your opponent piece
    if (owner(move.From) != currentPlayer()) return false;

    bool legal = false;
    bool moveIsCastle = false;
    MoveType type = MoveType::MOVE_NONSPECIAL;
    Coord2D<int> enPassantCoords = {-1, -1};

    switch (pieceAt(move.From).type()) {
        case Piece::Type::Pawn:
            legal = isLegalPawnMove(move, type, enPassantCoords);
            break;
        case Piece::Type::Knight:
            legal = isLegalKnightMove(move);
            break;
        case Piece::Type::Bishop:
            legal = isLegalBishopMove(move);
            break;
        case Piece::Type::Rook:
            legal = isLegalRookMove(move);
            break;
        case Piece::Type::Queen:
            legal = isLegalQueenMove(move);
            break;
        case Piece::Type::King:
            legal = isLegalKingMove(move, moveIsCastle, type);
            break;
        default:
            break;
    }

    if (!legal || isInCheckAfterTheMove(currentPlayer(), move, type))
        return false;
    if (moveIsCastle && !canCastle(type)) return false;

    // King movement takes away castling rights
    if (pieceAt(move.From).isKing()) {
        NextState.ShortCastlingRight[currentPlayer()] = false;
        NextState.LongCastlingRight[currentPlayer()] = false;
    } else if (pieceAt(move.From).isRook()) {
        // Rook movement too
        if (move.From == H1)
            NextState.ShortCastlingRight[Player::white()] = false;
        else if (move.From == A1)
            NextState.LongCastlingRight[Player::white()] = false;
        else if (move.From == H8)
            NextState.ShortCastlingRight[Player::black()] = false;
        else if (move.From == A8)
            NextState.LongCastlingRight[Player::black()] = false;
    }
    NextState.WhoIsPlaying = currentPlayer().opponent();
    NextState.IsCheck =
        isInCheckAfterTheMove(currentPlayer().opponent(), move, type);

    // Half move clock update
    if (pieceAt(move.from()).isPawn() || owner(move.to()) != Player::none() ||
        type == MoveType::MOVE_ENPASSANT_CAPTURE)
        NextState.HalfMoveClock = 0;
    else
        ++NextState.HalfMoveClock;
    // Full move counter update
    if (pieceAt(move.from()).owner().isBlack()) ++NextState.FullMoveCounter;
    // En-passant target update
    if (enPassantCoords == m_state.EnPassantCoords)
        NextState.EnPassantCoords = {-1, -1};
    else {
        NextState.EnPassantCoords = enPassantCoords;
        NextState.EnPassantPlayer = currentPlayer().opponent();
    }

    if (retState) *retState = NextState;
    if (retType) *retType = type;
    return true;
}

bool Board::isLegalPawnMove(Move move, MoveType& Type,
                            Coord2D<int>& EnPassantCoords) const {
    // No backward movement
    if (move.From.y - move.To.y < 0 && currentPlayer().isWhite()) return false;
    if (move.From.y - move.To.y > 0 && currentPlayer().isBlack()) return false;

    int Distance = std::abs(move.To.y - move.From.y);
    if (Distance == 2) {
        // Not really a good rank to start such crazy things
        if ((currentPlayer().isWhite() && move.From.y != 6) ||
            (currentPlayer().isBlack() && move.From.y != 1))
            return false;

        if (!isLegalRookMove(move)) return false;

        // Did we generate EnPassant?
        Coord2D<int> Neighbour[] = {{move.To.x - 1, move.To.y},
                                    {move.To.x + 1, move.To.y}};
        for (int i = 0; i < 2; i++) {
            if (!isLegalCoord(Neighbour[i])) continue;
            if (pieceAt(Neighbour[i]).isPawn() &&
                owner(Neighbour[i]) != currentPlayer()) {
                // Yes, we did
                Type = MoveType::MOVE_ENPASSANT_GENERATE;
            }
        }
        EnPassantCoords.x = move.to().x;
        EnPassantCoords.y = move.to().y + (currentPlayer().isWhite() ? +1 : -1);

        return true;
    } else if (Distance == 1) {
        CoordsVector Attacks =
            getPawnAttack(move.From.x, move.From.y, currentPlayer());
        bool Promotion = (currentPlayer().isWhite() && move.To.y == 0) ||
                         (currentPlayer().isBlack() && move.To.y == 7);

        // Capture
        if (std::find(Attacks.begin(), Attacks.end(), move.To) !=
            Attacks.end()) {
            // Normal capture
            if (!pieceAt(move.To).isNone() &&
                owner(move.To) != currentPlayer()) {
                if (Promotion) Type = MoveType::MOVE_PROMOTION;
                return true;
            }
            // En-passant
            else if (m_state.EnPassantPlayer == currentPlayer() &&
                     m_state.EnPassantCoords == move.To) {
                Type = MoveType::MOVE_ENPASSANT_CAPTURE;
                return true;
            }
        } else if (pieceAt(move.To).isNone() && isLegalRookMove(move)) {
            if (Promotion) Type = MoveType::MOVE_PROMOTION;
            return true;
        }
        return false;
    }
    return false;
}

bool Board::isLegalKnightMove(Move move) const {
    return getKnightAttack(move.From.x, move.From.y).contains(move.to());
}

bool Board::isLegalBishopMove(Move move) const {
    return getBishopAttack(move.From.x, move.From.y).contains(move.to());
}

bool Board::isLegalRookMove(Move move) const {
    return getRookAttack(move.From.x, move.From.y).contains(move.to());
}

bool Board::isLegalQueenMove(Move move) const {
    return isLegalBishopMove(move) || isLegalRookMove(move);
}

bool Board::isLegalKingMove(Move move, bool& moveIsCastle,
                            MoveType& side) const {
    // Normal king movement
    moveIsCastle = false;
    if (getKingAttack(move.From.x, move.From.y).contains(move.to()))
        return true;

    // Castling move
    bool legal = false;
    if (move.To == G1)
        side = MoveType::MOVE_CASTLE_SHORT,
        legal = hasShortCastlingRights(Player::white());
    else if (move.To == C1 && pieceAt(B1).isNone())
        side = MoveType::MOVE_CASTLE_LONG,
        legal = hasLongCastlingRights(Player::white());
    else if (move.To == G8)
        side = MoveType::MOVE_CASTLE_SHORT,
        legal = hasShortCastlingRights(Player::black());
    else if (move.To == C8 && pieceAt(B8).isNone())
        side = MoveType::MOVE_CASTLE_LONG,
        legal = hasLongCastlingRights(Player::black());

    // It is not enough to check if this is the right castle,
    // we need to see there is no blocking pieces on the way.
    if (legal && isLegalRookMove(move)) return moveIsCastle = true;
    return false;
}

bool Board::isLegalCoord(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::isLegalCoord(Coord2D<int> Coord) const {
    return isLegalCoord(Coord.x, Coord.y);
}

int Board::countAttacksFor(Coord2D<int> coord, Player attacker) const {
    int numAttacks = 0;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            if (pieceAt(i, j).isNone() || owner(i, j) != attacker) continue;
            if (getAttackedCoords(pieceAt(i, j), attacker, Coord2D<int>(i, j))
                    .contains(coord))
                ++numAttacks;
        }
    }
    return numAttacks;
}

int Board::countChecksFor(Player player) const {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (owner(x, y) == player && pieceAt(x, y).isKing())
                return countAttacksFor(Coord2D<int>(x, y), player.opponent());

    assert(!"Amazingly we've lost the King");
    return 0;
}

CoordsVector Board::getAttackedCoords(Piece piece, Player owner,
                                      Coord2D<int> position) const {
    int x = position.x;
    int y = position.y;

    switch (piece.type()) {
        case Piece::Type::Pawn:
            return getPawnAttack(x, y, owner);
        case Piece::Type::Knight:
            return getKnightAttack(x, y);
        case Piece::Type::Bishop:
            return getBishopAttack(x, y);
        case Piece::Type::Rook:
            return getRookAttack(x, y);
        case Piece::Type::Queen:
            return getQueenAttack(x, y);
        case Piece::Type::King:
            return getKingAttack(x, y);
        default:
            return {};
    }
}

Player Board::currentPlayer() const { return m_state.WhoIsPlaying; }

CoordsVector Board::getPawnAttack(int x, int y, Player owner) const {
    CoordsVector AttackedCoords;
    int NextY = owner.isWhite() ? y - 1 : y + 1;
    if (isLegalCoord(x + 1, NextY)) AttackedCoords.push_back({x + 1, NextY});
    if (isLegalCoord(x - 1, NextY)) AttackedCoords.push_back({x - 1, NextY});
    return AttackedCoords;
}

CoordsVector Board::getBishopAttack(int x, int y) const {
    CoordsVector LegalMoves;

    for (int p = x + 1, q = y + 1; isLegalCoord(p, q); p++, q++) {
        LegalMoves.push_back({p, q});
        if (pieceAt(p, q).type() != Piece::Type::None) break;
    }

    for (int p = x - 1, q = y - 1; isLegalCoord(p, q); p--, q--) {
        LegalMoves.push_back({p, q});
        if (pieceAt(p, q).type() != Piece::Type::None) break;
    }

    for (int p = x + 1, q = y - 1; isLegalCoord(p, q); p++, q--) {
        LegalMoves.push_back({p, q});
        if (pieceAt(p, q).type() != Piece::Type::None) break;
    }

    for (int p = x - 1, q = y + 1; isLegalCoord(p, q); p--, q++) {
        LegalMoves.push_back({p, q});
        if (pieceAt(p, q).type() != Piece::Type::None) break;
    }

    return LegalMoves;
}

CoordsVector Board::getKnightAttack(int x, int y) const {
    static CoordsVector Jumps = {{1, 2},  {-1, 2},  {2, 1},  {2, -1},
                                 {1, -2}, {-1, -2}, {-2, 1}, {-2, -1}};
    CoordsVector AttackedCoords;
    for (Coord2D<int>& Coord : Jumps)
        if (isLegalCoord(Coord.x + x, Coord.y + y))
            AttackedCoords.push_back({Coord.x + x, Coord.y + y});
    return AttackedCoords;
}

CoordsVector Board::getRookAttack(int x, int y) const {
    CoordsVector LegalMoves;

    for (int i = x + 1; isLegalCoord(i, y); i++) {
        LegalMoves.push_back({i, y});
        if (pieceAt(i, y).type() != Piece::Type::None) break;
    }

    for (int i = x - 1; isLegalCoord(i, y); i--) {
        LegalMoves.push_back({i, y});
        if (pieceAt(i, y).type() != Piece::Type::None) break;
    }

    for (int i = y + 1; isLegalCoord(x, i); i++) {
        LegalMoves.push_back({x, i});
        if (pieceAt(x, i).type() != Piece::Type::None) break;
    }

    for (int i = y - 1; isLegalCoord(x, i); i--) {
        LegalMoves.push_back({x, i});
        if (pieceAt(x, i).type() != Piece::Type::None) break;
    }
    return LegalMoves;
}

CoordsVector Board::getQueenAttack(int x, int y) const {
    return getRookAttack(x, y) + getBishopAttack(x, y);
}

CoordsVector Board::getKingAttack(int x, int y) const {
    static CoordsVector Moves = {{0, 1},  {0, -1},  {1, 0},  {-1, 0},
                                 {1, -1}, {-1, -1}, {-1, 1}, {1, 1}};
    CoordsVector AttackedCoords;
    for (Coord2D<int>& Coord : Moves)
        if (isLegalCoord(Coord.x + x, Coord.y + y))
            AttackedCoords.push_back({Coord.x + x, Coord.y + y});
    return AttackedCoords;
}
