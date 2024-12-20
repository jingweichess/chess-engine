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
#include "../types/score.h"

#include "../../game/math/bitreset.h"
#include "../../game/math/bitscan.h"
#include "../../game/math/byteswap.h"
#include "../../game/math/popcount.h"

#include "../../game/search/hashtable.h"

extern ChessEvaluation DoubledRooks;
extern ChessEvaluation EmptyFileQueen;
extern ChessEvaluation EmptyFileRook;
extern ChessEvaluation Tempo;

extern std::array<ChessEvaluation, 2> BishopPawns;
extern std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> KingAttacks;
extern std::array<ChessEvaluation, 2> KingShield;
extern std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> Outpost;

extern ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT];

extern ChessEvaluation QueenBehindPassedPawnPst[Square::SQUARE_COUNT];
extern ChessEvaluation RookBehindPassedPawnPst[Square::SQUARE_COUNT];

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

    if (enableEvaluationHashtable) {
        evaluationHashtable.initialize(EVALUATION_HASH_MEGABYTES);
    }

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

    if (enableEvaluationHashtable) {
        HashtableEntry hashtableEntryInfo;
        const bool hashFound = evaluationHashtable.search(hashtableEntryInfo, board.hashValue);
    
        if (hashFound) {
            this->passedPawns[Color::WHITE] = this->calculatePassedPawns(board, Color::WHITE);
            this->passedPawns[Color::BLACK] = this->calculatePassedPawns(board, Color::BLACK);

            const ChessEvaluation evaluation = { hashtableEntryInfo.getMg(), hashtableEntryInfo.getEg() };

            const std::int32_t phase = board.getPhase();
            const Score result = evaluation(phase);

            return isWhiteToMove ? result : -result;
        }
    }

    //3) Check for lazy evaluation
    const std::int32_t phase = board.getPhase();

    const Score lazyEvaluation = this->lazyEvaluate(board);

    const Score lazyThreshold = 3 * PAWN_SCORE;
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

    //6) Loop through pieces
    evaluation += this->evaluatePawnAttacks(board, Color::WHITE);
    evaluation -= this->evaluatePawnAttacks(board, Color::BLACK);

    const BitboardPair UnsafeSquares = {
        this->attackGenerator.unsafeSquares(Color::WHITE, board.blackPieces),
        this->attackGenerator.unsafeSquares(Color::BLACK, board.whitePieces)
    };

    for (const Color color : { Color::WHITE, Color::BLACK }) {
        const bool isSideToMove = color == board.sideToMove;

        const bool colorIsWhite = color == Color::WHITE;
        const std::int32_t multiplier = colorIsWhite ? 1 : -1;

        const Bitboard* colorPieces = colorIsWhite ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = colorIsWhite ? board.blackPieces : board.whitePieces;

        const Square otherKingPosition = color == Color::WHITE ? board.blackKingPosition() : board.whiteKingPosition();
        const Bitboard otherKingMoves = PieceMoves[PieceType::KING][otherKingPosition] & ~otherPieces[PieceType::KING];

        for (PieceType pieceType = PieceType::KNIGHT; pieceType <= PieceType::QUEEN; pieceType++) {
            if (std::popcount(colorPieces[pieceType]) > 1) {
                evaluation += multiplier * PiecePairs[pieceType];
            }
        }

        const Bitboard allColorPieces = colorPieces[PieceType::ALL];
        for (const Square src : SquareBitboardIterator(allColorPieces ^ colorPieces[PieceType::PAWN])) {
            const PieceType pieceType = board.pieceAt(src);
            const Square evaluatedSrc = colorIsWhite ? src : FlipSquareOnHorizontalLine(src);

            const Bitboard PawnDefends = colorIsWhite ? BlackPawnCaptures[src] : WhitePawnCaptures[src];
            const Bitboard pawnsDefendedBy = PawnDefends & colorPieces[PieceType::PAWN];

            if (pawnsDefendedBy != EmptyBitboard) {
                evaluation += multiplier * Outpost[pieceType];
            }

            Bitboard mobilityDstSquares = EmptyBitboard;

            switch (pieceType) {
            case PieceType::KNIGHT:
                mobilityDstSquares = PieceMoves[PieceType::KNIGHT][src];
                break;
            case PieceType::BISHOP: {
                mobilityDstSquares = BishopMagic(src, board.allPieces);

                const Bitboard colorSameBishopColorPawns = SquaresSameColorAs(colorPieces[PieceType::PAWN], src);
                const std::uint32_t ourPawnsOnSameColor = std::popcount(colorSameBishopColorPawns);

                evaluation += multiplier * std::popcount(ourPawnsOnSameColor) * BishopPawns[Color::CURRENT_COLOR];

                const Bitboard otherOppositeBishopColorPawns = SquaresOppositeColorAs(otherPieces[PieceType::PAWN], src);
                const std::uint32_t otherPawnsOnOppositeColor = std::popcount(otherOppositeBishopColorPawns);

                evaluation += multiplier * std::popcount(otherPawnsOnOppositeColor) * BishopPawns[Color::OTHER_COLOR];

            } break;
            case PieceType::ROOK: {
                mobilityDstSquares = RookMagic(src, board.allPieces);

                const Bitboard colorRooks = colorPieces[PieceType::ROOK];

                if ((colorRooks & mobilityDstSquares) != EmptyBitboard) {
                    evaluation += multiplier * DoubledRooks;
                }

                const File file = GetFile(src);
                const Bitboard piecesInSameFile = board.allPieces & FileBitboard[file];

                if (piecesInSameFile == OneShiftedBy(src)) {
                    evaluation += multiplier * EmptyFileRook;
                }
            } break;
            case PieceType::QUEEN:
                mobilityDstSquares = QueenMagic(src, board.allPieces);

                break;
            case PieceType::KING: {
                //mobilityDstSquares = QueenMagic(src, board.allPieces);

                const File file = GetFile(src);
                const Rank rank = GetRank(evaluatedSrc);

                if (rank < Rank::_3) {
                    const Bitboard shield = KingPawnShield[file];

                    Bitboard evaluatedPawns = colorIsWhite ? colorPieces[PieceType::PAWN] : FlipBitboardOnVertical(colorPieces[PieceType::PAWN]);
                    evaluatedPawns <<= 8 * (Rank::_1 - rank + 1);

                    evaluation += multiplier * std::popcount(evaluatedPawns & shield) * KingShield[0];
                    evaluation += multiplier * std::popcount((evaluatedPawns + Direction::DOWN) & shield) * KingShield[1];
                }

            }   break;
            default:
                assert(0);
            }

            const std::uint32_t mobility = std::popcount(mobilityDstSquares & ~allColorPieces & ~UnsafeSquares[color]);
            evaluation += multiplier * MobilityParameters[pieceType][mobility];

            if (pieceType != PieceType::KING) {
                const Bitboard kingAttacks = mobilityDstSquares & otherKingMoves & ~UnsafeSquares[color];

                const std::uint32_t kingAttackCount = std::popcount(kingAttacks);
                evaluation += multiplier * KingAttacks[pieceType] * kingAttackCount;

                evaluation += multiplier * this->evaluateAttacks(board, color, pieceType, mobilityDstSquares);
                evaluation += multiplier * this->evaluateTropism(pieceType, src, otherKingPosition);
            }
        }
    }

    if (enableEvaluationHashtable) {
        evaluationHashtable.insert(board.hashValue, evaluation.mg, evaluation.eg);
    }

    //7) Begin Result Calculation
    const Score result = (isWhiteToMove ? evaluation(phase) : -evaluation(phase)) + Tempo(phase);

    return result;
}

ChessEvaluation ChessEvaluator::evaluateTropism(PieceType pieceType, Square src, Square otherKingPosition) const
{
    const std::uint32_t tropism = Distance[FileDistance(otherKingPosition, src)][RankDistance(otherKingPosition, src)];

    return TropismParameters[pieceType][tropism];
}

Score ChessEvaluator::lazyEvaluateImplementation(const BoardType& board)
{
    const EvaluationType evaluation = board.materialEvaluation + board.pstEvaluation + Tempo;

    const std::int32_t phase = board.getPhase();
    const Score result = evaluation(phase);

    const bool isWhiteToMove = board.isWhiteToMove();
    return isWhiteToMove ? result : -result;
}

void ChessEvaluator::prefetch(Hash hashValue) const
{
    if (enableEvaluationHashtable) {
        evaluationHashtable.prefetch(hashValue);
    }
}
