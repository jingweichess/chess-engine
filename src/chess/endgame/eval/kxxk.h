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

#include <array>

#include "../function.h"

#include "../../../chess/types/square.h"

#include "../../../game/math/bitreset.h"
#include "../../../game/math/bitscan.h"

#include "../../../game/types/score.h"

constexpr std::array<Score, Square::SQUARE_COUNT> DarkSquaredBishopMate =
{
	350, 400, 450, 500, 550, 600, 650, 700,
	400, 350, 400, 450, 500, 550, 600, 650,
	450, 400, 350, 400, 450, 500, 550, 600,
	500, 450, 400, 350, 400, 450, 500, 550,
	550, 500, 450, 400, 350, 400, 450, 500,
	600, 550, 500, 450, 400, 350, 400, 450,
	650, 600, 550, 500, 450, 400, 350, 400,
	700, 650, 600, 550, 500, 450, 400, 350
};

constexpr std::array<Score, Square::SQUARE_COUNT> LightSquaredBishopMate =
{
	700, 650, 600, 550, 500, 450, 400, 350,
	650, 600, 550, 500, 450, 400, 350, 400,
	600, 550, 500, 450, 400, 350, 400, 450,
	550, 500, 450, 400, 350, 400, 450, 500,
	500, 450, 400, 350, 400, 450, 500, 550,
	450, 400, 350, 400, 450, 500, 550, 600,
	400, 350, 400, 450, 500, 550, 600, 650,
	350, 400, 450, 500, 550, 600, 650, 700
};

constexpr bool kppk(const ChessBoard& board, Score& score)
{
    constexpr Bitboard RookFileBitboards = FileBitboard[File::_A] | FileBitboard[File::_H];

    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) If there is a pawn that isn't on a rook file, this is a win
    const Bitboard strongPawns = strongSideIsWhite ? board.whitePieces[PieceType::PAWN] : board.blackPieces[PieceType::PAWN];

    if ((strongPawns & RookFileBitboards) == strongPawns) {
        const bool result = drawEndgameFunction(board, score);

        score += board.pstEvaluation(board.getPhase());

        return result;
    }

    return weakKingEndgameFunction(board, score);
}

constexpr ChessEndgame::EndgameFunctionType kpppk = kppk;

constexpr ChessEndgame::EndgameFunctionType knpk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType knnk = drawEndgameFunction;

constexpr bool kbpk(const ChessBoard& board, Score& score)
{
    //1) Determine strong side
    const Color strongSide = FindStrongSide(board);
    const bool strongSideIsWhite = strongSide == Color::WHITE;

    //2) If Pawn is not on outside File, return normal score
    const Bitboard strongPawns = strongSideIsWhite ? board.whitePieces[PieceType::PAWN] : board.blackPieces[PieceType::PAWN];
    const Square pawnSrc = BitScanForward<Square>(strongPawns);
    const File pawnFile = GetFile(pawnSrc);

    //3) If there's more than one pawn and they're on different files, this is a win.
    if (std::popcount(strongPawns) > 1) {
        const Bitboard fileBitboard = FileBitboard[pawnFile];

        if ((strongPawns & fileBitboard) != strongPawns) {
            //No special case; return normal score
            return weakKingEndgameFunction(board, score);
        }
    }

    //4) Account for light or dark squared bishop
    const Bitboard strongBishops = strongSideIsWhite ? board.whitePieces[PieceType::BISHOP] : board.blackPieces[PieceType::BISHOP];
    const Square bishopSrc = BitScanForward<Square>(strongBishops);

    const bool isLightColoredBishop = IsLightSquare(bishopSrc);

    //5) If Pawn is on outside file, and the bishop cannot cover it, return draw.
    if (pawnFile == File::_A
        && (strongSideIsWhite ? !isLightColoredBishop : isLightColoredBishop)) {
        score = DRAW_SCORE;
        return true;
    }
    else if (pawnFile == File::_H
        && (strongSideIsWhite ? isLightColoredBishop : !isLightColoredBishop)) {
        score = DRAW_SCORE;
        return true;
    }

    //6) No special case; return normal score
    return weakKingEndgameFunction(board, score);
}

constexpr ChessEndgame::EndgameFunctionType kbppk = kbpk;

constexpr bool kbnk(const ChessBoard& board, Score& score)
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

	//4) Account for light or dark squared bishop
	const Bitboard strongBishops = strongSideIsWhite ? board.whitePieces[PieceType::BISHOP] : board.blackPieces[PieceType::BISHOP];

	const Square src = BitScanForward<Square>(strongBishops);

	const bool isLightSquaredBishop = IsLightSquare(src);
	const std::array<Score, Square::SQUARE_COUNT>& BishopMate = isLightSquaredBishop ? LightSquaredBishopMate : DarkSquaredBishopMate;

	//5) Put it all together for the strong side
	const std::int32_t kingDistance = constexpr_sqrt(file * file + rank * rank);
	score = BASICALLY_WINNING_SCORE + BishopMate[weakKingPosition] + KingProximity[kingDistance];

	//6) Ensure score is returned for side to move
	if (board.sideToMove != strongSide) {
		score = -score;
	}

	return true;
}

constexpr bool kbbk(const ChessBoard& board, Score& score)
{
	//1) Determine strong side
	const Color strongSide = FindStrongSide(board);
	const bool strongSideIsWhite = strongSide == Color::WHITE;

	//2) Ensure there is a bishop on a light square and the second is on a dark square
	Bitboard strongBishops = strongSideIsWhite ? board.whitePieces[PieceType::BISHOP] : board.blackPieces[PieceType::BISHOP];

	const Square src1 = BitScanForward<Square>(strongBishops);

	strongBishops = ResetLowestSetBit(strongBishops);
	const Square src2 = BitScanForward<Square>(strongBishops);

	const bool isLightSquaredBishop1 = IsLightSquare(src1);

	if (const bool isLightSquaredBishop2 = IsLightSquare(src2); 
		isLightSquaredBishop1 == isLightSquaredBishop2) {
		return drawEndgameFunction(board, score);
	}

	//3) If not a draw, defer to normal weakKingEndgameFunction for mate
	return weakKingEndgameFunction(board, score);
}

constexpr ChessEndgame::EndgameFunctionType krpk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krnk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krbk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krrk = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqpk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqnk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqbk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqrk = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqqk = weakKingEndgameFunction;
