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

#include "../bitboards/infront.h"
#include "../bitboards/passedpawn.h"

#include "../board/attackgenerator.h"
#include "../board/board.h"

#include "../endgame/endgame.h"

#include "../../game/eval/evaluator.h"

#include "../../game/math/byteswap.h"

#include "../hash/chesshashtable.h"

#include "constructor.h"

extern ChessEvaluation PassedPawnDefended;
extern std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> PassedPawnBlockedByPiece;

extern ChessEvaluation AttackParameters[PieceType::PIECETYPE_COUNT][PieceType::PIECETYPE_COUNT];
extern ChessEvaluation MobilityParameters[PieceType::PIECETYPE_COUNT][32];

extern ChessEvaluation PawnBlockedPstParameters[Color::COLOR_COUNT][Square::SQUARE_COUNT];

//extern ChessEvaluation PawnChainBackByRank[Rank::RANK_COUNT];
//extern ChessEvaluation PawnChainFrontByRank[Rank::RANK_COUNT];

extern ChessEvaluation PawnChainBack;
extern ChessEvaluation PawnChainFront;

extern ChessEvaluation PawnChainBackPerRank;
extern ChessEvaluation PawnChainFrontPerRank;

extern ChessEvaluation PawnDoubledByRank[Rank::RANK_COUNT];
extern ChessEvaluation PawnPassedByRank[Rank::RANK_COUNT];
extern ChessEvaluation PawnPhalanxByRank[Rank::RANK_COUNT];

class ChessEvaluator : public Evaluator<ChessEvaluator, ChessBoard, ChessEvaluation>
{
protected:
    ChessEndgame endgame;

    ChessAttackGenerator attackGenerator;

    Bitboard passedPawns[Color::COLOR_COUNT]{ EmptyBitboard, EmptyBitboard };

    constexpr void calculatePassedPawns(const BoardType& board, Bitboard& whitePassedPawns, Bitboard& blackPassedPawns) const
    {
        whitePassedPawns = this->calculatePassedPawns(board, Color::WHITE);
        blackPassedPawns = this->calculatePassedPawns(board, Color::BLACK);
    }

    constexpr EvaluationType evaluateAttacks(const BoardType& board, Color color, PieceType srcPiece, Bitboard mobilityDstSquares) const
    {
        EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

        const bool colorIsWhite = color == Color::WHITE;

        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;
        const Bitboard attackSquares = mobilityDstSquares & otherPieces[PieceType::ALL];

        for (const Square dst : SquareBitboardIterator(attackSquares)) {
            const PieceType attackedPiece = board.pieces[dst];

            assert(attackedPiece != PieceType::KING);

            //TODO: Account for side to move
            result += AttackParameters[srcPiece][attackedPiece];
        }

        return result;
    }

    constexpr ChessEvaluation evaluatePawnAttacks(const BoardType& board, Color color) const
    {
        EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

        const bool colorIsWhite = color == Color::WHITE;
        const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

        const Bitboard pawnAttacks = this->attackGenerator.unsafeSquares(~color, colorPieces);
        const Bitboard allPawnAttacks = pawnAttacks & otherPieces[PieceType::ALL] & ~otherPieces[PieceType::PAWN];

        for (const Square dst : SquareBitboardIterator(allPawnAttacks)) {
            const PieceType attackedPiece = board.pieces[dst];
            result += AttackParameters[PieceType::PAWN][attackedPiece];
        }

        return result;
    }

    ChessEvaluation evaluateTropism(PieceType pieceType, Square src, Square otherKingPosition) const;

