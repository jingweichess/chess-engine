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

#include <cassert>

#include "evaluator.h"
#include "lazy.h"

#include "../bitboards/infront.h"
#include "../bitboards/magics.h"

#include "../bitboards/moves.h"
#include "../bitboards/passedpawn.h"

#include "../endgame/function.h"

#include "../types/bitboard.h"
#include "../types/controlboards.h"
#include "../types/score.h"

#include "../../game/math/bitreset.h"
#include "../../game/math/bitscan.h"
#include "../../game/math/byteswap.h"
#include "../../game/math/popcount.h"

#include "../../game/search/hashtable.h"

extern ChessEvaluation DoubledRooks;
extern ChessEvaluation EmptyFileQueen;
extern ChessEvaluation EmptyFileRook;
extern ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT];

extern ChessEvaluation ControlParameters[PieceType::PIECETYPE_COUNT][32];

extern ChessEvaluation ControlPstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];
extern ChessEvaluation KingControlPstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

extern ChessEvaluation QueenBehindPassedPawnPst[Square::SQUARE_COUNT];
extern ChessEvaluation RookBehindPassedPawnPst[Square::SQUARE_COUNT];

extern ChessEvaluation OutpostPstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

extern ChessEvaluation TropismParameters[PieceType::PIECETYPE_COUNT][16];

extern std::uint32_t Distance[File::FILE_COUNT][Rank::RANK_COUNT];

constexpr std::uint32_t EVALUATION_HASH_MEGABYTES = 1;
constexpr std::uint32_t PAWN_HASH_MEGABYTES = 1;

constexpr bool enableEvaluationHashtable = false;
constexpr bool enablePawnHashtable = false;

static Hashtable evaluationHashtable;
static Hashtable pawnHashtable;

ChessEvaluator::ChessEvaluator()
{
    InitializeEndgame(this->endgame);

    //if (enableEvaluationHashtable) {
    //    evaluationHashtable.initialize(EVALUATION_HASH_MEGABYTES);
    //}

    //if (enablePawnHashtable) {
    //    pawnHashtable.initialize(PAWN_HASH_MEGABYTES);
    //}
}

