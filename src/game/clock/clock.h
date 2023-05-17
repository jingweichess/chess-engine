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

#include <chrono>

#include <ctime>

#include "../types/depth.h"
#include "../types/nodecount.h"

enum ClockType {
    NO_CLOCK,
    SEARCHTIME, SEARCHDEPTH, SEARCHNODES, SEARCHLEVEL
};

class Clock {
protected:
    using SteadyClock = std::chrono::steady_clock;

    ClockType clockType;

    Depth maxSearchDepth;
    NodeCount maxSearchNodes;
    std::time_t currentSearchTime, maxSearchTime;

    NodeCount nps;

    struct {
        NodeCount moves;
        std::time_t milliseconds, increment;
    } Level;

    NodeCount movesLeft;
    std::time_t engineTimeLeft, opponentTimeLeft;

    std::chrono::time_point<SteadyClock> startTime;

    bool minimumDepthReached;

    bool handleSearchLevel(NodeCount nodeCount);
public:
    constexpr Clock()
    {
        this->initializeClock();
    }

    constexpr ~Clock() = default;

    constexpr void initializeClock()
    {
        this->clockType = ClockType::NO_CLOCK;
        this->minimumDepthReached = false;
        this->nps = ZeroNodes;
    }

    std::time_t getElapsedTime(NodeCount nodeCount);
    std::time_t getTimeLeft(NodeCount nodeCount = ZeroNodes);

    bool shouldContinueSearch(Depth depth, NodeCount nodeCount);

    void decrementMovesLeft();

    void setClockDepth(Depth depth);
    void setClockEngineTimeLeft(std::time_t engineTimeLeft);
    void setClockLevel(NodeCount moveCount, std::time_t milliseconds, std::time_t increment);
    void setMovesLeft(std::uint32_t movesLeft);
    void setClockNodes(NodeCount nodeCount);
    void setClockNps(NodeCount nps);
    void setClockOpponentTimeLeft(std::time_t opponentTimeLeft);
    void setClockSearchTime(std::time_t milliseconds);

    void startClock();
};
