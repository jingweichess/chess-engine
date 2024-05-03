/*
    Jing Wei, the rebirth of the chess engine I started in 2010
    Copyright(C) 2019-2024 Chris Florin

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

#include "bitboard.h"
#include "square.h"

#include "file.h"
#include "rank.h"

enum Direction {
    NO_DIRECTION = 0,
    UP = -8, DOWN = 8, RIGHT = 1, LEFT = -1,
    UP_RIGHT = UP + RIGHT,
    UP_LEFT = UP + LEFT,
    DOWN_RIGHT = DOWN + RIGHT,
    DOWN_LEFT = DOWN + LEFT,
    TWO_UP = UP + UP,
    TWO_DOWN = DOWN + DOWN,
    UP_LEFT_LEFT = UP + LEFT + LEFT,
    UP_UP_LEFT = UP + UP + LEFT,
    UP_UP_RIGHT = UP + UP + RIGHT,
    UP_RIGHT_RIGHT = UP + RIGHT + RIGHT,
    DOWN_LEFT_LEFT = DOWN + LEFT + LEFT,
    DOWN_DOWN_LEFT = DOWN + DOWN + LEFT,
    DOWN_DOWN_RIGHT = DOWN + DOWN + RIGHT,
    DOWN_RIGHT_RIGHT = DOWN + RIGHT + RIGHT,
    ONE_RANK = 8, ONE_FILE = 1
};

constexpr Direction operator * (Direction d, int i)
{
    return Direction(int(d) * i);
}

constexpr Square operator + (Square s, Direction d)
{
    return Square(int(s) + int(d));
}

constexpr Square operator - (Square s, Direction d)
{
    return Square(int(s) - int(d));
}

constexpr Square operator += (Square s, Direction d)
{
    return s = s + d;
}

constexpr Bitboard operator + (Bitboard b, Direction d)
{
    if (d < 0) {
        return b >> (-d);
    }
    else {
        return b << d;
    }
}

constexpr Bitboard operator - (Bitboard b, Direction d)
{
    if (d < 0) {
        return b << (-d);
    }
    else {
        return b >> d;
    }
}

constexpr Bitboard operator += (Bitboard b, Direction d)
{
    return b = b + d;
}

constexpr File operator + (File f, Direction dir)
{
    return File(int(f) + int(dir));
}

constexpr Rank operator + (Rank r, Direction d)
{
    const Rank dr = GetRank(Square(int(d)));

    return Rank(r + int(dr));
}

constexpr bool IsOnBoard(Square src, Direction dr, Direction df)
{
    Rank rank = GetRank(src);
    File file = GetFile(src);

    rank = rank + dr;
    file = file + df;

    if ((rank > Rank::_8) || (rank < Rank::_1)) { return false; }
    if ((file < File::_A) || (file > File::_H)) { return false; }

    return true;
}
