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

#include <algorithm>
#include <string>
#include <sstream>

#include "../../game/math/shift.h"

#include "../../game/types/bitboard.h"

#include "file.h"
#include "rank.h"

enum Square : std::int32_t {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
    SQUARE_COUNT,
    NO_SQUARE,
    FIRST_SQUARE = A8,
    LAST_SQUARE = H1
};

using SquareBitboardIterator = ForwardBitScanBitboardIterator<Square>;

constexpr Bitboard DarkSquares = 0x55aa55aa55aa55aaull;
constexpr Bitboard LightSquares = 0xaa55aa55aa55aa55ull;

constexpr Bitboard EdgeSquares = 0xff818181818181ffull;

constexpr Bitboard GetDarkSquares(Bitboard squares)
{
    return squares & DarkSquares;
}

constexpr Bitboard GetLightSquares(Bitboard squares)
{
    return squares & LightSquares;
}

constexpr bool IsDarkSquare(Square src)
{
    return (DarkSquares & OneShiftedBy(src)) != EmptyBitboard;
}

constexpr bool IsLightSquare(Square src)
{
    return (LightSquares & OneShiftedBy(src)) != EmptyBitboard;
}

constexpr Bitboard SquaresOppositeColorAs(Bitboard squares, Square src)
{
    return IsDarkSquare(src) ? GetLightSquares(squares) : GetDarkSquares(squares);
}

constexpr Bitboard SquaresSameColorAs(Bitboard squares, Square src)
{
    return IsDarkSquare(src) ? GetDarkSquares(squares) : GetLightSquares(squares);
}

constexpr bool AreSquaresSameColor(Square src, Square dst)
{
    return IsLightSquare(src) == IsLightSquare(dst);
}

constexpr Square FlipSquareOnHorizontalLine(Square src)
{
    return Square(std::uint32_t(src) ^ 56);
}

constexpr Square FlipSquareOnVerticalLine(Square src)
{
    return Square(std::uint32_t(src) ^ 7);
}

Square StringToSquare(const std::string& str);

constexpr Square& operator ++ (Square& s, int)
{
    return s = Square(int(s) + 1);
}

constexpr Square operator ^ (Square src, Square dst)
{
    return static_cast<Square>(static_cast<std::uint32_t>(src) ^ static_cast<std::uint32_t>(dst));
}

constexpr Bitboard operator | (Bitboard b, Square s)
{
    return b | OneShiftedBy(s);
}

constexpr Bitboard& operator |= (Bitboard& b, Square s)
{
    return b = b | s;
}

constexpr Bitboard operator & (Bitboard b, Square s)
{
    return b & OneShiftedBy(s);
}

constexpr Bitboard& operator &= (Bitboard& b, Square s)
{
    return b = b & s;
}

constexpr Bitboard operator ^ (Bitboard b, Square s)
{
    return b ^ OneShiftedBy(s);
}

constexpr Bitboard& operator ^= (Bitboard& b, Square s)
{
    return b = b ^ s;
}

constexpr Square operator * (File f, Rank r)
{
    return Square(int(f) + 8 * int(r));
}

constexpr Square operator * (Rank r, File f)
{
    return f * r;
}

constexpr File GetFile(Square s)
{
    return File(int(s) % 8);
}

constexpr Rank GetRank(Square s)
{
    return Rank(int(s) / 8);
}

constexpr Square SetRank(Square s, Rank r)
{
    return Square((int(s) & 7) + (int(r) << 3));
}

static Rank RankDistance(Square s1, Square s2)
{
    return Rank(std::abs(GetRank(s1) - GetRank(s2)));
}

static File FileDistance(Square s1, Square s2)
{
    return File(std::abs(GetFile(s1) - GetFile(s2)));
}

class SquareIterator {
private:
    Square currentState;

public:
    explicit constexpr SquareIterator(Square beginState = Square::FIRST_SQUARE) : currentState(beginState) {}

    constexpr bool operator==(const SquareIterator& other) const
    {
        return this->currentState == other.currentState;
    }

    constexpr bool operator!=(const SquareIterator& other) const
    {
        return this->currentState != other.currentState;
    }

    constexpr Square operator*() const
    {
        return this->currentState;
    }

    constexpr SquareIterator& operator++()
    {
        this->currentState++;

        return *this;
    }

    constexpr SquareIterator begin() const
    {
        return SquareIterator{ Square::FIRST_SQUARE };
    }

    constexpr SquareIterator end() const
    {
        return SquareIterator{ Square::SQUARE_COUNT };
    }
};

static const std::string FilePrintUpperCase = "ABCDEFGH";
static const std::string FilePrintLowerCase = "abcdefgh";
static const std::string RankPrint = "87654321";

static std::string SquareToString(Square src, bool lowercase = true)
{
    const File srcFile = GetFile(src);
    const Rank srcRank = GetRank(src);

    const char* file = lowercase ? FilePrintLowerCase.c_str() : FilePrintUpperCase.c_str();
    const char* rank = RankPrint.c_str();

    char f = file[srcFile];
    char r = rank[srcRank];

    std::ostringstream ss;

    ss << f << r;

    return ss.str();
}
