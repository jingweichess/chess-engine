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

#include <map>

#include "../types/color.h"
#include "../types/hash.h"
#include "../types/score.h"

template <class Board>
class Endgame
{
public:
    using BoardType = Board;
    using EndgameFunctionType = bool (*) (const BoardType& board, Score& score);
    using EndgameFunctionMapType = std::map<Hash, EndgameFunctionType>;

protected:
    EndgameFunctionMapType endgameFunctionMap;

    static constexpr bool nullEndgameFunction(const BoardType& board, Score& score)
    {
        return false;
    }
public:
    constexpr Endgame() = default;
    constexpr ~Endgame() = default;

    constexpr bool add(const BoardType& board, const EndgameFunctionType& endgameFunction)
    {
        const Hash materialHash = board.materialHashValue;
        Endgame::endgameFunctionMap[materialHash] = endgameFunction;

        return true;
    }
     
    constexpr bool probe(const BoardType& board, Score& score) const
    {
        const Hash materialHash = board.materialHashValue;

        if (Endgame::endgameFunctionMap.count(materialHash) == 0) {
            return false;
        }

        const EndgameFunctionType& endgameFunction = Endgame::endgameFunctionMap.at(materialHash);
        return endgameFunction(board, score);
    }
};
