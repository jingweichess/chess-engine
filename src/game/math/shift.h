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

#include <array>
#include <cstdint>

constexpr std::array<std::uint64_t, 64> oneShiftedBy { []() constexpr {
    std::array<std::uint64_t, 64> result{};

    for (std::uint32_t i = 0; i < 64; i++) {
        result[i] = 1ull << i;
    }

    return result;
}() };

constexpr std::uint64_t OneShiftedBy(std::uint32_t n) {
    return oneShiftedBy[n];
}

constexpr std::array<std::uint64_t, 64> oneShiftedByMinusOne { []() constexpr {
    std::array<std::uint64_t, 64> result{};

    for (std::uint32_t i = 0; i < 64; i++) {
        result[i] = OneShiftedBy(i) - 1;
    }

    return result;
}() };

constexpr std::uint64_t OneShiftedByMinusOne(std::uint32_t n) {
    return oneShiftedByMinusOne[n];
}
