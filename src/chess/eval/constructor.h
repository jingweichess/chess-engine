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

#include "../types/file.h"
#include "../types/rank.h"
#include "../types/score.h"
#include "../types/square.h"

struct QuadraticConstruct {
    ChessEvaluation quadratic;
    ChessEvaluation slope;
    ChessEvaluation yintercept;

    constexpr ChessEvaluation operator() (const std::int32_t x) const
    {
        return (x * x) * this->quadratic / 128 + x * this->slope / 16 + this->yintercept;
    }
};

struct PstConstruct {
    QuadraticConstruct rank;
    QuadraticConstruct filecenter;
    QuadraticConstruct rankcenter;
    QuadraticConstruct center;

    constexpr ChessEvaluation operator() (Square src) const
    {
        const Rank rank = GetRank(src);
        ChessEvaluation result = this->rank(7 - rank);

        const File file = GetFile(src);
        const std::int32_t f = 2 * file - 7;
        const Score fileDistance = f >= 0 ? f : -f;

        result -= this->filecenter(fileDistance * 2);

        const std::int32_t r = 2 * rank - 7;
        const Score rankDistance = r >= 0 ? r : -r;
        result -= this->rankcenter(rankDistance * 2);

        const Score centerDistance = rankDistance * rankDistance + fileDistance * fileDistance;
        result -= this->center(centerDistance / 8);

        return result;
    }
};

class ScoreConstructor
{
public:
    constexpr ScoreConstructor() = default;
    constexpr ~ScoreConstructor() = default;

    constexpr bool addPst(ChessEvaluation(&destinationPst)[Square::SQUARE_COUNT], const ChessEvaluation(&sourcePst)[Square::SQUARE_COUNT]) const
    {
        for (const Square src : SquareIterator()) {
            destinationPst[src] += sourcePst[src];
        }

        return true;
    }

    constexpr bool construct(const PstConstruct& pstConstruct, ChessEvaluation(&pst)[Square::SQUARE_COUNT], ChessEvaluation defaultEvaluation = { ZERO_SCORE, ZERO_SCORE }) const
    {
        for (const Square src : SquareIterator()) {
            pst[src] = defaultEvaluation + pstConstruct(src);
        }

        return true;
    }

    constexpr bool construct(QuadraticConstruct quadraticConstruct, ChessEvaluation* line, std::int32_t size) const
    {
        for (std::int32_t x = 0; x < size; x++) {
            line[x] = quadraticConstruct(x);
        }

        return true;
    }
};
