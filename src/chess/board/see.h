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

#include "board.h"

#include "../../game/types/color.h"

#include "../types/piecetype.h"
#include "../types/score.h"
#include "../types/square.h"

#include "../bitboards/inbetween.h"
#include "../bitboards/moves.h"

constexpr std::array<Score, PieceType::PIECETYPE_COUNT> SeeMaterialValues = {
    ZERO_SCORE, PAWN_SCORE, KNIGHT_SCORE, BISHOP_SCORE, ROOK_SCORE, QUEEN_SCORE, WIN_SCORE
};

class ChessStaticExchangeEvaluator
{
public:
    constexpr ChessStaticExchangeEvaluator() = default;
    constexpr ~ChessStaticExchangeEvaluator() = default;

    constexpr Score staticExchangeEvaluation(const ChessBoard& board, const ChessMove& move) const {
        if (move.seeScore != INVALID_SCORE) {
            return move.seeScore;
        }

        return this->staticExchangeEvaluation(board, move.src, move.dst);
    }

    constexpr Score staticExchangeEvaluation(const ChessBoard& board, Square src, Square dst) const
    {
        bool isWhiteToMove = board.isWhiteToMove();

        const PieceType movingPiece = board.pieces[src];
        const PieceType capturedPiece = dst == board.enPassant ? PieceType::PAWN : board.pieces[dst];

        //1) If the captured piece value > the moving piece value, return a quick, pessimistic SEE score
        if (SeeMaterialValues[capturedPiece] > SeeMaterialValues[movingPiece]) {
            return SeeMaterialValues[capturedPiece] - SeeMaterialValues[movingPiece];
        }

        //2) Get all of the valid pieces on the board that can be used for SEE
        Bitboard allAttackers = WhitePawnCaptures[dst] & board.blackPieces[PieceType::PAWN]
            | BlackPawnCaptures[dst] & board.whitePieces[PieceType::PAWN];

        for (PieceType piece = PieceType::KNIGHT; piece <= PieceType::KING; piece++) {
            allAttackers |= PieceMoves[piece][dst] & (board.whitePieces[piece] | board.blackPieces[piece]);
        }

        //2a) Exclude the first attacker on the source square
        Bitboard allPieces = board.allPieces ^ OneShiftedBy(src) & ~OneShiftedBy(dst);  //EnPassant can mess this up if we use xor
        allAttackers = allAttackers & allPieces;

        //2b) If there are no valid attackers, return value of the captured piece
        if ((allAttackers & allPieces) == EmptyBitboard) {
            return SeeMaterialValues[capturedPiece];
        }

        //3) Set Side To Move Attackers
        const Bitboard* piecesToMove = isWhiteToMove ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        isWhiteToMove = !isWhiteToMove;
        std::swap(piecesToMove, otherPieces);

        //4) If the other side has no pieces to recapture, return the value of the captured piece
            //without capturing the moved piece
        Bitboard sideToMoveAttackers = allAttackers & piecesToMove[PieceType::ALL];

        if (sideToMoveAttackers == EmptyBitboard) {
            return SeeMaterialValues[capturedPiece];
        }

        //5) Begin the main loop
        PieceType lastMovedPiece = movingPiece;
        std::array<PieceType, Color::COLOR_COUNT> bestKnown = { PieceType::PAWN, PieceType::PAWN };

        std::array<Score, 32> gain;
        gain[0] = SeeMaterialValues[capturedPiece];
        std::uint32_t depth = 1;

        bool attackersFound = false;

        do {
            //7) Find the lowest value attacker for the current side to move
            PieceType currentPieceType = bestKnown[isWhiteToMove];

            attackersFound = false;
            Bitboard attackingPieces = EmptyBitboard;

            while (!attackersFound && currentPieceType <= PieceType::KING) {
                attackingPieces = sideToMoveAttackers & piecesToMove[currentPieceType];

                if ((sideToMoveAttackers & attackingPieces) != EmptyBitboard) {
                    attackersFound = true;
                }
                else {
                    currentPieceType++;
                }
            }

            if (!attackersFound) {
                break;
            }

            bestKnown[isWhiteToMove] = currentPieceType;

            //Scan through attackingPieces for one which is a valid attack to the square
            bool specificAttackerFound = false;
            for (const Square attackSrc : SquareBitboardIterator(attackingPieces)) {
                if ((InBetween(attackSrc, dst) & allPieces) != EmptyBitboard) {
                    continue;
                }

                specificAttackerFound = true;

                //This is a valid attacker.  Do the gain and remove from attacker list and all pieces
                allAttackers ^= OneShiftedBy(attackSrc);
                allPieces ^= OneShiftedBy(attackSrc);

                gain[depth] = SeeMaterialValues[lastMovedPiece] - gain[depth - 1];
                depth++;

                lastMovedPiece = currentPieceType;

                bestKnown[isWhiteToMove] = PieceType::PAWN;

                isWhiteToMove = !isWhiteToMove;
                std::swap(piecesToMove, otherPieces);

                break;
            }

            if (!specificAttackerFound) {
                bestKnown[isWhiteToMove]++;
            }

            sideToMoveAttackers = allAttackers & piecesToMove[PieceType::ALL];
        } while ((sideToMoveAttackers != EmptyBitboard)
            || !attackersFound);

        //11) Finish calculating the SEE
        while (--depth) {
            gain[depth - 1] = std::min(-gain[depth], gain[depth - 1]);
        }

        return gain[0];
    }
};
