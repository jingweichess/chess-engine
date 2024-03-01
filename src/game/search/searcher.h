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

#include <algorithm>

#include <cstdio>
#include <cstdint>

#include "pv.h"

#include "../clock/clock.h"

#include "../eval/evaluator.h"

#include "../types/depth.h"
#include "../types/nodecount.h"
#include "../types/score.h"

static constexpr bool enableAllSearchFeatures = true;

static constexpr bool enableAspirationWindow = enableAllSearchFeatures && true;

extern Evaluation AspirationWindowInitialValue;
extern Evaluation AspirationWindowDelta;

template <class T, class Evaluator, class MoveGenerator, class MoveHistory, class PrincipalVariation>
class Searcher
{
protected:
    Clock clock;
    Evaluator evaluator;
    MoveGenerator moveGenerator;
    MoveHistory moveHistory;

    NodeCount nodeCount;
    bool abortedSearch;

public:
    using BoardType = typename Evaluator::BoardType;
    using EvaluationType = typename Evaluator::EvaluationType;
    using MoveGeneratorType = MoveGenerator;
    using MoveType = typename BoardType::MoveType;
    using PrincipalVariationType = PrincipalVariation;

    Searcher()
    {
        this->abortedSearch = false;
        this->nodeCount = ZeroNodes;
    }
    ~Searcher() = default;

    void addMoveToHistory(BoardType& board, MoveType& move)
    {
        this->moveHistory.addMoveToHistory(board, move);
    }

    void initializeSearch(BoardType& board)
    {
        this->abortedSearch = false;
        this->nodeCount = ZeroNodes;

        this->clock.startClock();

        static_cast<T*>(this)->initializeSearchImplementation(board);
    }

    Score iterativeDeepeningLoop(BoardType& board, PrincipalVariationType& principalVariation)
    {
        const std::int32_t phase = board.getPhase();
        
        const Score initialDelta = AspirationWindowInitialValue(phase);
        const Score deltaExpansion = AspirationWindowDelta(phase);

        this->initializeSearch(board);

        Score aspirationWindowDelta = initialDelta;

        Score previousScore = ZERO_SCORE;
        Score alpha = -WIN_SCORE, beta = WIN_SCORE;
        Score bestScore = -WIN_SCORE;
         
        Depth searchDepth = Depth::TWO;

        bool isSearching = true;
        bool foundMateSolution = false;

        while (isSearching) {
            if (enableAspirationWindow
                && !foundMateSolution
                && (searchDepth >= Depth::THREE)) {
                aspirationWindowDelta = initialDelta;

                alpha = std::max(previousScore - aspirationWindowDelta, -WIN_SCORE);
                beta = std::min(previousScore + aspirationWindowDelta, WIN_SCORE);
            }
            else if (foundMateSolution) {
                if (previousScore > WIN_SCORE - Depth::MAX) {
                    alpha = WIN_SCORE - Depth::MAX;
                    beta = WIN_SCORE;
                }
                else {
                    alpha = -WIN_SCORE;
                    beta = -WIN_SCORE + Depth::MAX;
                }
            }

            Score score = this->rootSearch(board, principalVariation, searchDepth, , beta);
            foundMateSolution = IsMateScore(score);

            while (enableAspirationWindow
//                && !foundMateSolution
                && !this->wasSearchAborted()
                && (score <= alpha || score >= beta)) {

                if (foundMateSolution) {
                    if (score > WIN_SCORE - Depth::MAX) {
                        alpha = WIN_SCORE - Depth::MAX;
                        beta = WIN_SCORE;
                    }
                    else {
                        alpha = -WIN_SCORE;
                        beta = -WIN_SCORE + Depth::MAX;
                    }
                }
                else if (score <= alpha) {
                    alpha = std::max(score - aspirationWindowDelta, -WIN_SCORE);
                }
                else if (score >= beta) {
                    beta = std::min(score + aspirationWindowDelta, WIN_SCORE);
                }
                else {
                    break;
                }

                score = this->rootSearch(board, principalVariation, searchDepth, alpha, beta);
                foundMateSolution = IsMateScore(score);

                aspirationWindowDelta += deltaExpansion;
            }

            if (this->wasSearchAborted()) {
                break;
            }

            if (foundMateSolution) {
                Depth distanceToMate = DistanceToMate(score);
                if (searchDepth > distanceToMate * 3) {
                    isSearching = false;
                }
            }

            isSearching = isSearching && this->clock.shouldContinueSearch(searchDepth, this->getNodeCount());

            previousScore = score;
            searchDepth++;
        }

        return bestScore;
    }

    NodeCount getNodeCount()
    {
        return this->nodeCount;
    }

    void removeLastMoveFromHistory()
    {
        this->moveHistory.removeSingleMove();
    }

    void resetMoveHistory()
    {
        this->moveHistory.resetHistory();
    }

    Score rootSearch(BoardType& board, PrincipalVariationType& principalVariation, Depth maxDepth, Score alpha, Score beta)
    {
        return static_cast<T*>(this)->rootSearchImplementation(board, principalVariation, maxDepth, alpha, beta);
    }

    void setClock(Clock& clock)
    {
        this->clock = clock;
    }

    void setEvaluator(Evaluator& evaluator)
    {
        this->evaluator = evaluator;
    }

    bool wasSearchAborted()
    {
        return this->abortedSearch;
    }
};
