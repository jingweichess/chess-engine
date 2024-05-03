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

#include <cstdint>

#include "../../game/types/bitboard.h"

#include "../types/square.h"

struct Magic {
    const Bitboard magic;
    const Bitboard mask;
    const std::uint32_t shift;
    const std::uint32_t attacks;

    constexpr std::uint32_t index(Bitboard occupied) const {
#ifdef __BMI2__
        return static_cast<std::uint32_t>(Pext(occupied, mask));
#else
        return static_cast<std::uint32_t>(((occupied & mask) * magic) >> shift);
#endif
    }
};

extern std::array<Magic, Square::SQUARE_COUNT> BishopMagicList;
extern std::array<Magic, Square::SQUARE_COUNT> RookMagicList;

extern std::array<Bitboard, 0x1480> BishopAttackTable;
extern std::array<Bitboard, 0x19000> RookAttackTable;

constexpr Bitboard BishopMagic(Square src, Bitboard allPieces)
{
    const Magic& magic = BishopMagicList[src];

    return BishopAttackTable[magic.attacks + magic.index(allPieces)];
}

constexpr Bitboard RookMagic(Square src, Bitboard allPieces)
{
    const Magic& magic = RookMagicList[src];

    return RookAttackTable[magic.attacks + magic.index(allPieces)];
}

constexpr Bitboard QueenMagic(Square src, Bitboard allPieces)
{
    return BishopMagic(src, allPieces) | RookMagic(src, allPieces);
}
