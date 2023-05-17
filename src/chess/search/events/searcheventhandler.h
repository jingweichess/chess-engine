#pragma once

#include <memory>

#include "../chesspv.h"

#include "../../board/board.h"

#include "../../../game/types/depth.h"
#include "../../../game/types/nodecount.h"
#include "../../../game/types/score.h"

class SearchEventHandler
{
public:
    constexpr SearchEventHandler() = default;
    constexpr ~SearchEventHandler() = default;

    void virtual onLineCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth) = 0;
    void virtual onDepthCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth) = 0;
    void virtual onSearchCompleted(const ChessBoard& board) = 0;
};

using SearchEventHandlerSharedPtr = std::shared_ptr<SearchEventHandler>;

class SearchEventHandlerList
{
protected:
    std::vector<SearchEventHandlerSharedPtr> searchEventHandlerList;
public:
    constexpr SearchEventHandlerList() = default;
    constexpr ~SearchEventHandlerList() = default;

    void onLineCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        for (SearchEventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onLineCompleted(principalVariation, time, nodeCount, score, depth);
        }
    }

    void onDepthCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        for (SearchEventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onDepthCompleted(principalVariation, time, nodeCount, score, depth);
        }
    }

    void onSearchCompleted(const ChessBoard& board)
    {
        for (SearchEventHandlerSharedPtr searchEventHandler : this->searchEventHandlerList) {
            searchEventHandler->onSearchCompleted(board);
        }
    }

    constexpr void push_back(SearchEventHandlerSharedPtr& _Val)
    {
        this->searchEventHandlerList.push_back(_Val);
    }
};