    constexpr ChessEvaluation evaluatePawnStructure(const BoardType& board, Bitboard& whitePassedPawns, Bitboard& blackPassedPawns) const
    {
        //if (enablePawnHashtable) {
        //    HashtableEntryInfo hashtableEntryInfo;
        //    const bool hashFound = pawnHashtable.search(hashtableEntryInfo, board.pawnHashValue);

        //    if (hashFound) {
        //        this->calculatePassedPawns(board, whitePassedPawns, blackPassedPawns);
        //        return { hashtableEntryInfo.getMg(), hashtableEntryInfo.getEg() };
        //    }
        //}

        EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

        for (Color color = Color::COLOR_START; color < Color::COLOR_COUNT; color++) {
            const bool colorIsWhite = color == Color::WHITE;
            const std::int32_t multiplier = colorIsWhite ? 1 : -1;

            const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
            const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

            const Bitboard colorPawns = colorPieces[PieceType::PAWN];
            const Bitboard otherPawns = otherPieces[PieceType::PAWN];

            const Square otherKingPosition = colorIsWhite ? board.blackKingPosition() : board.whiteKingPosition();

            const Bitboard* colorPawnCaptures = colorIsWhite ? WhitePawnCaptures : BlackPawnCaptures;

            const Bitboard evaluatedColorPawns = colorIsWhite ? colorPawns : SwapBytes(colorPawns);
            const Bitboard evaluatedOtherPawns = colorIsWhite ? otherPawns : SwapBytes(otherPawns);

            Bitboard passedPawns = EmptyBitboard;

            for (const Square src : SquareBitboardIterator(colorPawns)) {
                const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);
                const Rank evaluatedRank = GetRank(evaluatedSrc);

                const Bitboard pawnDefends = colorIsWhite ? BlackPawnCaptures[src] : WhitePawnCaptures[src];
                const Bitboard pawnsDefendedBy = pawnDefends & colorPawns;

                const bool isDefendedByPawn = pawnsDefendedBy != EmptyBitboard;

                //1) Check for a chained pawn
                if (isDefendedByPawn) {
                    //result += multiplier * PawnChainFrontPstParameters[evaluatedSrc];
                    result += multiplier * (PawnChainFront + ~evaluatedRank * PawnChainFrontPerRank);

                    for (const Square dst : SquareBitboardIterator(pawnsDefendedBy)) {
                        const Square evaluatedDst = colorIsWhite ? dst : FlipSquareOnHorizontalLine(dst);
                        //result += multiplier * PawnChainBackPstParameters[evaluatedDst];
                        result += multiplier * (PawnChainBack + ~evaluatedRank * PawnChainBackPerRank);
                    }
                }

                //2) Check for phalanxed pawn
                const File file = GetFile(src);
                if (file != File::_H
                    && (OneShiftedBy(src + Direction::RIGHT) & colorPawns) != EmptyBitboard) {
                    result += multiplier * PawnPhalanxByRank[evaluatedRank];
                }

                //3) Check for a blocked pawn
                //if ((colorPawnMoves[src] & colorPieces[PieceType::ALL]) != EmptyBitboard) {
                //    result += multiplier * PawnBlockedPstParameters[Color::CURRENT_COLOR][evaluatedSrc];
                //}
                //else if ((colorPawnMoves[src] & otherPieces[PieceType::ALL]) != EmptyBitboard) {
                //    result += multiplier * PawnBlockedPstParameters[Color::OTHER_COLOR][evaluatedSrc];
                //}

                //4) Check for doubled/tripled pawns
                const Bitboard pawnsInFrontOfSrc = SquaresInFront(evaluatedSrc) & evaluatedColorPawns;
                //const Bitboard pawnsInFrontOfSrc = OneShiftedBy(evaluatedSrc + Direction::UP) & evaluatedColorPawns;

                const bool doubled = pawnsInFrontOfSrc != EmptyBitboard;
                if (doubled) {
                    if (PopCountIsOne(pawnsInFrontOfSrc)) {
                        result += multiplier * PawnDoubledByRank[evaluatedRank];
                    }
                    else {
                        result += multiplier * PawnDoubledByRank[evaluatedRank];
                        //result += multiplier * PawnTripledByRank[evaluatedRank];
                    }
                }

                //5) Check for a passed pawn
                const bool passed = (PassedPawnCheck(evaluatedSrc) & evaluatedOtherPawns) == EmptyBitboard;
                if (passed) {
                    passedPawns |= src;

                    result += multiplier * PawnPassedByRank[evaluatedRank];

                    if (isDefendedByPawn) {
                        result += multiplier * PassedPawnDefended;
                    }

                    //const Direction forward = colorIsWhite ? Direction::UP : Direction::DOWN;
                    //const PieceType blockedByPieceType = board.pieceAt(src + forward);
                    //if ((colorPawns & OneShiftedBy(src + forward)) == EmptyBitboard
                    //    && blockedByPieceType != PieceType::NO_PIECE) {
                    //    assert(blockedByPieceType != PieceType::PAWN);

                    //    result += multiplier * PassedPawnBlockedByPiece[blockedByPieceType];
                    //}

                    //constexpr Rank lastRank = Rank::_8;
                    //const Square lastRankSquare = SetRank(evaluatedSrc, lastRank);

                    //4b) The pawn is unstoppable if it's closer to the back rank than the enemy king is to the pawn
                    //if (SquareDistance(evaluatedSrc, lastRankSquare) < SquareDistance(evaluatedSrc, otherKingPosition)) {
                    //    result += multiplier * PawnUnstoppablePstParameters[evaluatedSrc];
                    //}
                }
            }

            assert(passedPawns == this->calculatePassedPawns(board, color));

            if (colorIsWhite) {
                whitePassedPawns = passedPawns;
            }
            else {
                blackPassedPawns = passedPawns;
            }
        }

        //if (enablePawnHashtable) {
        //    pawnHashtable.insert(board.pawnHashValue, result.mg, result.eg);
        //}

        return result;
    }

    //EvaluationType evaluateRook(const Bitboard* colorPieces, Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src, bool hasPiecePair) const;
    //EvaluationType evaluateQueen(Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src) const;
public:
	ChessEvaluator();
    ~ChessEvaluator() = default;

    constexpr Bitboard calculatePassedPawns(const BoardType& board, Color color) const
    {
        const bool colorIsWhite = color == Color::WHITE;

        const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

        const Bitboard colorPawns = colorPieces[PieceType::PAWN];
        const Bitboard otherPawns = otherPieces[PieceType::PAWN];

        const Bitboard evaluatedOtherPawns = colorIsWhite ? otherPawns : SwapBytes(otherPawns);

        Bitboard result = EmptyBitboard;

        for (const Square src : SquareBitboardIterator(colorPawns)) {
            const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);

            if ((PassedPawnCheck(evaluatedSrc) & evaluatedOtherPawns) == EmptyBitboard) {
                result |= src;
            }
        }

        return result;
    }

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

    constexpr Bitboard getPassedPawnsForColor(Color color) const
    {
        return this->passedPawns[color];
    }

	Score lazyEvaluateImplementation(const BoardType& board);

    void prefetch(Hash hashValue) const;
};
