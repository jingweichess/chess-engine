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
#include <cstdint>

#include "../board/board.h"

#include "../clock/clock.h"

#include "../eval/evaluator.h"

#include "../types/depth.h"
#include "../types/nodecount.h"
#include "../types/score.h"

#include "events/searcheventhandler.h"

#include "movehistory.h"

#include "hashtable.h"

//constexpr bool enableAllSearchFeatures = true;
//
//constexpr bool enableAspirationWindow = enableAllSearchFeatures && false;
//constexpr bool enableReverseFutilityPruning = enableAllSearchFeatures && false;
//constexpr bool enableHistoryTable = enableAllSearchFeatures && true;
//constexpr bool enableKillerMoves = enableAllSearchFeatures && true;
//constexpr bool enableMoveExtensions = enableAllSearchFeatures && false;
//constexpr bool enableNullMove = enableAllSearchFeatures && false;
//constexpr bool enablePositionExtensions = enableAllSearchFeatures && false;
//constexpr bool enableRazoring = enableAllSearchFeatures && false;
//constexpr bool enableSearchHashtable = enableAllSearchFeatures && false;

constexpr std::uint32_t SearchStackSize = Depth::MAX + 2;

constexpr std::uint32_t HASH_MEGABYTES = 1;
constexpr std::uint32_t HASH_SIZE = HASH_MEGABYTES * 65536;

template <class T, class Evaluator, class MoveGenerator, class PrincipalVariation, class SearchStack, class BoardMover, class MoveOrderer, class HistoryTable, class StaticExchangeEvaluator>
class PrincipalVariationSearcher
{
public:
    using AttackGeneratorType = MoveGenerator::AttackGeneratorType;
    using BoardType = typename Evaluator::BoardType;
    using BoardMoverType = BoardMover;
    using EvaluationType = typename Evaluator::EvaluationType;
    using EvaluatorType = Evaluator;
    using HistoryTableType = HistoryTable;
    using MoveGeneratorType = MoveGenerator;
    using MoveOrdererType = MoveOrderer;
    using MoveType = typename BoardType::MoveType;
    using MoveListType = MoveList<MoveType>;
    using PrincipalVariationType = PrincipalVariation;
    using SearchStackType = SearchStack;
    using StaticExchangeEvaluatorType = StaticExchangeEvaluator;

    using EventHandler = SearchEventHandler<BoardType, PrincipalVariationType>;
    using EventHandlerSharedPtr = SearchEventHandlerSharedPtr<BoardType, PrincipalVariationType>;

protected:
    Clock clock;

    AttackGeneratorType attackGenerator;
    BoardMoverType boardMover;
    EvaluatorType evaluator;
    MoveGeneratorType moveGenerator;
    MoveOrderer moveOrderer;
    StaticExchangeEvaluatorType staticExchangeEvaluator;

    Hashtable hashtable;

    HistoryTableType historyTable;
    //HistoryTableType mateHistoryTable;

    MoveHistory<BoardType, MoveType> moveHistory;

    MoveListType rootMoveList;

    std::array<SearchStackType, SearchStackSize> searchStack;

    NodeCount nodeCount;

    bool abortedSearch;
    Depth rootSearchDepth;

    SearchEventHandlerList<BoardType, PrincipalVariationType> searchEventHandlerList;

    bool checkHashtable(const BoardType& board, HashtableEntry& hashtableEntry) const
    {
        return this->hashtable.search(hashtableEntry, board.hashValue);
    }

