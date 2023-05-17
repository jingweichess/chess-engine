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

#include <cassert>

#include "evaluator.h"
#include "lazy.h"

#include "../bitboards/infront.h"
#include "../bitboards/magics.h"

#include "../bitboards/moves.h"
#include "../bitboards/passedpawn.h"

#include "../endgame/function.h"

#include "../types/bitboard.h"
#include "../types/score.h"

#include "../../game/math/bitreset.h"
#include "../../game/math/bitscan.h"
#include "../../game/math/byteswap.h"
#include "../../game/math/popcount.h"

extern ChessEvaluation DoubledRooks;
extern ChessEvaluation EmptyFileQueen;
extern ChessEvaluation EmptyFileRook;
extern ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT];
extern ChessEvaluation QueenBehindPassedPawnPst[Square::SQUARE_COUNT];
extern ChessEvaluation RookBehindPassedPawnPst[Square::SQUARE_COUNT];
extern ChessEvaluation TropismParameters[PieceType::PIECETYPE_COUNT][16];

extern ChessEvaluation PawnBlockedPstParameters[Color::COLOR_COUNT][Square::SQUARE_COUNT];
extern ChessEvaluation PawnChainBackPstParameters[Square::SQUARE_COUNT];
extern ChessEvaluation PawnChainFrontPstParameters[Square::SQUARE_COUNT];
extern ChessEvaluation PawnDoubledPstParameters[Square::SQUARE_COUNT];
extern ChessEvaluation PawnPassedPstParameters[Square::SQUARE_COUNT];
extern ChessEvaluation PawnPhalanxPstParameters[Square::SQUARE_COUNT];
extern ChessEvaluation PawnUnstoppablePstParameters[Square::SQUARE_COUNT];

extern std::uint32_t Distance[File::FILE_COUNT][Rank::RANK_COUNT];

constexpr std::uint32_t EVALUATION_HASH_MEGABYTES = 1;
constexpr std::uint32_t PAWN_HASH_MEGABYTES = 1;

constexpr bool enableEvaluationHashtable = false;
constexpr bool enablePawnHashtable = false;

static ChessHashtable evaluationHashtable;
static ChessHashtable pawnHashtable;

ChessEvaluator::ChessEvaluator()
{
    InitializeEndgame(this->endgame);

    if (enableEvaluationHashtable) {
        evaluationHashtable.initialize(EVALUATION_HASH_MEGABYTES);
    }

    if (enablePawnHashtable) {
        pawnHashtable.initialize(PAWN_HASH_MEGABYTES);
    }
}

