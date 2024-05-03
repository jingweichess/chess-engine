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

enum Rank {
    _8, _7, _6, _5, _4, _3, _2, _1, RANK_COUNT, NO_RANK, FIRST_RANK = _8, LAST_RANK = _1
};

constexpr Rank& operator ++ (Rank& s, int)
{
    return s = Rank(int(s) + 1);
}

constexpr Rank& operator -- (Rank& s, int)
{
    return s = Rank(int(s) - 1);
}

constexpr Rank operator + (Rank r1, Rank r2)
{
    return Rank(int(r1) + int(r2));
}

constexpr Rank operator - (Rank r1, Rank r2)
{
    return Rank(int(r1) - int(r2));
}

constexpr bool operator > (Rank r1, Rank r2)
{
    return int(r1) < int(r2);
}

constexpr bool operator < (Rank r1, Rank r2)
{
    return int(r1) > int(r2);
}

constexpr Rank operator ~ (Rank r)
{
    return Rank::RANK_COUNT - r;
}

class RankIterator {
private:
    Rank currentState;

public:
    explicit constexpr RankIterator(Rank beginState = Rank::FIRST_RANK) : currentState(beginState) {}

    constexpr bool operator==(const RankIterator& other) const
    {
        return this->currentState == other.currentState;
    }

    constexpr bool operator!=(const RankIterator& other) const
    {
        return this->currentState != other.currentState;
    }

    constexpr Rank operator*() const
    {
        return this->currentState;
    }

    constexpr RankIterator& operator++()
    {
        this->currentState++;

        return *this;
    }

    constexpr RankIterator begin() const
    {
        return RankIterator{ Rank::FIRST_RANK };
    }

    constexpr RankIterator end() const
    {
        return RankIterator{ Rank::RANK_COUNT };
    }
};