    template <NodeType nodeType>
    Score quiescenceSearch(BoardType& board, SearchStackType* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
    {
        assert(alpha >= -INFINITE_SCORE && beta <= INFINITE_SCORE);

        PrincipalVariationType& principalVariation = searchStack->principalVariation;

        //1) Check for instant abort conditions
        if (currentDepth >= (Depth::MAX - Depth::ONE)
            || !this->clock.shouldContinueSearch(Depth::ZERO, this->getNodeCount())) {
            this->abortedSearch = true;

            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            return NO_SCORE;
        }

        PrincipalVariationType& nextPrincipalVariation = (searchStack + 1)->principalVariation;

        if (nodeType == NodeType::PV
            && principalVariation.size() > 0) {
            principalVariation.copyForward(nextPrincipalVariation);

            searchStack->pvMove = principalVariation[0];
            principalVariation.clear();
        }

        this->nodeCount++;
        //this->quiescentNodeCount++;

        const Depth depthLeft = maxDepth - currentDepth;

        //4) Evaluate board statically, for a stand-pat option
        const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

        if (isInCheck) {
            searchStack->staticEvaluation = LostInDepth(currentDepth);
        }
        else {
            searchStack->staticEvaluation = nodeType == NodeType::PV
                ? this->evaluator.evaluate(board, currentDepth, -INFINITE_SCORE, INFINITE_SCORE)
                : this->evaluator.evaluate(board, currentDepth, alpha, beta);

            if (searchStack->staticEvaluation >= beta) {
                return searchStack->staticEvaluation;
            }

            alpha = std::max(alpha, searchStack->staticEvaluation);
        }

        //5) Generate Moves.  Return if Checkmate.
        MoveListType& moveList = searchStack->moveList;
        const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllCaptures(board, moveList);

        if (moveCount == ZeroNodes
            && isInCheck) {
            return LostInDepth(currentDepth);
        }

        //6) Reorder Moves
        if (isInCheck) {
            this->moveOrderer.reorderMoves(board, moveList, searchStack, this->historyTable);
        }
        else {
            this->moveOrderer.reorderQuiescenceMoves(board, moveList, searchStack);
        }

        //7) MoveList loop
        Score bestScore = searchStack->staticEvaluation;
        NodeCount movesSearched = ZeroNodes;

        for (MoveType& move : moveList) {
            const Square& src = move.src;
            const Square& dst = move.dst;

            const PieceType capturedPiece = board.pieces[dst];

            //10) DoMove
            BoardType nextBoard = board;
            this->boardMover.dispatchDoMove(nextBoard, move);

            searchStack->currentMove = move;

            //11) Recurse to next depth
            Score score;
            switch (nodeType) {
            case NodeType::PV:
                if (movesSearched == ZeroNodes) {
                    score = -this->quiescenceSearch<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth, currentDepth + Depth::ONE);
                }
                else {
                    score = -this->quiescenceSearch<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

                    if (score > alpha
                        && score < beta) {
                        score = -this->quiescenceSearch<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth, currentDepth + Depth::ONE);
                    }
                }

                break;
            case NodeType::CUT:
                score = -this->quiescenceSearch<NodeType::ALL>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

                break;
            case NodeType::ALL:
                score = -this->quiescenceSearch<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

                break;
            default:
                assert(0);
            }

            //12) Undo Move
            movesSearched++;
            move.ordinal = score;

            //13) Compare returned value to alpha/beta
            if (score > bestScore) {
                bestScore = score;
                searchStack->bestMove = move;
            }

            if (score > alpha) {
                if (score >= beta) {
                    return score;
                }

                assert(nodeType == NodeType::PV);

                principalVariation.copyBackward(nextPrincipalVariation, move);

                alpha = score;
            }
        }

