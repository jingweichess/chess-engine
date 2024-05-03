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

#include <algorithm>

#include "clock.h"

void Clock::decrementMovesLeft()
{
    this->movesLeft--;
}

std::time_t Clock::getElapsedTime(NodeCount nodeCount)
{
    //If we've receive an nps command, time elapsed is only based on the node count.
    if (this->nps != ZeroNodes) {
        //Multiply by 1000 first to improve precision.
        return static_cast<int>(1000.0f * nodeCount / this->nps);
    }

    std::chrono::time_point<SteadyClock> currentTime = SteadyClock::now();
    std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - this->startTime);

    return std::time_t(elapsedTime.count());
}

std::time_t Clock::getTimeLeft(NodeCount nodeCount)
{
    return this->engineTimeLeft;
}

bool Clock::handleSearchLevel(NodeCount nodeCount)
{
    std::time_t millisecondsPerMove;

    if (this->Level.moves == 0) {
        millisecondsPerMove = this->engineTimeLeft / 30 + this->Level.increment;
    }
    else if (this->movesLeft == 1) {
        millisecondsPerMove = this->engineTimeLeft;
    }
    else {
        millisecondsPerMove = this->engineTimeLeft / this->movesLeft + this->Level.increment;
    }

    return this->getElapsedTime(nodeCount) < std::max(static_cast<std::int32_t>(millisecondsPerMove) - 20, 1);
}

void Clock::setClockDepth(Depth depth)
{
    this->clockType = ClockType::SEARCHDEPTH;

    this->maxSearchDepth = depth;
}

void Clock::setClockEngineTimeLeft(std::time_t engineTimeLeft)
{
    this->engineTimeLeft = engineTimeLeft;
}

void Clock::setClockLevel(NodeCount moveCount, std::time_t milliseconds, std::time_t increment)
{
    this->Level.moves = moveCount;
    this->Level.milliseconds = milliseconds;
    this->Level.increment = increment;

    this->movesLeft = moveCount;
    this->engineTimeLeft = milliseconds;

    this->clockType = ClockType::SEARCHLEVEL;
}

void Clock::setClockNodes(NodeCount nodeCount)
{
    this->clockType = ClockType::SEARCHNODES;

    this->maxSearchNodes = nodeCount;
}

void Clock::setClockNps(NodeCount nps)
{
    this->nps = nps;
}

void Clock::setClockOpponentTimeLeft(std::time_t opponentTimeLeft)
{
    this->opponentTimeLeft = opponentTimeLeft;
}

void Clock::setClockSearchTime(std::time_t milliseconds)
{
    this->clockType = ClockType::SEARCHTIME;

    this->maxSearchTime = milliseconds;
}

void Clock::setMovesLeft(std::uint32_t movesLeft)
{
    this->movesLeft = movesLeft;
}

bool Clock::shouldContinueSearch(Depth depth, NodeCount nodeCount)
{
    if (depth >= Depth::MAX) {
        return false;
    }

    if (this->clockType != ClockType::SEARCHDEPTH
        && !this->minimumDepthReached) {
        if (depth > Depth::ONE) {
            this->minimumDepthReached = true;
        }
        else {
            return true;
        }
    }

    if (this->nps != ZeroNodes) {
        //If we've received an nps command, we are going by faked search time
        const NodeCount maxNodes = this->nps * this->maxSearchTime / 1000;
        return nodeCount < maxNodes;
    }

    switch (this->clockType) {
    case ClockType::NO_CLOCK:
        return false;
    case ClockType::SEARCHTIME:
        if (this->getElapsedTime(nodeCount) >= this->maxSearchTime) {
            return false;
        }
        break;
    case ClockType::SEARCHDEPTH:
        if (depth >= this->maxSearchDepth) {
            return false;
        }
        break;
    case ClockType::SEARCHNODES:
        if (nodeCount >= this->maxSearchNodes) {
            return false;
        }
        break;
    case ClockType::SEARCHLEVEL:
        if (nodeCount % 1024 != 0) {
            return true;
        }

        return this->handleSearchLevel(nodeCount);
    default:
        break;
    }

    return true;
}

void Clock::startClock()
{
    this->minimumDepthReached = false;
    this->startTime = SteadyClock::now();
}
