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

#include <array>
#include <cstdint>

#include "../../game/types/bitboard.h"

#include "../types/direction.h"
#include "../types/square.h"

std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Square::SQUARE_COUNT> InBetweenList
{ []() constexpr {
    std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Square::SQUARE_COUNT> result{};

    for (Square src = Square::FIRST_SQUARE; src < Square::SQUARE_COUNT; src++) {
        std::int32_t j = 1;
        Bitboard CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::DOWN * j, Direction::RIGHT * j)) {
            result[src][src + Direction::DOWN_RIGHT * j] = CurrentInBetweenBoard;
            result[src + Direction::DOWN_RIGHT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::DOWN_RIGHT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::DOWN * j, Direction::LEFT * j)) {
            result[src][src + Direction::DOWN_LEFT * j] = CurrentInBetweenBoard;
            result[src + Direction::DOWN_LEFT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::DOWN_LEFT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::UP * j, Direction::RIGHT * j)) {
            result[src][src + Direction::UP_RIGHT * j] = CurrentInBetweenBoard;
            result[src + Direction::UP_RIGHT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::UP_RIGHT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::UP * j, Direction::LEFT * j)) {
            result[src][src + Direction::UP_LEFT * j] = CurrentInBetweenBoard;
            result[src + Direction::UP_LEFT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::UP_LEFT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::NO_DIRECTION, Direction::RIGHT * j)) {
            result[src][src + Direction::RIGHT * j] = CurrentInBetweenBoard;
            result[src + Direction::RIGHT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::RIGHT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::DOWN * j, Direction::NO_DIRECTION)) {
            result[src][src + Direction::DOWN * j] = CurrentInBetweenBoard;
            result[src + Direction::DOWN * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::DOWN * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::NO_DIRECTION, Direction::LEFT * j)) {
            result[src][src + Direction::LEFT * j] = CurrentInBetweenBoard;
            result[src + Direction::LEFT * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::LEFT * j);
            j++;
        }

        j = 1;
        CurrentInBetweenBoard = EmptyBitboard;

        while (IsOnBoard(src, Direction::UP * j, Direction::NO_DIRECTION)) {
            result[src][src + Direction::UP * j] = CurrentInBetweenBoard;
            result[src + Direction::UP * j][src] = CurrentInBetweenBoard;

            CurrentInBetweenBoard |= OneShiftedBy(src + Direction::UP * j);
            j++;
        }
    }

    return result;
}() };