        return bestScore;
    }

    Score rootSearch(const BoardType& board, PrincipalVariationType& principalVariation, Score alpha, Score beta, Depth maxDepth)
    {
        Score bestScore = -INFINITE_SCORE;
        this->rootSearchDepth = maxDepth;

        SearchStackType* searchStack = &this->searchStack[0];
        PrincipalVariationType& nextPrincipalVariation = (searchStack + 1)->principalVariation;

        NodeCount movesSearched = ZeroNodes;

        for (MoveType& move : this->rootMoveList) {
            BoardType nextBoard = board;
            this->boardMover.dispatchDoMove(nextBoard, move);

            this->moveHistory.push_back(nextBoard, move);

            Score score;

            if (movesSearched == 0) {
                principalVariation.copyForward(nextPrincipalVariation);

                score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth, Depth::ONE);
            }
            else {
                score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -(alpha + 1), -alpha, maxDepth, Depth::ONE);

                if (score > alpha
                    && score < beta) {
                    nextPrincipalVariation.clear();
                    score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth, Depth::ONE);
                }
            }

            movesSearched++;

            this->moveHistory.pop_back();

            if (this->abortedSearch) {
                break;
            }

            move.ordinal = score;

            if (score > bestScore) {
                bestScore = score;
            }

            if (score > alpha) {
                if (score >= beta) {
                    std::stable_sort(this->rootMoveList.begin(), this->rootMoveList.end(), std::greater<MoveType>());

                    return score;
                }

                alpha = score;

                principalVariation.copyBackward(nextPrincipalVariation, move);

                const NodeCount nodeCount = this->getNodeCount();
                const std::time_t time = this->clock.getElapsedTime(nodeCount);

                this->searchEventHandlerList.onLineCompleted(principalVariation, time, nodeCount, score, maxDepth);

                this->verifyPrincipalVariation(board, principalVariation, score, maxDepth);
            }
        }

        std::stable_sort(this->rootMoveList.begin(), this->rootMoveList.end(), std::greater<MoveType>());

        this->verifyPrincipalVariation(board, principalVariation, bestScore, maxDepth);

        return bestScore;
    }

    bool saveToHashtable(const BoardType& board, const MoveType& move, Score alpha, Score beta, Score score, Depth currentDepth, Depth depthLeft)
    {
        HashtableEntryType hashtableEntryType = HashtableEntryType::NONE;

        if (score >= beta) {
            hashtableEntryType = HashtableEntryType::LOWER_BOUND;
        }
        else if (score <= alpha) {
            hashtableEntryType = HashtableEntryType::UPPER_BOUND;
        }
        else if (score > alpha
            && score < beta) {
            hashtableEntryType = HashtableEntryType::EXACT_VALUE;
        }

        if (hashtableEntryType == HashtableEntryType::NONE) {
            assert(0);
            return false;
        }

        this->hashtable.insert(board.hashValue, score, currentDepth, depthLeft, hashtableEntryType);

        return true;
    }

    template <NodeType nodeType>
    Score search(BoardType& board, SearchStackType* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
    {
        PrincipalVariationType& principalVariation = searchStack->principalVariation;

        //1) Check for instant abort conditions
        if (currentDepth >= (Depth::MAX - Depth::ONE)
            || !this->clock.shouldContinueSearch(Depth::ZERO, this->getNodeCount())) {
            this->abortedSearch = true;

            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            return NO_SCORE;
        }

        //2) Check for Draw condition
        const TwoPlayerGameResult gameResult = this->checkBoardGameResult(board, false, nodeType == NodeType::PV);
        if (gameResult == TwoPlayerGameResult::DRAW) {
            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            return DRAW_SCORE;
            //return DRAW_SCORE + ((this->getNodeCount() & 2) == 0 ? 1 : -1) * (this->getNodeCount() & 1);
        }

        //3) Mate Distance Pruning
        alpha = std::max(LostInDepth(currentDepth - Depth::ONE), alpha);
        beta = std::min(WinInDepth(currentDepth), beta);

        if (alpha >= beta) {
            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            return alpha;
        }

        if (nodeType == NodeType::PV) {
            searchStack->distanceFromPv = Depth::ZERO;
        }
        else {
            searchStack->distanceFromPv = (searchStack - 1)->distanceFromPv + Depth::ONE;
        }

        //4) Quiescence Search
        const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

        const Depth depthLeft = maxDepth - currentDepth;
        if (depthLeft <= Depth::ZERO) {
            //assert(!isInCheck);

            return this->quiescenceSearch<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);
        }

        assert(depthLeft > Depth::ZERO);

        this->nodeCount++;

        //5) Check Hashtable
        searchStack->hashDepth = Depth::ZERO;
        searchStack->hashFound = false;

        Score hashScore = NO_SCORE;

        if (enableSearchHashtable) {
            HashtableEntry hashtableEntry;
            searchStack->hashFound = this->checkHashtable(board, hashtableEntry);

            if (searchStack->hashFound) {
                const HashtableEntryType hashtableEntryType = hashtableEntry.getType();
                searchStack->hashDepth = hashtableEntry.getDepthLeft();
                hashScore = hashtableEntry.getScore(currentDepth);

                //if (nodeType != NodeType::PV
                //    && IsMateScore(hashScore)) {
                //    return hashScore;
                //}

                if (nodeType != NodeType::PV
                    //&& !IsMateScore(hashScore)
                    && searchStack->hashDepth >= depthLeft) {

                    switch (hashtableEntryType) {
                    case HashtableEntryType::NONE:
                        assert(0);
                        break;
                    case HashtableEntryType::EXACT_VALUE:
                        searchStack->bestMove = NullMove;
                        return hashScore;
                    case HashtableEntryType::LOWER_BOUND:
                        if (hashScore >= beta) {
                            searchStack->bestMove = NullMove;
                            return hashScore;
                        }

                        break;
                    case HashtableEntryType::UPPER_BOUND:
                        if (hashScore < alpha) {
                            searchStack->bestMove = NullMove;
                            return hashScore;
                        }

                        break;
                    }
                }

                if constexpr (nodeType != NodeType::PV) {
                    //ChessBoard nextBoard = board;
                    //this->boardMover.dispatchDoMove(nextBoard, hashMove);

                    //constexpr NodeType newNodeType = nodeType == NodeType::ALL ? NodeType::CUT : NodeType::ALL;
                    //const Score score = -this->search<newNodeType>(nextBoard, principalVariation, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

                    //if (score >= beta) {
                    //    this->saveToHashtable(board, hashMove, alpha, beta, score, currentDepth, depthLeft);

                    //    return score;
                    //}
                }
            }
        }

        //4) Evaluate Position
        if (isInCheck) {
            searchStack->staticEvaluation = LostInDepth(currentDepth);
            searchStack->passedPawns = this->evaluator.calculatePassedPawns(board, board.sideToMove);
        }
        else if (enableSearchHashtable) {
            if (searchStack->hashFound) {
                searchStack->staticEvaluation = hashScore;
                searchStack->passedPawns = this->evaluator.calculatePassedPawns(board, board.sideToMove);
            }
            else {
                searchStack->staticEvaluation = this->evaluator.evaluate(board, currentDepth, alpha, beta);
                searchStack->passedPawns = this->evaluator.getPassedPawnsForColor(board.sideToMove);
            }
        }
        else {
            searchStack->staticEvaluation = this->evaluator.evaluate(board, currentDepth, alpha, beta);
            searchStack->passedPawns = this->evaluator.getPassedPawnsForColor(board.sideToMove);
        }

#ifdef _DEBUG
        const Bitboard calculatedPassedPawns = this->evaluator.calculatePassedPawns(board, board.sideToMove);
        assert(searchStack->passedPawns == calculatedPassedPawns);
#endif

        const bool hasNonPawnMaterial = board.hasNonPawnMaterial();
        const std::uint32_t phase = board.getPhase();

        if (nodeType != NodeType::PV
            && !searchStack->hashFound
            && !isInCheck
            && !IsMateScore(alpha)
            && !IsDrawScore(alpha)
            && hasNonPawnMaterial) {

            //7) Reverse Futility Pruning
            if (enableReverseFutilityPruning
                && depthLeft < Depth::FOUR
                && searchStack->staticEvaluation < BASICALLY_WINNING_SCORE
                && searchStack->staticEvaluation >= beta + FutilityMargin(depthLeft, phase)) {
                return searchStack->staticEvaluation;
            }

            //8) Razoring
            if (enableRazoring
                && depthLeft < Depth::FOUR
                && searchStack->staticEvaluation < alpha - RazorMargin(depthLeft, phase)) {
                const Score razorScore = this->quiescenceSearch<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);

                if (razorScore < alpha) {
                    return razorScore;
                }
            }

            //9) Null Move Pruning
            if (enableNullMove
                && (searchStack - 1)->currentMove != NullMove
                && (searchStack)->staticEvaluation >= beta + NullMoveMargin(depthLeft, phase)
                && (searchStack)->staticEvaluation < BASICALLY_WINNING_SCORE
                && phase > 9
                /*&& depthLeft >= Depth::ONE*/) {

                ChessBoard newBoard = board;
                this->boardMover.doNullMove(newBoard);

                searchStack->currentMove = NullMove;

                const Depth nullReduction = NullMoveReduction(depthLeft, phase);
                const Score nullScore = -this->search<NodeType::ALL>(newBoard, searchStack + 1, -beta, -alpha, maxDepth - nullReduction, currentDepth + Depth::ONE);

                bool isMateThreat = IsMateScore(nullScore);

                if (!isMateThreat
                    && nullScore >= beta) {
                    return nullScore;

                    //const Depth nullVerificationReduction = NullMoveVerificationReduction(depthLeft, phase);
                    //const Score verifiedNullScore = this->search<nodeType>(board, searchStack, alpha, beta, maxDepth - nullVerificationReduction, currentDepth);

                    //isMateThreat = IsMateScore(verifiedNullScore);
                    //if (!isMateThreat
                    //    && verifiedNullScore >= beta) {

                    //    return verifiedNullScore;
                    //}
                }
            }
        }

        //5) Generate Moves.  Return if Checkmate or Stalemate.
        MoveListType& moveList = searchStack->moveList;
        searchStack->moveCount = this->moveGenerator.DispatchGenerateAllMoves(board, moveList);

        if (searchStack->moveCount == ZeroNodes) {
            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            if (isInCheck) {
                return LostInDepth(currentDepth);
            }
            else {
                return DRAW_SCORE;
            }
        }

        //6) Search Loop
        const Score score = this->searchLoop<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);

        //13) Save to Hashtable
        if (enableSearchHashtable
            && !this->abortedSearch
            ) {
            this->saveToHashtable(board, searchStack->bestMove, alpha, beta, score, currentDepth, depthLeft);
        }

        return score;
    }

    template <NodeType nodeType, bool iidSearch = false, bool capturesOnly = false>
    Score searchLoop(BoardType& board, SearchStackType* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
    {
        constexpr bool isPvNode = nodeType == NodeType::PV;

        MoveListType& moveList = searchStack->moveList;
        PrincipalVariationType& principalVariation = searchStack->principalVariation;

        if (nodeType == NodeType::PV
            && principalVariation.size() > 0) {
            PrincipalVariationType& nextPrincipalVariation = (searchStack + 1)->principalVariation;
            principalVariation.copyForward(nextPrincipalVariation);

            searchStack->pvMove = principalVariation[0];
            principalVariation.clear();
        }

        //TODO: IID
        this->moveOrderer.reorderMoves(board, moveList, searchStack, this->historyTable);

        //2) Calculate Position Extensions (independent of type of move)
        Depth positionExtensions = Depth::ZERO;
        const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

        if (enablePositionExtensions) {
            if (moveList.size() == 1) {
                positionExtensions += Depth::ONE;
            }

            if (isInCheck) {
                positionExtensions += Depth::ONE;
            }

            //if (currentDepth >= Depth::TWO) {
            //    //***Don't use this reference if currentDepth < 2***
            //    const ChessSearchStack* ourLastSearchStack = searchStack - 2;

            //    if (!isInCheck
            //        && searchStack->staticEvaluation >= BASICALLY_WINNING_SCORE
            //        && ourLastSearchStack->staticEvaluation < BASICALLY_WINNING_SCORE) {
            //        positionExtensions += Depth::ONE;
            //    }
            //}
        }

        const Depth depthLeft = maxDepth - currentDepth;

        Score bestScore = -INFINITE_SCORE;
        NodeCount movesSearched = ZeroNodes;

        for (MoveType& move : moveList) {
            const Square& src = move.src;
            const Square& dst = move.dst;

            const PieceType movingPiece = board.pieces[src];
            const PieceType capturedPiece = board.pieces[dst];

            const PieceType promotionPiece = move.promotionPiece;

            const bool isQuietMove = capturedPiece == PieceType::NO_PIECE
                && promotionPiece == PieceType::NO_PIECE;

            assert(movingPiece != PieceType::NO_PIECE);

            //3) DoMove
            BoardType nextBoard = board;
            this->boardMover.dispatchDoMove(nextBoard, move);

            //5) Calculate Move Extensions
            //const bool givesCheck = this->attackGenerator.dispatchIsInCheck(nextBoard);

            const ChessMove lastOpponentMove = (searchStack - 1)->currentMove;
            const bool isRecapture = lastOpponentMove.capturedPiece != PieceType::NO_PIECE
                && lastOpponentMove.dst == move.dst;

            Depth extensions = positionExtensions;

            if (enableMoveExtensions
                /*&& currentDepth < this->rootSearchDepth*/) {
                //Extend Castle Moves
                if (movingPiece == PieceType::KING
                    && (PieceMoves[PieceType::KING][src] & OneShiftedBy(dst)) == EmptyBitboard) {
                    extensions += Depth::ONE;
                }

                //Extend a Recapture?
                if (isRecapture
                    && depthLeft < Depth::FOUR) {
                    extensions += Depth::ONE;
                }

                //Extend Promotions?
                //if (promotionPiece == PieceType::QUEEN) {
                //    extensions += Depth::ONE;
                //}

                move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

                //Don't extend these types of moves when the See score < 0
                if (move.seeScore >= ZERO_SCORE) {
                    //Extend moves that give check with a positive See Score
                    //if (move.seeScore > ZERO_SCORE
                    //    && givesCheck) {
                    //    extensions += Depth::ONE;
                    //}

                    //Extend Passed Pawns!
                    const bool isWhiteToMove = board.isWhiteToMove();

                    const Rank SixthRank = isWhiteToMove ? Rank::_6 : Rank::_3;
                    const Rank FifthRank = isWhiteToMove ? Rank::_5 : Rank::_4;

                    const Rank srcRank = GetRank(src);

                    if ((searchStack->passedPawns & OneShiftedBy(src)) != EmptyBitboard
                        && (srcRank == SixthRank
                            || srcRank == FifthRank)) {
                        assert(movingPiece == PieceType::PAWN);

                        extensions += Depth::ONE;
                    }

                    //Extend Pieces which threaten the Queen?
                    if (movingPiece == PieceType::BISHOP) {
                        const Bitboard otherQueens = isWhiteToMove ? nextBoard.blackPieces[PieceType::QUEEN] : nextBoard.whitePieces[PieceType::QUEEN];
                        const Bitboard dstMoves = BishopMagic(dst, nextBoard.allPieces);

                        for (const Square queenSrc : SquareBitboardIterator(otherQueens& dstMoves)) {
                            extensions += Depth::ONE;
                        }
                    }

                    //Extend if See captures a Minor Piece or more?
                    //if (move.seeScore >= ROOK_SCORE) {
                    //    extensions += Depth::ONE;
                    //}
                }
                else {  //if (move.seeScore < ZERO_SCORE)
                    //Reduce if See loses a Pawn or more
                    if (nodeType != NodeType::PV
                        && move.seeScore <= -PAWN_SCORE) {
                        extensions -= Depth::TWO;
                    }
                }
            }

            //7) Recurse
            searchStack->currentMove = move;
            this->moveHistory.push_back(nextBoard, move);

            Score score = ZERO_SCORE;
            (searchStack + 1)->excludedMove = NullMove;

            switch (nodeType) {
            case NodeType::PV:
                if (movesSearched == ZeroNodes) {
                    score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
                }
                else {
                    score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

                    if (score > alpha
                        && score < beta) {
                        score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
                    }
                }
                break;
            case NodeType::CUT:
                score = -this->search<NodeType::ALL>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

                if (score > alpha) {
                    score = -this->search<NodeType::ALL>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
                }

                break;
            case NodeType::ALL:
                score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

                if (score > alpha) {
                    score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
                }

                break;
            default:
                assert(0);
            }

            //8) Undo Move
            this->moveHistory.pop_back();

            move.ordinal = score;

            //9) Check if the new score is a best score
            if (score > bestScore) {
                bestScore = score;
                searchStack->bestMove = move;
            }

            //10a) Check if the new score is > alpha
            if (score > alpha) {
                //11) Check if the new score > beta.  Save some statistics and return.
                if (score >= beta) {
                    if (enableHistoryTable) {
                        if (isQuietMove) {
                            const std::uint32_t phase = nextBoard.getPhase();

                            const std::uint32_t delta = HistoryDelta(depthLeft, phase);
                            this->historyTable.add(movingPiece, dst, delta);
                        }
                    }

                    if (enableKillerMoves
                        && isQuietMove
                        && searchStack->killer1 != move) {
                        searchStack->killer2 = searchStack->killer1;
                        searchStack->killer1 = move;
                    }

                    return score;
                }

                //10b) Save Principal Variation
                assert(nodeType == NodeType::PV);

                const PrincipalVariationType& nextPrincipalVariation = (searchStack + 1)->principalVariation;
                principalVariation.copyBackward(nextPrincipalVariation, move);

                searchStack->pvMove = move;

                //10c) Update new alpha
                alpha = score;
            }

            movesSearched++;
        }

        assert(movesSearched > 0);

        //11) Return score.
        return bestScore;
    }

    void verifyPrincipalVariation(const BoardType& board, PrincipalVariationType& principalVariation, Score score, Depth depth)
    {
        assert(principalVariation.size() > 0);
        assert(score == principalVariation[0].ordinal);

        if (score != principalVariation[0].ordinal) {
            std::printf("assert(score == principalVariation[0].ordinal)\n");
            //savePosition(board, principalVariation, score, depth);
            return;
        }

        BoardType pvBoard = board;
        for (MoveType& move : principalVariation) {
            this->boardMover.dispatchDoMove(pvBoard, move);

            this->moveHistory.push_back(pvBoard, move);
        }

        if (IsDrawScore(score)) {
            const TwoPlayerGameResult gameResult = this->checkBoardGameResult(pvBoard, true, true);

            if (gameResult != TwoPlayerGameResult::DRAW) {
                const Depth pvDepth = static_cast<Depth>(principalVariation.size());
                const Score evaluatedScore = this->evaluator.evaluate(pvBoard, pvDepth, -INFINITE_SCORE, INFINITE_SCORE);

                assert(evaluatedScore == DRAW_SCORE);

                if (evaluatedScore != DRAW_SCORE) {
                    std::printf("assert(evaluatedScore == DRAW_SCORE)\n");
                    //savePosition(board, principalVariation, score, depth);

                    goto verifyend;
                }
            }
        }
        else if (IsMateScore(score)) {
            const Depth distanceToMate = DistanceToWin(score);

            assert(distanceToMate == principalVariation.size());

            if (distanceToMate != principalVariation.size()) {
                std::printf("assert(distanceToMate == principalVariation.size())\n");
                //savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }

            const bool isInCheck = this->attackGenerator.dispatchIsInCheck(pvBoard);

            assert(isInCheck);

            if (!isInCheck) {
                std::printf("assert(isInCheck)\n");
                //savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }

            MoveListType moveList;
            const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllMoves(pvBoard, moveList);

            assert(moveCount == ZeroNodes);

            if (moveCount != ZeroNodes) {
                std::printf("assert(moveCount == ZeroNodes)\n");
                //savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }
        }
        else {
            assert(enableAspirationWindow || principalVariation.size() >= depth);

            if (!enableAspirationWindow
                && principalVariation.size() < depth) {
                std::printf("assert(principalVariation.size() >= depth)\n");
                //savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }

            const Depth pvDepth = static_cast<Depth>(principalVariation.size());
            Score evaluatedScore = this->evaluator.evaluate(pvBoard, pvDepth, -INFINITE_SCORE, INFINITE_SCORE);

            if (pvBoard.sideToMove != board.sideToMove) {
                evaluatedScore = -evaluatedScore;
            }

            assert(enableAspirationWindow || evaluatedScore == score);

            if (!enableAspirationWindow
                && evaluatedScore != score) {
                std::printf("assert(evaluatedScore == score)\n");
                //savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }
        }

    verifyend:
        for (std::uint32_t i = 0; i < principalVariation.size(); i++) {
            this->moveHistory.pop_back();
        }
    }

public:
    constexpr PrincipalVariationSearcher()
    {
        if constexpr (enableSearchHashtable) {
            this->hashtable.initialize(HASH_SIZE);
        }
    }

    constexpr ~PrincipalVariationSearcher() = default;

    constexpr void addMoveToHistory(BoardType& board, MoveType& move, bool irreversible = false)
    {
        this->moveHistory.push_back(board, move, irreversible);
    }

    constexpr void addSearchEventHandler(EventHandlerSharedPtr& searchEventHandler)
    {
        this->searchEventHandlerList.push_back(searchEventHandler);
    }

    constexpr TwoPlayerGameResult checkBoardGameResult(const BoardType& board, bool checkMoveCount, bool isPrincipalVariation) const
    {
        //Check for checkmate or stalemate
        //Cannot optimize this by doing it last; technically if a mate exists at the end of a 50 move count rule, the mate stands
        if (checkMoveCount
            || board.fiftyMoveCount >= 100) {
            MoveListType moveList;
            const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllMoves(board, moveList);

            if (moveCount == ZeroNodes) {
                //const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

                //if (isInCheck) {
                return TwoPlayerGameResult::LOSS;
                //}
                //else {
                //    return TwoPlayerGameResult::DRAW;
                //}
            }
        }

        //50 Move Count Draw detection
        if (board.fiftyMoveCount >= 100) {
            return TwoPlayerGameResult::DRAW;
        }

        //Repetition Draw detection
        const std::uint32_t repetitionCount = this->moveHistory.checkForDuplicateHash(board.hashValue);
        if (repetitionCount > static_cast<std::uint32_t>(isPrincipalVariation ? 2 : 1)) {
            return TwoPlayerGameResult::DRAW;
        }

        //Check for InsufficientMaterial
        if (this->evaluator.checkBoardForInsufficientMaterial(board)) {
            return TwoPlayerGameResult::DRAW;
        }

        return TwoPlayerGameResult::NO_GAMERESULT;
    }

    constexpr NodeCount getNodeCount() const
    {
        return this->nodeCount;
    }

    constexpr void initializeSearch(const BoardType& board)
    {
        this->clock.startClock();

        this->abortedSearch = false;
        this->nodeCount = ZeroNodes;

        this->moveGenerator.DispatchGenerateAllMoves(board, this->rootMoveList);

        static_cast<T*>(this)->initializeSearchImplementation(board);
    }

    void iterativeDeepeningLoop(const BoardType& board, PrincipalVariationType& principalVariation)
    {
        constexpr Score alpha = -INFINITE_SCORE, beta = INFINITE_SCORE;

        PrincipalVariationType localPrincipalVariation;

        Depth searchDepth = Depth::TWO;

        bool isSearching = true;
        bool foundMateSolution = false;

        this->initializeSearch(board);

        while (isSearching) {
            Score score = this->rootSearch(board, localPrincipalVariation, alpha, beta, searchDepth);

            if (this->wasSearchAborted()) {
                break;
            }

            foundMateSolution = IsMateScore(score);

            principalVariation = localPrincipalVariation;

            const NodeCount nodeCount = this->getNodeCount();
            const std::time_t time = this->clock.getElapsedTime(nodeCount);

            this->searchEventHandlerList.onDepthCompleted(principalVariation, time, nodeCount, score, searchDepth);

            if (foundMateSolution) {
                const Depth distanceToMate = DistanceToWin(score);
                if (searchDepth > distanceToMate * 3) {
                    isSearching = false;
                }
            }

            isSearching = isSearching && this->clock.shouldContinueSearch(searchDepth, this->getNodeCount());

            searchDepth++;
        }

        this->searchEventHandlerList.onSearchCompleted(board);
    }

    constexpr void resetHashtable()
    {
        //this->hashtable.reset();
    }

    constexpr void resetMoveHistory()
    {
        this->moveHistory.clear();
    }

    constexpr void setClock(const Clock& clock)
    {
        this->clock = clock;
    }

    constexpr bool wasSearchAborted() const
    {
        return this->abortedSearch;
    }
};
