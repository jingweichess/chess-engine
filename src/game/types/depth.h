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

enum Depth {
    ZERO = 0,
    ONE = 1,
    TWO = ONE * 2,
    THREE = ONE * 3,
    FOUR = ONE * 4,
    FIVE = ONE * 5,
    SIX = ONE * 6,
    SEVEN = ONE * 7,
    EIGHT = ONE * 8,
    NINE = ONE * 9,
    TEN = ONE * 10,

    MAX = ONE * 128
};

constexpr Depth operator + (Depth d1, Depth d2)
{
    return Depth(int(d1) + int(d2));
}

constexpr Depth operator - (Depth d1, Depth d2)
{
    return Depth(int(d1) - int(d2));
}

constexpr Depth operator * (Depth d, int i)
{
    return Depth(int(d) * i);
}

constexpr Depth operator * (Depth d, float f)
{
    return Depth(float(d) * f);
}

constexpr Depth operator / (Depth d, int i)
{
    return Depth(int(d) / i);
}

constexpr Depth operator / (Depth d, float f)
{
    return Depth(float(d) / f);
}

constexpr Depth& operator ++ (Depth& d, int)
{
    return d = Depth(d + Depth::ONE);
}

constexpr Depth& operator += (Depth& d1, Depth d2)
{
    return d1 = Depth(int(d1) + int(d2));
}

constexpr Depth& operator -= (Depth& d1, Depth d2)
{
    return d1 = Depth(int(d1) - int(d2));
}
