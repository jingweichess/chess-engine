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

#include "../bitboards/passedpawn.h"

#include "../board/attackgenerator.h"
#include "../board/board.h"

#include "../endgame/endgame.h"

#include "../../game/eval/evaluator.h"

#include "../../game/math/byteswap.h"

#include "../hash/chesshashtable.h"

extern ChessEvaluation AttackParameters[PieceType::PIECETYPE_COUNT][PieceType::PIECETYPE_COUNT];
extern ChessEvaluation MobilityParameters[PieceType::PIECETYPE_COUNT][32];

class ChessEvaluator : public Evaluator<ChessEvaluator, ChessBoard, ChessEvaluation>
{
protected:
    ChessEndgame endgame;

    ChessAttackGenerator attackGenerator;

    Bitboard passedPawns[Color::COLOR_COUNT]{ EmptyBitboard, EmptyBitboard };

    constexpr void calculatePassedPawns(const BoardType& board, Bitboard& whitePassedPawns, Bitboard& blackPassedPawns) const
    {
        for (Color color = Color::COLOR_START; color < Color::COLOR_COUNT; color++) {
            const bool colorIsWhite = color == Color::WHITE;

            const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
            const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

            const Bitboard colorPawns = colorPieces[PieceType::PAWN];
            const Bitboard otherPawns = otherPieces[PieceType::PAWN];

            const Bitboard evaluatedOtherPawns = colorIsWhite ? otherPawns : SwapBytes(otherPawns);

            Bitboard calculatedPassedPawns = EmptyBitboard;

            for (const Square src : SquareBitboardIterator(colorPawns)) {
                const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);

                if ((PassedPawnCheck(evaluatedSrc) & evaluatedOtherPawns) == EmptyBitboard) {
                    calculatedPassedPawns |= src;
                }
            }

            if (color == Color::WHITE) {
                whitePassedPawns = calculatedPassedPawns;
            }
            else {
                blackPassedPawns = calculatedPassedPawns;
            }
        }
    }

    constexpr EvaluationType evaluateAttacks(const BoardType& board, Color color, PieceType srcPiece, Bitboard mobilityDstSquares) const
    {
        EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

        const bool colorIsWhite = color == Color::WHITE;

        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;
        const Bitboard attackSquares = mobilityDstSquares & otherPieces[PieceType::ALL];

        for (const Square dst : SquareBitboardIterator(attackSquares)) {
            const PieceType attackedPiece = board.pieces[dst];

            result += AttackParameters[srcPiece][attackedPiece];
        }

        return result;
    }

    constexpr EvaluationType evaluateMobility(Bitboard& outDstSquares, Bitboard allPieces, Bitboard piecesToMove, PieceType pieceType, Square src, Bitboard unsafeSquares) const
    {
        switch (pieceType) {
        case PieceType::KNIGHT:
            outDstSquares = PieceMoves[pieceType][src];
            break;
        case PieceType::BISHOP:
            outDstSquares = BishopMagic(src, allPieces);
            break;
        case PieceType::ROOK:
            outDstSquares = RookMagic(src, allPieces);
            break;
        case PieceType::QUEEN:
            outDstSquares = QueenMagic(src, allPieces);
            break;
        default:
            assert(0);

            return { ZERO_SCORE, ZERO_SCORE };
        }

        const std::uint32_t mobility = std::popcount(outDstSquares & ~piecesToMove & ~unsafeSquares);

        return MobilityParameters[pieceType][mobility];
    }

    EvaluationType evaluatePawnAttacks(const BoardType& board, const Bitboard* piecesToMove, const Bitboard* otherPieces, Color color) const;
    EvaluationType evaluateTropism(PieceType pieceType, Square src, Square otherKingPosition) const;

    EvaluationType evaluatePawnStructure(const BoardType& board, Bitboard& whitePassedPawns, Bitboard& blackPassedPawns) const;

    EvaluationType evaluateRook(const Bitboard* piecesToMove, Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src, bool hasPiecePair) const;
    EvaluationType evaluateQueen(Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src) const;
public:
	ChessEvaluator();
    ~ChessEvaluator() = default;

    constexpr bool checkBoardForInsufficientMaterial(const BoardType& board) const
    {
        const std::uint32_t pieceCount = board.getPieceCount();

        switch (pieceCount) {
        case 2:
            return true;
        case 3:
            if ((board.whitePieces[PieceType::KNIGHT] | board.whitePieces[PieceType::BISHOP] | board.blackPieces[PieceType::KNIGHT] | board.blackPieces[PieceType::BISHOP]) != EmptyBitboard) {
                return true;
            }
            break;
        case 4:
            if (std::popcount(board.whitePieces[PieceType::KNIGHT]) == 2) {
                return true;
            }

            if (std::popcount(board.blackPieces[PieceType::KNIGHT]) == 2) {
                return true;
            }

            if (PopCountIsOne(board.whitePieces[PieceType::BISHOP]) && PopCountIsOne(board.blackPieces[PieceType::BISHOP])) {
                const Square blackSquare = BitScanForward<Square>(board.blackPieces[PieceType::BISHOP]);
                const Square whiteSquare = BitScanForward<Square>(board.whitePieces[PieceType::BISHOP]);

                if (AreSquaresSameColor(blackSquare, whiteSquare)) {
                    return true;
                }
            }

            break;
        default:
            break;
        }

        return false;
    }

	Score evaluateImplementation(const BoardType& board, Depth currentDepth, Score alpha, Score beta);

	Score lazyEvaluateImplementation(const BoardType& board);

    void prefetch(Hash hashValue) const;
};
