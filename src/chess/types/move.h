/*
    Jing Wei, the rebirth of the chess engine I started in 2010
    Copyright(C) 2019-2023 Chris Florin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "piecetype.h"
#include "square.h"

#include "../types/castlerights.h"

#include "../../game/types/color.h"
#include "../../game/types/hash.h"
#include "../../game/types/movelist.h"
#include "../../game/types/score.h"

enum ChessMoveOrdinal {
    NO_CHESS_MOVE_ORDINAL = 0,
    PV_MOVE =             -10000000,
    GOOD_CAPTURE_MOVE =   -20000000,
    PROMOTION_MOVE =      -30000000,
    EQUAL_CAPTURE_MOVE =  -40000000,
    KILLER1_MOVE =        -50000000,
    KILLER2_MOVE =        -60000000,
    HISTORY_MOVE =        -70000000,
    UNCLASSIFIED_MOVE =   -80000000,
    BAD_CAPTURE_MOVE =    -90000000,
    UNSAFE_MOVE =        -100000000,

    QUIESENCE_KILLER1_MOVE = -1000000,
    QUIESENCE_KILLER2_MOVE = -2000000,
    GOOD_QUIESENCE_MOVE =   -30000000,
    QUIESENCE_MOVE =        -40000000,
    BAD_QUIESENCE_MOVE =    -90000000
};

constexpr ChessMoveOrdinal NoChessMoveOrdinal = ChessMoveOrdinal::NO_CHESS_MOVE_ORDINAL;

constexpr ChessMoveOrdinal operator + (ChessMoveOrdinal cmo1, ChessMoveOrdinal cmo2)
{
    return ChessMoveOrdinal(int(cmo1) + int(cmo2));
}

struct ChessMove {
    Square src;
    Square dst;
	PieceType promotionPiece = PieceType::NO_PIECE;

    Score seeScore = INVALID_SCORE;
    ChessMoveOrdinal ordinal = ChessMoveOrdinal::NO_CHESS_MOVE_ORDINAL;

    PieceType capturedPiece = PieceType::NO_PIECE;
    PieceType movedPiece = PieceType::NO_PIECE;
};

using ChessMoveList = MoveList<ChessMove>;
using ChessMoveIterator = ChessMoveList::iterator;

constexpr ChessMove NullMove { Square::NO_SQUARE, Square::NO_SQUARE };

constexpr bool operator == (const ChessMove& m1, const ChessMove& m2)
{
    return m1.src == m2.src
        && m1.dst == m2.dst
        && m1.promotionPiece == m2.promotionPiece;
}

constexpr bool operator != (const ChessMove& m1, const ChessMove& m2)
{
    return m1.src != m2.src
        || m1.dst != m2.dst
        || m1.promotionPiece != m2.promotionPiece;
}

constexpr bool operator > (ChessMove m1, ChessMove m2)
{
    return m1.ordinal > m2.ordinal;
}
