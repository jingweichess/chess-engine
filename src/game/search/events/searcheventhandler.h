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

#include <memory>
#include <type_traits>

#include "../../board/board.h"
#include "../../clock/clock.h"

#include "../pv.h"

#include "../../types/depth.h"
#include "../../types/nodecount.h"
#include "../../types/score.h"

template <class Board, class PrincipalVariation>
class SearchEventHandler
{
public:
    using BoardType = Board;
    using PrincipalVariationType = PrincipalVariation;

    //static_assert(std::is_same<BoardType::MoveType, PrincipalVariation::MoveType>);
public:
    constexpr SearchEventHandler() = default;
    constexpr ~SearchEventHandler() = default;

    void virtual onLineCompleted(const PrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth) = 0;
    void virtual onDepthCompleted(const PrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth) = 0;
    void virtual onSearchCompleted(const Board& board) = 0;
};

template <class Board, class PrincipalVariation>
using SearchEventHandlerSharedPtr = std::shared_ptr<SearchEventHandler<Board, PrincipalVariation>>;

template <class Board, class PrincipalVariation>
class SearchEventHandlerList
{
public:
    using BoardType = Board;
    using PrincipalVariationType = PrincipalVariation;

    using EventHandlerSharedPtr = SearchEventHandlerSharedPtr<BoardType, PrincipalVariationType>;

protected:
    std::vector<EventHandlerSharedPtr> searchEventHandlerList;

public:
    constexpr SearchEventHandlerList() = default;
    constexpr ~SearchEventHandlerList() = default;

    void onLineCompleted(const PrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        for (EventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onLineCompleted(principalVariation, time, nodeCount, score, depth);
        }
    }

    void onDepthCompleted(const PrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        for (EventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onDepthCompleted(principalVariation, time, nodeCount, score, depth);
        }
    }

    void onSearchCompleted(const Board& board)
    {
        for (EventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onSearchCompleted(board);
        }
    }

    constexpr void push_back(EventHandlerSharedPtr& _Val)
    {
        this->searchEventHandlerList.push_back(_Val);
    }
};