Score ChessEvaluator::evaluateImplementation(const BoardType& board, Depth currentDepth, Score alpha, Score beta)
{
    assert(!this->attackGenerator.dispatchIsInCheck(board));

    if (enablePawnHashtable) {
        pawnHashtable.prefetch(board.pawnHashValue);
    }

    const bool whiteToMove = board.sideToMove == Color::WHITE;

    //1) Check for end game score
    if (std::popcount(board.allPieces) <= 9) {
        Score endgameScore;

        const bool endgameFound = this->endgame.probe(board, endgameScore);

        if (endgameFound) {
            if (IsMateScore(endgameScore)) {
                endgameScore += endgameScore > DRAW_SCORE ? -currentDepth : currentDepth;
            }

            return endgameScore;
        }

        //2) Check for lone king
        else if (PopCountIsOne(board.whitePieces[PieceType::ALL])
            || PopCountIsOne(board.blackPieces[PieceType::ALL])) {
            weakKingEndgameFunction(board, endgameScore);

            return endgameScore;
        }
    }

    if (enableEvaluationHashtable) {
        HashtableEntryInfo hashtableEntryInfo;
        const bool hashFound = evaluationHashtable.search(hashtableEntryInfo, board.hashValue);
    
        if (hashFound) {
            this->calculatePassedPawns(board, this->passedPawns[Color::WHITE], this->passedPawns[Color::BLACK]);

            const ChessEvaluation evaluation = { hashtableEntryInfo.getMg(), hashtableEntryInfo.getEg() };

            const std::uint32_t phase = std::popcount(board.allPieces);
            const Score result = evaluation(phase);

            return whiteToMove ? result : -result;
        }
    }

    //3) Check for lazy evaluation
    //const Score lazyEvaluation = this->lazyEvaluate(board);
    const std::uint32_t phase = std::popcount(board.allPieces);

    //const Score lazyThreshold = LazyThreshold(phase);
    //if (lazyEvaluation + lazyThreshold < alpha
    //    || lazyEvaluation - lazyThreshold >= beta) {
    //    return lazyEvaluation;
    //}

    //4) Continue, actually evaluating the board
    EvaluationType evaluation = board.materialEvaluation + board.pstEvaluation;

    //5) Evaluate Pawn Structure.  Since the Pawn Evaluator is another evaluator, it will return score with side to move
    //  This must be done first because other evaluation terms rely on pawn structure calculations.
    evaluation += this->evaluatePawnStructure(board, this->passedPawns[Color::WHITE], this->passedPawns[Color::BLACK]);

    //6) Loop through pieces
    for (Color color = Color::WHITE; color < Color::COLOR_COUNT; color++) {
        const bool colorIsWhite = color == Color::WHITE;
        const std::int32_t multiplier = colorIsWhite ? 1 : -1;

        const Bitboard* piecesToMove = colorIsWhite ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

        const Square otherKingPosition = color == Color::WHITE ? board.blackKingPosition() : board.whiteKingPosition();

        evaluation += multiplier * this->evaluatePawnAttacks(board, piecesToMove, otherPieces, color);

        //TODO: Move Blocked Pawn/Piece Evaluation here

        const Bitboard unsafeSquares = this->attackGenerator.unsafeSquares(color, otherPieces);

        for (PieceType pieceType = PieceType::KNIGHT; pieceType <= PieceType::QUEEN; pieceType++) {
            const Bitboard srcPieces = piecesToMove[pieceType];
            const bool hasPiecePair = std::popcount(srcPieces) > 1;

            if (hasPiecePair) {
                evaluation += multiplier * PiecePairs[pieceType];
            }

            for (const Square src : SquareBitboardIterator(srcPieces)) {
                Bitboard mobilityDstSquares = EmptyBitboard;

                evaluation += multiplier * this->evaluateMobility(mobilityDstSquares, board.allPieces, piecesToMove[PieceType::ALL], pieceType, src, unsafeSquares);

                evaluation += multiplier * this->evaluateAttacks(board, color, pieceType, mobilityDstSquares);
                
                evaluation += multiplier * this->evaluateTropism(pieceType, src, otherKingPosition);

                switch (pieceType) {
                case PieceType::KNIGHT:
                case PieceType::BISHOP:
                    break;
                case PieceType::ROOK:
                    //evaluation += multiplier * this->evaluateRook(piecesToMove, mobilityDstSquares, board.allPieces, passedPawns[color], src, hasPiecePair);
                    break;
                case PieceType::QUEEN:
                    //evaluation += multiplier * this->evaluateQueen(mobilityDstSquares, board.allPieces, passedPawns[color], src);
                    break;
                default:
                    assert(0);
                }
            }
        }
    }

    if (enableEvaluationHashtable) {
        evaluationHashtable.insert(board.pawnHashValue, evaluation.mg, evaluation.eg);
    }

    //7) Begin Result Calculation
    const Score result = evaluation(phase);
    return whiteToMove ? result : -result;
}

ChessEvaluation ChessEvaluator::evaluatePawnAttacks(const BoardType& board, const Bitboard* piecesToMove, const Bitboard* otherPieces, Color color) const
{
    EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

    const Bitboard pawnAttacks = attackGenerator.unsafeSquares(~color, piecesToMove);
    const Bitboard allPawnAttacks = pawnAttacks & otherPieces[PieceType::ALL] & ~otherPieces[PieceType::PAWN];

    for (const Square dst : SquareBitboardIterator(allPawnAttacks)) {
        const PieceType attackedPiece = board.pieces[dst];
        result += AttackParameters[PieceType::PAWN][attackedPiece];
    }

    return result;
}

