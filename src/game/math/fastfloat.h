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

#include "bitscan.h"

struct FixedPoint16
{
    union {
        struct {
            std::int16_t decimal;
            std::int16_t integer;
        } parts;

        std::uint32_t all;
    };

    FixedPoint16()
    {
        FixedPoint16(0, 0);
    }

    FixedPoint16(std::int16_t integer, std::int16_t decimal) {
        this->parts.integer = integer;
        this->parts.decimal = decimal;
    }

    std::int32_t log()
    {

    }
};

static FixedPoint16 operator + (FixedPoint16 e1, FixedPoint16 e2)
{
    FixedPoint16 result;
    result.all = e1.all + e2.all;

    return result;
}

static FixedPoint16 operator - (FixedPoint16 e1, FixedPoint16 e2)
{
    FixedPoint16 result;
    result.all = e1.all - e2.all;

    return result;
}

static FixedPoint16 operator * (FixedPoint16 e1, FixedPoint16 e2)
{
    std::int64_t all = (std::int64_t)e1.all * (std::int64_t)e2.all;

    FixedPoint16 result;
    result.all = all >> 16;

    return result;
}

static FixedPoint16 operator * (FixedPoint16 e1, std::int32_t i)
{
    FixedPoint16 result;
    result.all = e1.all * i;

    return result;
}

static FixedPoint16 operator / (FixedPoint16 e1, FixedPoint16 e2)
{
    std::int64_t all = ((std::int64_t)e1.all << 32) / (std::int64_t)e2.all;

    FixedPoint16 result;
    result.all = all >> 16;

    return result;
}

static FixedPoint16 operator / (FixedPoint16 e1, std::int32_t i)
{
    FixedPoint16 result;
    result.all = e1.all / i;

    return result;
}
