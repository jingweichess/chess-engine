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

#include <bit>
#include <cstdint>

constexpr std::uint32_t PopCount(std::uint64_t n)
{
    return std::popcount(n);
}

constexpr bool PopCountIsOne(std::uint64_t n)
{
    return n != 0 && (n & (n - 1)) == 0;

    //return std::has_single_bit(n);
}
