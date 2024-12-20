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

#include "../../game/types/score.h"
#include "../../game/types/nodecount.h"

#include "../eval/constructor.h"

//extern QuadraticConstruct LateMoveReductionsDepthLeft;
extern QuadraticConstruct LateMoveReductionsSearchedMoves;

extern QuadraticConstruct PruningMarginDepthLeft;
extern QuadraticConstruct PruningMarginSearchedMoves;

constexpr Depth LateMoveReductions(Depth depthLeft, NodeCount searchedMoves, std::uint32_t phase)
{
    const Score result = LateMoveReductionsSearchedMoves(static_cast<std::int32_t>(searchedMoves * 2))(phase);

    if (result < ZERO_SCORE) {
        return Depth::ZERO;
    }

    if (result > 1024) {
        return Depth::FOUR;
    }
    else {
        return static_cast<Depth>(result / PAWN_SCORE);
    }
}

constexpr Score PruningMargin(Depth depthLeft, NodeCount searchedMoves, std::uint32_t phase)
{
    const Score result = PruningMarginDepthLeft(depthLeft * 8)(phase) + PruningMarginSearchedMoves(static_cast<std::int32_t>(searchedMoves * 2))(phase);

    return result;
}

constexpr Score StaticExchangeEvaluationReductionThreshold(Depth depthLeft, NodeCount searchedMoves, std::uint32_t phase)
{
    return -PAWN_SCORE;
}