ChessEvaluation ChessEvaluator::evaluatePawnStructure(const BoardType& board, Bitboard& whitePassedPawns, Bitboard& blackPassedPawns) const
{
    if (enablePawnHashtable) {
        HashtableEntryInfo hashtableEntryInfo;
        const bool hashFound = pawnHashtable.search(hashtableEntryInfo, board.pawnHashValue);

        if (hashFound) {
            this->calculatePassedPawns(board, whitePassedPawns, blackPassedPawns);
            return { hashtableEntryInfo.getMg(), hashtableEntryInfo.getEg() };
        }
    }

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

            //1) Check for a chained pawn
            if ((colorPawnCaptures[src] & colorPieces[PieceType::PAWN]) != EmptyBitboard) {
                result += multiplier * PawnChainBackPstParameters[evaluatedSrc];

                const Bitboard frontChainPawns = colorPawnCaptures[src] & colorPieces[PieceType::PAWN];

                for (const Square dst : SquareBitboardIterator(frontChainPawns)) {
                    const Square evaluatedDst = colorIsWhite ? dst : FlipSquareOnHorizontalLine(dst);
                    result += multiplier * PawnChainFrontPstParameters[evaluatedDst];
                }
            }

            //2) Check for phalanxed pawn
            File file = GetFile(src);
            if (file != File::_H
                && (OneShiftedBy(src + Direction::RIGHT) & colorPawns) != EmptyBitboard) {
                //result += multiplier * (PawnPhalanxPstParameters[evaluatedSrc] + PawnPhalanxPstParameters[evaluatedSrc + Direction::RIGHT]);
            }

            //3) Check for a blocked pawn
            //if ((colorPawnMoves[src] & colorPieces[PieceType::ALL]) != EmptyBitboard) {
            //    result += multiplier * PawnBlockedPstParameters[Color::CURRENT_COLOR][evaluatedSrc];
            //}
            //else if ((colorPawnMoves[src] & otherPieces[PieceType::ALL]) != EmptyBitboard) {
            //    result += multiplier * PawnBlockedPstParameters[Color::OTHER_COLOR][evaluatedSrc];
            //}

            //4) Check for a passed pawn
            if ((PassedPawnCheck(evaluatedSrc) & evaluatedOtherPawns) == EmptyBitboard) {
                passedPawns |= src;

                constexpr Rank lastRank = Rank::_8;
                const Square lastRankSquare = SetRank(evaluatedSrc, lastRank);

                result += multiplier * PawnPassedPstParameters[evaluatedSrc];

                //4b) The pawn is unstoppable if it's closer to the back rank than the enemy king is to the pawn
                //if (SquareDistance(evaluatedSrc, lastRankSquare) < SquareDistance(evaluatedSrc, otherKingPosition)) {
                //    result += multiplier * PawnUnstoppablePstParameters[evaluatedSrc];
                //}
            }

            //5) Check for doubled/tripled pawns
            const Bitboard pawnsInFrontOfSrc = SquaresInFront(evaluatedSrc) & evaluatedColorPawns;
            if (pawnsInFrontOfSrc != EmptyBitboard) {
//                if (ConstExprPopCountIsOne(pawnsInFrontOfSrc)) {
                    result += multiplier * PawnDoubledPstParameters[evaluatedSrc];
//                }
//                else {
//                    result += multiplier * PawnTripledPstParameters[evaluatedSrc];
//                }
            }
        }

        if (colorIsWhite) {
            whitePassedPawns = passedPawns;
        }
        else {
            blackPassedPawns = passedPawns;
        }
    }

    if (enablePawnHashtable) {
        pawnHashtable.insert(board.pawnHashValue, result.mg, result.eg);
    }

    return result;
}

ChessEvaluation ChessEvaluator::evaluateTropism(PieceType pieceType, Square src, Square otherKingPosition) const
{
    const std::uint32_t tropism = Distance[FileDistance(otherKingPosition, src)][RankDistance(otherKingPosition, src)];

    return TropismParameters[pieceType][tropism];
}

ChessEvaluation ChessEvaluator::evaluateRook(const Bitboard* piecesToMove, Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src, bool hasPiecePair) const
{
    EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

    if (hasPiecePair) {
        const Bitboard sideToMoveRooks = piecesToMove[PieceType::ROOK];

        if ((sideToMoveRooks & mobilityDstSquares) == EmptyBitboard) {
            result += DoubledRooks;
        }
    }

    const File file = GetFile(src);
    const Bitboard piecesInSameFile = allPieces & FileBitboard[file];

    if (piecesInSameFile == OneShiftedBy(src)) {
        result += EmptyFileRook;
    }
    else if ((piecesInSameFile & passedPawns) != EmptyBitboard) {
        const Bitboard possiblePassedPawns = mobilityDstSquares & passedPawns & piecesInSameFile;

        for (const Square dst : SquareBitboardIterator(possiblePassedPawns)) {
            result += RookBehindPassedPawnPst[dst];
        }
    }

    return result;
}

ChessEvaluation ChessEvaluator::evaluateQueen(Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src) const
{
    EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

    const File file = GetFile(src);
    const Bitboard piecesInSameFile = allPieces & FileBitboard[file];

    if (piecesInSameFile == OneShiftedBy(src)) {
        result += EmptyFileQueen;
    }
    else if ((piecesInSameFile & passedPawns) != EmptyBitboard) {
        const Bitboard possiblePassedPawns = mobilityDstSquares & passedPawns & piecesInSameFile;

        for (const Square dst : SquareBitboardIterator(possiblePassedPawns)) {
            result += QueenBehindPassedPawnPst[dst];
        }
    }

    return result;
}

Score ChessEvaluator::lazyEvaluateImplementation(const BoardType& board)
{
    const EvaluationType evaluation = board.materialEvaluation + board.pstEvaluation;

    const std::uint32_t phase = board.getPhase();
    const Score result = evaluation(phase);

    const bool whiteToMove = board.sideToMove == Color::WHITE;
    return whiteToMove ? result : -result;
}

void ChessEvaluator::prefetch(Hash hashValue) const
{
    if (enableEvaluationHashtable) {
        evaluationHashtable.prefetch(hashValue);
    }
}
