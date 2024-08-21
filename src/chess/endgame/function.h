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

#include <array>
#include <cassert>
#include <cmath>

#include "endgame.h"

#include "../../game/math/bitscan.h"

#include "../board/board.h"

#include "../types/score.h"
#include "../types/square.h"

constexpr std::uint32_t constexpr_sqrt(std::uint32_t n, std::uint32_t i = 1) {
    return n == i ? n : (i * i < n ? constexpr_sqrt(n, i + 1) : i);
}

constexpr std::array<Score, Square::SQUARE_COUNT> GeneralMate =
{
    800, 700, 600, 500, 500, 600, 700, 800,
    700, 600, 500, 400, 400, 500, 600, 700,
    600, 500, 400, 300, 300, 400, 500, 600,
    500, 400, 300, 200, 200, 300, 400, 500,
    500, 400, 300, 200, 200, 300, 400, 500,
    600, 500, 400, 300, 300, 400, 500, 600,
    700, 600, 500, 400, 400, 500, 600, 700,
    800, 700, 600, 500, 500, 600, 700, 800
};

constexpr std::array<Score, 11> KingProximity = { 0, 0, 90, 80, 70, 60, 50, 40, 30, 20, 10 };

extern ChessEvaluation PstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

constexpr bool drawEndgameFunction(const ChessBoard& board, Score& score)
{
    score = DRAW_SCORE;

    return true;
}

constexpr Color FindStrongSide(const ChessBoard& board)
{
    //There's no need to calculate the actual material value based on the phase calculated score
    const Score materialScore = board.materialEvaluation.eg;

    //Default "strong" side is WHITE if material is even
    return materialScore >= DRAW_SCORE ? Color::WHITE : Color::BLACK;
}

constexpr bool nullEndgameFunction(const ChessBoard& board, const Score& score)
{
    return false;
}

template <Score baseScore = BASICALLY_WINNING_SCORE>
constexpr bool pushPawnEndgameFunction(const ChessBoard& board, Score& score)
{
    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) Account for other pieces being placed optimally
    const Bitboard pawns = strongSideIsWhite ? board.whitePieces[PieceType::PAWN] : board.blackPieces[PieceType::PAWN];
    Square src = BitScanForward<Square>(pawns);

    if (strongSide == Color::BLACK) {
        src = FlipSquareOnHorizontalLine(src);
    }

    //psts are relative to white.  If strong side is Black, we have to negate.
    const Score pst = strongSideIsWhite ? PstParameters[PieceType::PAWN][src].eg : -PstParameters[PieceType::PAWN][src].eg;

    //3) Put it all together for the strong side
    score = baseScore + pst;

    //4) Ensure score is returned for side to move
    if (board.sideToMove != strongSide) {
        score = -score;
    }

    return true;
}

template <Score baseScore = BASICALLY_WINNING_SCORE>
constexpr bool weakKingEndgameFunction(const ChessBoard& board, Score& score)
{
    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) Return value based on weak king's position
    const Square weakKingPosition = strongSideIsWhite ? board.blackKingPosition() : board.whiteKingPosition();

    //3) Calculate king proximity.  The strong king being close to the weak king makes
    //  it easier to force it around
    const File file = GetFile(board.whiteKingPosition()) - GetFile(board.blackKingPosition());
    const Rank rank = GetRank(board.whiteKingPosition()) - GetRank(board.blackKingPosition());

    //4) Account for other pieces being placed optimally
    //psts are relative to white.  If strong side is Black, we have to negate.
    const Score pst = strongSideIsWhite ? board.pstEvaluation.eg : -board.pstEvaluation.eg;

    //5) Put it all together for the strong side
    const std::int32_t kingDistance = constexpr_sqrt(file * file + rank * rank);
    score = baseScore + GeneralMate[weakKingPosition] + KingProximity[kingDistance] + pst;

    //6) Ensure score is returned for side to move
    if (board.sideToMove != strongSide) {
        score = -score;
    }

    return true;
}

constexpr bool weakKingDrawishEndgameFunction(const ChessBoard& board, Score& score)
{
    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) Calculate king proximity.  The strong king being close to the weak king makes
    //  it easier to force it around
    const File file = GetFile(board.whiteKingPosition()) - GetFile(board.blackKingPosition());
    const Rank rank = GetRank(board.whiteKingPosition()) - GetRank(board.blackKingPosition());

    //3) Account for other pieces being placed optimally
    const Score pst = strongSideIsWhite ? board.pstEvaluation.eg : -board.pstEvaluation.eg;

    //4) Put it all together for the strong side
    const std::int32_t kingDistance = constexpr_sqrt(file * file + rank * rank);
    score = DRAW_SCORE + KingProximity[kingDistance] + pst;

    //5) Ensure score is returned for side to move
    if (board.sideToMove != strongSide) {
        score = -score;
    }

    return true;
}
