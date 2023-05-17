#pragma once

#include "../../game/types/score.h"
#include "../../game/types/nodecount.h"

#include "../eval/constructor.h"

extern QuadraticConstruct LateMoveReductionsDepthLeft;
extern QuadraticConstruct LateMoveReductionsSearchedMoves;

extern QuadraticConstruct PruningMarginDepthLeft;
extern QuadraticConstruct PruningMarginSearchedMoves;

constexpr Depth LateMoveReductions(Depth depthLeft, NodeCount searchedMoves, std::uint32_t phase)
{
    const Score result = LateMoveReductionsDepthLeft(depthLeft * 8)(phase) + LateMoveReductionsSearchedMoves(static_cast<std::int32_t>(searchedMoves * 2))(phase);

    if (result < Depth::ZERO) {
        return Depth::ZERO;
    }

    return static_cast<Depth>(result / 256);
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
