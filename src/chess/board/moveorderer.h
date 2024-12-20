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

#include "attackgenerator.h"

#include "../types/nodetype.h"

#include "../search/history.h"

#include "../types/searchstack.h"

#include "../types/move.h"

#include "board.h"
#include "see.h"

class ChessMoveOrderer
{
protected:
    const ChessAttackGenerator attackGenerator;
    const ChessStaticExchangeEvaluator staticExchangeEvaluator;
public:
    constexpr ChessMoveOrderer() = default;
    constexpr ~ChessMoveOrderer() = default;

    void reorderMoves(const ChessBoard& board, ChessMoveList& moveList, const ChessSearchStack* searchStack, const PieceTypeSquareHistoryTable& historyTable, const SquareSquareHistoryTable(&mateHistoryTable)[2]) const
    {
        const bool isWhiteToMove = board.isWhiteToMove();

        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        const Bitboard unsafeSquares = this->attackGenerator.unsafeSquares(board.sideToMove, otherPieces);

        for (ChessMove& move : moveList) {
            const Square& src = move.src;
            const Square& dst = move.dst;

            const Bitboard dstBitboard = OneShiftedBy(dst);

            const PieceType& movingPiece = board.pieces[src];
            const PieceType& capturedPiece = board.pieces[dst];
            const PieceType& promotionPiece = move.promotionPiece;

            move.seeScore = INVALID_SCORE;

            if (searchStack->pvMove == move
                || searchStack->hashMove == move) {
                move.ordinal = ChessMoveOrdinal::PV_MOVE;
            }
            else if (movingPiece != PieceType::PAWN
                && (unsafeSquares & dstBitboard) != EmptyBitboard) {
                move.ordinal = ChessMoveOrdinal::UNSAFE_MOVE;
            }
            else if (capturedPiece != PieceType::NO_PIECE) {
                move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

                if (move.seeScore > ZERO_SCORE) {
                    move.ordinal = ChessMoveOrdinal::GOOD_CAPTURE_MOVE;
                }
                else if (move.seeScore == ZERO_SCORE) {
                    move.ordinal = ChessMoveOrdinal::EQUAL_CAPTURE_MOVE;
                }
                else {
                    move.ordinal = ChessMoveOrdinal::BAD_CAPTURE_MOVE;
                }
            }
            else if (promotionPiece != PieceType::NO_PIECE) {
                move.ordinal = promotionPiece == PieceType::QUEEN ? ChessMoveOrdinal::QUEEN_PROMOTION_MOVE : ChessMoveOrdinal::OTHER_PROMOTION_MOVE;
            }
            else if (searchStack->killer1 == move) {
                move.ordinal = ChessMoveOrdinal::KILLER1_MOVE;
            }
            else if (searchStack->killer2 == move) {
                move.ordinal = ChessMoveOrdinal::KILLER2_MOVE;
            }
            else if (searchStack->mateKiller1 == move) {
                move.ordinal = ChessMoveOrdinal::MATE_KILLER1_MOVE;
            }
            else if (searchStack->mateKiller2 == move) {
                move.ordinal = ChessMoveOrdinal::MATE_KILLER2_MOVE;
            }
            else {
                move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

                if (move.seeScore < 0) {
                    move.ordinal = ChessMoveOrdinal::UNSAFE_MOVE + static_cast<ChessMoveOrdinal>(move.seeScore);
                    continue;
                }

                const std::uint32_t mateHistoryScore = mateHistoryTable[board.sideToMove].get(src, dst);

                if (mateHistoryScore > 0) {
                    move.ordinal = ChessMoveOrdinal::MATE_HISTORY_MOVE + ChessMoveOrdinal(mateHistoryScore);
                    continue;
                }

                const std::uint32_t historyScore = historyTable.get(movingPiece, dst);

                if (historyScore > 0) {
                    move.ordinal = ChessMoveOrdinal::HISTORY_MOVE + ChessMoveOrdinal(historyScore);
                }
                else {
                    move.ordinal = ChessMoveOrdinal::UNCLASSIFIED_MOVE;
                }
            }
        }

        std::stable_sort(moveList.begin(), moveList.end(), std::greater<ChessMove>());
    }

    void reorderQuiescenceMoves(const ChessBoard& board, ChessMoveList& moveList, const ChessSearchStack* searchStack) const
    {
        const bool isWhiteToMove = board.isWhiteToMove();

        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        const Bitboard unsafeSquares = this->attackGenerator.unsafeSquares(board.sideToMove, otherPieces);

        const std::int32_t phase = board.getPhase();

        for (ChessMove& move : moveList) {
            const Square& src = move.src;
            const Square& dst = move.dst;

            const PieceType& movingPiece = board.pieces[src];
            const PieceType& capturedPiece = board.pieces[dst];

            const Evaluation capturedPieceEvaluation = MaterialParameters[capturedPiece];
            const Evaluation movingPieceEvaluation = MaterialParameters[movingPiece];
            const Score mvvlvaScore = capturedPieceEvaluation.mg - movingPieceEvaluation.mg;

            move.seeScore = INVALID_SCORE;

            if (searchStack->pvMove == move
                || searchStack->hashMove == move) {
                move.ordinal = ChessMoveOrdinal::PV_MOVE;
            }
            else if (movingPiece != PieceType::PAWN
                && (movingPiece != capturedPiece)
                && (unsafeSquares & OneShiftedBy(src)) != EmptyBitboard) {
                move.ordinal = ChessMoveOrdinal::UNSAFE_MOVE;
            }
            else if (searchStack->killer1 == move) {
                move.ordinal = ChessMoveOrdinal::QUIESENCE_KILLER1_MOVE;
            }
            else if (searchStack->killer2 == move) {
                move.ordinal = ChessMoveOrdinal::QUIESENCE_KILLER2_MOVE;
            }
            else {
                move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

                if (move.seeScore < 0) {
                    move.ordinal = ChessMoveOrdinal::BAD_QUIESENCE_MOVE + static_cast<ChessMoveOrdinal>(move.seeScore);
                }
                else if (move.seeScore > 0) {
                    move.ordinal = ChessMoveOrdinal::GOOD_QUIESENCE_MOVE + static_cast<ChessMoveOrdinal>(move.seeScore);
                }
                else {
                    move.ordinal = ChessMoveOrdinal::QUIESENCE_MOVE + static_cast<ChessMoveOrdinal>(capturedPieceEvaluation(phase));
                }
            }
        }

        std::stable_sort(moveList.begin(), moveList.end(), std::greater<ChessMove>());
    }
};
