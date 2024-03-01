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

#include <array>
#include <cstdint>

#include "../../game/math/shift.h"

#include "../../game/types/bitboard.h"

#include "../types/direction.h"
#include "../types/square.h"

constexpr std::array<Bitboard, Square::SQUARE_COUNT> SquaresInFrontList
{ []() constexpr {
    std::array<Bitboard, Square::SQUARE_COUNT> result{};

    for (Square src = Square::FIRST_SQUARE; src < Square::SQUARE_COUNT; src++) {
        std::int32_t j = 1;

        while (IsOnBoard(src, Direction::UP * j, Direction::NO_DIRECTION)) {
            const Square dst = src + Direction::UP * j;

            result[src] |= OneShiftedBy(dst);

            j++;
        }
    }

    return result;
}() };

constexpr Bitboard SquaresInFront(Square src)
{
    return SquaresInFrontList[src];
}
