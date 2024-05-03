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

#include <vector>

#include "../types/hash.h"

template <class Board, class Move>
class MoveHistory
{
public: 
    using BoardType = Board;
    using MoveType = typename BoardType::MoveType;

protected:
    struct MoveHistoryStruct {
        Hash hashValue;
        MoveType move;
        bool irreversible;
    };

public:
    using MoveHistoryStructType = MoveHistoryStruct;

protected:
    std::vector<MoveHistoryStructType> moveHistoryList;
    using size_type = typename std::vector<MoveHistoryStructType>::size_type;

public:

    constexpr MoveHistory() = default;
    constexpr ~MoveHistory() = default;

    constexpr std::uint32_t checkForDuplicateHash(Hash hashValue) const
    {
        std::uint32_t result = 0;

        for (auto it = this->moveHistoryList.rbegin(); it != this->moveHistoryList.rend(); ++it) {
            const MoveHistoryStructType& moveHistoryStruct = (*it);

            if (moveHistoryStruct.hashValue == hashValue) {
                result++;
            }

            if (moveHistoryStruct.irreversible) {
                break;
            }
        }

        return result;
    }

    constexpr void clear()
    {
        this->moveHistoryList.clear();
    }

    constexpr void pop_back()
    {
        this->moveHistoryList.pop_back();
    }

    constexpr void push_back(const BoardType& board, const MoveType& move, bool irreversible = false)
    {
        MoveHistoryStructType moveHistoryStruct{
            board.hashValue,
            move,
            irreversible
        };

        this->moveHistoryList.push_back(moveHistoryStruct);
    }

    constexpr void reserve(const size_type _Newcapacity)
    {
        this->moveHistoryList.reserve(_Newcapacity);
    }
};
