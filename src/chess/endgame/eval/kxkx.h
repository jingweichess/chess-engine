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

//Keep in mind here, even though the "strong side" has more value in material, the strong side cannot win and must prevent the pawn from advancing
constexpr bool knkp(const ChessBoard& board, Score& score)
{
    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) Get pst values for KN and KP
    const Score pst = board.pstEvaluation.eg;

    //psts are relative to white.  If strong side is Black, we have to negate.
    score = strongSideIsWhite ? pst : -pst;

    //3) If we're returning the strong side can win, bias toward "weak" side
    if (score > DRAW_SCORE) {
        score = DRAW_SCORE - 1;
    }

    //6) Ensure score is returned for side to move
    if (board.sideToMove != strongSide) {
        score = -score;
    }

    return true;
}

constexpr ChessEndgame::EndgameFunctionType knkn = drawEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kbkp = knkp;
constexpr ChessEndgame::EndgameFunctionType kbkn = drawEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbkb = drawEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krkn = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krkb = drawEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krkr = drawEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkq = drawEndgameFunction;
