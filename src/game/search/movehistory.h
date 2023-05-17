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

#include <vector>

#include "../types/hash.h"

template <class T, class Board, class MoveHistoryStruct>
class MoveHistory
{
protected:
    std::vector<MoveHistoryStruct> moveHistoryList;
public:
    using BoardType = Board;
    using MoveHistoryStructType = MoveHistoryStruct;
    using MoveType = typename BoardType::MoveType;

    using size_type = typename std::vector<MoveHistoryStruct>::size_type;

    MoveHistory() = default;
    ~MoveHistory() = default;

    void addMoveToHistory(BoardType& board, MoveType& move)
    {
        MoveHistoryStructType moveHistoryStruct;

        static_cast<T*>(this)->getNextMoveHistoryEntry(moveHistoryStruct, board, move);

        this->moveHistoryList.push_back(moveHistoryStruct);
    }

    std::uint32_t checkForDuplicateHash(Hash hashValue) const
    {
        std::uint32_t result = 0;

        for (auto it = this->moveHistoryList.rbegin(); it != this->moveHistoryList.rend(); ++it) {
            const MoveHistoryStructType& moveHistoryStruct = (*it);

            if (static_cast<const T*>(this)->beforeDuplicateHashCheckImplementation(hashValue, moveHistoryStruct)) {
                break;
            }

            if (moveHistoryStruct.hashValue == hashValue) {
                result++;
            }

            if (static_cast<const T*>(this)->afterDuplicateHashCheckImplementation(hashValue, moveHistoryStruct)) {
                break;
            }
        }

        return result;
    }

    void resetHistory()
    {
        this->moveHistoryList.clear();
    }

    void removeSingleMove()
    {
        this->moveHistoryList.pop_back();
    }

    void reserve(const size_type _Newcapacity)
    {
        this->moveHistoryList.reserve(_Newcapacity);
    }
};
