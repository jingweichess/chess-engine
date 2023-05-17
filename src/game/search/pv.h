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

#include <iostream>
#include <string>

#include "../clock/clock.h"

#include "../types/depth.h"
#include "../types/movelist.h"
#include "../types/nodecount.h"
#include "../types/score.h"

static const std::string PiecePrint = ".pnbrqk";

template <class T, class Move>
class PrincipalVariation
{
public:
    using MoveType = Move;
protected:
    MoveList<MoveType> moveList;
public:
    using const_iterator = typename MoveList<MoveType>::const_iterator;
    using iterator = typename MoveList<MoveType>::iterator;
    using size_type = typename MoveList<MoveType>::size_type;

    constexpr PrincipalVariation() = default;
    constexpr ~PrincipalVariation() = default;

    constexpr iterator begin()
    {
        return this->moveList.begin();
    }

    constexpr const_iterator cbegin() const
    {
        return this->moveList.cbegin();
    }

    void clear()
    {
        this->moveList.clear();
    }

    void copyBackward(const PrincipalVariation<T, MoveType>& principalVariation, MoveType& move)
    {
#ifdef _DEBUG
        Score moveOrdinal = move.ordinal;

        if (!IsMateScore(moveOrdinal)) {
            for (const MoveType& pvMove : principalVariation.moveList) {
                moveOrdinal = -moveOrdinal;
                assert(pvMove.ordinal == moveOrdinal);
            }
        }
#endif

        this->moveList = principalVariation.moveList;
        this->moveList.insert(this->moveList.begin(), move);
    }

    void copyForward(PrincipalVariation<T, MoveType>& nextPrincipalVariation)
    {
        if (this->moveList.size() == 0) {
            return;
        }

        MoveList<Move>& nextMoveList = nextPrincipalVariation.moveList;
        nextMoveList = this->moveList;

        nextPrincipalVariation.erase(nextPrincipalVariation.begin());
    }

    constexpr iterator end()
    {
        return this->moveList.end();
    }

    constexpr const_iterator cend() const
    {
        return this->moveList.cend();
    }

    constexpr iterator erase(const iterator _Where)
    {
        return this->moveList.erase(_Where);
    }

    void print() const
    {
        for (PrincipalVariation<T, Move>::const_iterator it = this->moveList.begin(); it != this->moveList.end(); ++it) {
            if (it != this->moveList.begin()) {
                std::cout << " ";
            }

            this->printMoveToConsole(*it);
        }
    }

    void printMoveToConsole(const MoveType& move) const
    {
        static_cast<const T*>(this)->printMoveToConsoleImplementation(move);
    }

    constexpr size_type size() const
    {
        return this->moveList.size();
    }

    void stringToMove(std::string& moveString, MoveType& move)
    {
        static_cast<T*>(this)->stringToMoveImplementation(moveString, move);
    }

    MoveType& operator[](const size_type _Pos)
    {
        return this->moveList[_Pos];
    }

    const MoveType& operator[](const size_type _Pos) const
    {
        return this->moveList[_Pos];
    }
};
