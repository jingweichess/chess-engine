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

#include "../board/board.h"

#include "../types/score.h"

template <class T, class Board, class Evaluation>
class Evaluator
{
public:
    using BoardType = Board;
    using EvaluationType = Evaluation;

    Evaluator() = default;
    ~Evaluator() = default;

    Score evaluate(const BoardType& board, Depth currentDepth, Score alpha, Score beta)
    {
        return static_cast<T *>(this)->evaluateImplementation(board, currentDepth, alpha, beta);
    }

    Score lazyEvaluate(const BoardType& board)
    {
        return static_cast<T *>(this)->lazyEvaluateImplementation(board);
    }
};