Score ChessEvaluator::evaluateImplementation(const BoardType& board, Depth currentDepth, Score alpha, Score beta)
{
    assert(!this->attackGenerator.dispatchIsInCheck(board));

    //if (enablePawnHashtable) {
    //    pawnHashtable.prefetch(board.pawnHashValue);
    //}

    const bool isWhiteToMove = board.isWhiteToMove();

    //1) Check for end game score
    if (board.getPhase() <= 9) {
        this->passedPawns[Color::WHITE] = this->calculatePassedPawns(board, Color::WHITE);
        this->passedPawns[Color::BLACK] = this->calculatePassedPawns(board, Color::BLACK);

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

    //if (enableEvaluationHashtable) {
    //    HashtableEntry hashtableEntryInfo;
    //    const bool hashFound = evaluationHashtable.search(hashtableEntryInfo, board.hashValue);
    //
    //    if (hashFound) {
    //        this->calculatePassedPawns(board, this->passedPawns[Color::WHITE], this->passedPawns[Color::BLACK]);

    //        const ChessEvaluation evaluation = { hashtableEntryInfo.getMg(), hashtableEntryInfo.getEg() };

    //        const std::uint32_t phase = board.getPhase();
    //        const Score result = evaluation(phase);

    //        return isWhiteToMove ? result : -result;
    //    }
    //}

    //3) Check for lazy evaluation
    const std::uint32_t phase = board.getPhase();

    const Score lazyEvaluation = this->lazyEvaluate(board);

    const Score lazyThreshold = LazyThreshold(phase);
    if (lazyEvaluation + lazyThreshold < alpha
        || lazyEvaluation - lazyThreshold >= beta) {

        this->passedPawns[Color::WHITE] = this->calculatePassedPawns(board, Color::WHITE);
        this->passedPawns[Color::BLACK] = this->calculatePassedPawns(board, Color::BLACK);

        return lazyEvaluation;
    }

    //4) Continue, actually evaluating the board
    EvaluationType evaluation = board.materialEvaluation + board.pstEvaluation;

    //5) Evaluate Pawn Structure.  Since the Pawn Evaluator is another evaluator, it will return score with side to move
    //  This must be done first because other evaluation terms rely on pawn structure calculations.
    evaluation += this->evaluatePawnStructure(board, this->passedPawns[Color::WHITE], this->passedPawns[Color::BLACK]);

    //ControlBoards controlBoards;

    //6) Loop through pieces
    evaluation += this->evaluatePawnAttacks(board, Color::WHITE);
    evaluation -= this->evaluatePawnAttacks(board, Color::BLACK);

    //controlBoards[PieceType::PAWN][Color::WHITE] = this->attackGenerator.unsafeSquares(Color::BLACK, board.whitePieces);
    //controlBoards[PieceType::PAWN][Color::BLACK] = this->attackGenerator.unsafeSquares(Color::WHITE, board.blackPieces);

    //TODO: Move Blocked Pawn/Piece Evaluation here

    const BitboardPair unsafeSquares = {
        this->attackGenerator.unsafeSquares(Color::WHITE, board.blackPieces),
        this->attackGenerator.unsafeSquares(Color::BLACK, board.whitePieces)
    };

    BitboardPair cumulativeControl { EmptyBitboard, EmptyBitboard };

    for (PieceType pieceType = PieceType::PAWN; pieceType <= PieceType::QUEEN; pieceType++) {
        if (pieceType != PieceType::PAWN) {
            for (const Color color : { Color::WHITE, Color::BLACK }) {
                const bool isSideToMove = color == board.sideToMove;

                const bool colorIsWhite = color == Color::WHITE;
                const std::int32_t multiplier = colorIsWhite ? 1 : -1;

                const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
                //const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

                const Square otherKingPosition = color == Color::WHITE ? board.blackKingPosition() : board.whiteKingPosition();

                //controlBoards[pieceType][color] = EmptyBitboard;

                const Bitboard srcPieces = colorPieces[pieceType];

                const bool hasPiecePair = std::popcount(srcPieces) > 1;

                if (hasPiecePair) {
                    evaluation += multiplier * PiecePairs[pieceType];
                }

                for (const Square src : SquareBitboardIterator(srcPieces)) {
                    Bitboard mobilityDstSquares = EmptyBitboard;

                    evaluation += multiplier * this->evaluateMobility(mobilityDstSquares, board.allPieces, colorPieces[PieceType::ALL], pieceType, src, unsafeSquares[color]);

                    //controlBoards[pieceType][color] |= mobilityDstSquares;

                    evaluation += multiplier * this->evaluateAttacks(board, color, pieceType, mobilityDstSquares);

                    evaluation += multiplier * this->evaluateTropism(pieceType, src, otherKingPosition);

                    //const Bitboard colorPawnDefenders = (colorIsWhite ? BlackPawnCaptures[src] : WhitePawnCaptures[src]) & colorPieces[PieceType::PAWN];
                    //if (colorPawnDefenders != EmptyBitboard) {
                    //    constexpr Bitboard outposts = FullBitboard; // 0x000000c3c3810000;
                    //    const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);

                    //    if ((OneShiftedBy(evaluatedSrc) & outposts) != EmptyBitboard) {
                    //        evaluation += multiplier * OutpostPstParameters[pieceType][evaluatedSrc];
                    //    }
                    //}

                    switch (pieceType) {
                    case PieceType::KNIGHT:
                    case PieceType::BISHOP:
                        break;
                    case PieceType::ROOK:
                        evaluation += multiplier * this->evaluateRook(colorPieces, mobilityDstSquares, board.allPieces, passedPawns[color], src, hasPiecePair);
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

        //const Bitboard control = cumulativeControl[Color::WHITE] | cumulativeControl[Color::BLACK];

        //controlBoards[pieceType][Color::WHITE] &= ~control;
        //controlBoards[pieceType][Color::BLACK] &= ~control;

        //cumulativeControl[Color::WHITE] |= controlBoards[pieceType][Color::WHITE];
        //cumulativeControl[Color::BLACK] |= controlBoards[pieceType][Color::BLACK];

        //for (Color color = Color::WHITE; color < Color::COLOR_COUNT; color++) {
        //    const bool colorIsWhite = color == Color::WHITE;
        //    const std::int32_t multiplier = colorIsWhite ? 1 : -1;

        //    const std::uint32_t controlCount = std::popcount(controlBoards[pieceType][color]);
        //    evaluation += multiplier * ControlParameters[pieceType][controlCount];

        //    //for (const Square src : SquareBitboardIterator(controlBoards[pieceType][color])) {
        //    //    const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);
        //    //    evaluation += multiplier * ControlPstParameters[pieceType][evaluatedSrc];
        //    //}

        //    //const Square otherKingSquare = board.otherKingPosition(color);
        //    //const Bitboard kingMoves = PieceMoves[PieceType::KING][otherKingSquare];

        //    //for (const Square src : SquareBitboardIterator(kingMoves & controlBoards[pieceType][color])) {
        //    //    const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);
        //    //    evaluation += multiplier * KingControlPstParameters[pieceType][evaluatedSrc];
        //    //}
        //}
    }

    //if (enableEvaluationHashtable) {
    //    evaluationHashtable.insert(board.pawnHashValue, evaluation.mg, evaluation.eg);
    //}

    //7) Begin Result Calculation
    const Score result = evaluation(phase);
    return isWhiteToMove ? result : -result;
}

ChessEvaluation ChessEvaluator::evaluateTropism(PieceType pieceType, Square src, Square otherKingPosition) const
{
    const std::uint32_t tropism = Distance[FileDistance(otherKingPosition, src)][RankDistance(otherKingPosition, src)];

    return TropismParameters[pieceType][tropism];
}

ChessEvaluation ChessEvaluator::evaluateRook(const Bitboard* colorPieces, Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src, bool hasPiecePair) const
{
    EvaluationType result = { ZERO_SCORE, ZERO_SCORE };

    if (hasPiecePair) {
        const Bitboard sideToMoveRooks = colorPieces[PieceType::ROOK];

        if ((sideToMoveRooks & mobilityDstSquares) != EmptyBitboard) {
            result += DoubledRooks;
        }
    }

    const File file = GetFile(src);
    const Bitboard piecesInSameFile = allPieces & FileBitboard[file];

    if (piecesInSameFile == OneShiftedBy(src)) {
        result += EmptyFileRook;
    }
    //else if ((piecesInSameFile & passedPawns) != EmptyBitboard) {
    //    const Bitboard possiblePassedPawns = mobilityDstSquares & passedPawns & piecesInSameFile;

    //    for (const Square dst : SquareBitboardIterator(possiblePassedPawns)) {
    //        result += RookBehindPassedPawnPst[dst];
    //    }
    //}

    return result;
}

//ChessEvaluation ChessEvaluator::evaluateQueen(Bitboard mobilityDstSquares, Bitboard allPieces, Bitboard passedPawns, Square src) const
//{
//    EvaluationType result = { ZERO_SCORE, ZERO_SCORE };
//
//    const File file = GetFile(src);
//    const Bitboard piecesInSameFile = allPieces & FileBitboard[file];
//
//    if (piecesInSameFile == OneShiftedBy(src)) {
//        result += EmptyFileQueen;
//    }
//    else if ((piecesInSameFile & passedPawns) != EmptyBitboard) {
//        const Bitboard possiblePassedPawns = mobilityDstSquares & passedPawns & piecesInSameFile;
//
//        for (const Square dst : SquareBitboardIterator(possiblePassedPawns)) {
//            result += QueenBehindPassedPawnPst[dst];
//        }
//    }
//
//    return result;
//}

Score ChessEvaluator::lazyEvaluateImplementation(const BoardType& board)
{
    const EvaluationType evaluation = board.materialEvaluation + board.pstEvaluation;

    const std::uint32_t phase = board.getPhase();
    const Score result = evaluation(phase);

    const bool isWhiteToMove = board.isWhiteToMove();
    return isWhiteToMove ? result : -result;
}

void ChessEvaluator::prefetch(Hash hashValue) const
{
    //if (enableEvaluationHashtable) {
    //    evaluationHashtable.prefetch(hashValue);
    //}
}
