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

#include <cstdint>

constexpr std::uint32_t maxHistoryValue = 10000000;

template <std::uint32_t width, std::uint32_t height>
class HistoryTable
{
protected:
    std::uint32_t scores[width][height];
public:
    constexpr HistoryTable()
    {
        this->reset();
    }

    constexpr ~HistoryTable() = default;

    constexpr std::uint32_t add(std::uint32_t x, std::uint32_t y, std::uint32_t score)
    {
        const std::uint32_t history = this->scores[x][y] + score;

        this->scores[x][y] = history;

        return history;
    }

    constexpr std::uint32_t get(std::uint32_t x, std::uint32_t y) const
    {
        return this->scores[x][y];
    }

    constexpr void reset()
    {
        for (std::uint32_t i = 0; i < width; i++) {
            for (std::uint32_t j = 0; j < height; j++) {
                this->scores[i][j] = 0;
            }
        }
    }
};
