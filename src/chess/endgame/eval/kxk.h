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

#include "../function.h"

constexpr ChessEndgame::EndgameFunctionType kpk = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType knk = drawEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbk = drawEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqk = weakKingEndgameFunction;

//extern const KqkTablebase kqkTablebase;

//constexpr bool kqk(const ChessBoard& board, Score& score)
//{
//    TablebaseResult tablebaseResult = kqkTablebase.probe(board);
//
//    if (tablebaseResult == TablebaseResult::DEFAULT
//        || tablebaseResult == TablebaseResult::DTC_ZERO) {
//        return DRAW_SCORE;
//    }
//
//    //1) Determine strong side
//    const Color strongSide = FindStrongSide(board);
//
//    const Depth distanceToMate = static_cast<Depth>(tablebaseResult - TablebaseResult::DTM_ZERO);
//    score = WinInDepth(distanceToMate);
//
//    //6) Ensure score is returned for side to move
//    if (board.sideToMove != strongSide) {
//        score = -score;
//    }
//
//    return true;
//}
