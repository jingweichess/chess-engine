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

#include <algorithm>
#include <fstream>
#include <iomanip>

#include <cassert>

#include "searcher.h"

#include "history.h"
#include "futilty.h"
#include "nullmove.h"
#include "quiescence.h"
#include "probcut.h"
#include "razoring.h"
#include "reductions.h"

#include "../board/boardmover.h"
#include "../board/moveorderer.h"

constexpr std::uint32_t HASH_MEGABYTES = 1;
constexpr std::uint32_t HASH_SIZE = HASH_MEGABYTES * 65536;

//static const std::string FilePrint = "abcdefgh";
//static const std::string PiecePrint = ".pnbrqk";
//static const std::string RankPrint = "87654321";

static void savePosition(const ChessBoard& board, ChessPrincipalVariation& principalVariation, Score score, Depth depth)
{
    std::ofstream positionFile;

    positionFile.open("data/saved-positions.txt", std::ofstream::out | std::ofstream::app);
    positionFile << "#expect ";
    
    for (const ChessMove& move : principalVariation) {
        Square src = move.src;
        Square dst = move.dst;

        File srcFile = GetFile(src);
        File dstFile = GetFile(dst);
        Rank srcRank = GetRank(src);
        Rank dstRank = GetRank(dst);

        PieceType promotionPiece = move.promotionPiece;

        const char* file = FilePrintLowerCase.c_str();
        const char* rank = RankPrint.c_str();

        positionFile << file[srcFile] << rank[srcRank] << file[dstFile] << rank[dstRank];

        if (promotionPiece != PieceType::NO_PIECE) {
            const char* print = PiecePrint.c_str();

            positionFile << print[promotionPiece];
        }

        positionFile << ' ';
    }

    positionFile << score << ' ' << depth << std::endl;

    const std::string boardFen = board.saveToFen();
    positionFile << "setboard " << boardFen << std::endl << "go" << std::endl;
}

ChessSearcher::ChessSearcher()
{
    if constexpr (enableSearchHashtable) {
        this->hashtable.initialize(HASH_SIZE);
    }

    this->moveHistory.reserve(1024);
}

void ChessSearcher::addMoveToHistory(ChessBoard& board, ChessMove& move)
{
    this->moveHistory.addMoveToHistory(board, move);
}

TwoPlayerGameResult ChessSearcher::checkBoardGameResult(const ChessBoard& board, bool checkMoveCount, bool isPrincipalVariation) const
{
    //Check for checkmate or stalemate
    //Cannot optimize this by doing it last; technically if a mate exists at the end of a 50 move count rule, the mate stands
    if (checkMoveCount
        || board.fiftyMoveCount >= 100) {
        MoveList<ChessMove> moveList;
        const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllMoves(board, moveList);

        if (moveCount == ZeroNodes) {
            const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

            if (isInCheck) {
                return TwoPlayerGameResult::LOSS;
            }
            else {
                return TwoPlayerGameResult::DRAW;
            }
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

bool ChessSearcher::checkHashtable(const ChessBoard& board, HashtableEntry& hashtableEntry) const
{
    return this->hashtable.search(hashtableEntry, board.hashValue);
}

NodeCount ChessSearcher::getNodeCount()
{
    return this->nodeCount + this->quiescentNodeCount;
}

void ChessSearcher::initialize()
{
    if (enableHistoryTable) {
        this->historyTable.reset();
    }

    this->abortedSearch = false;

    this->nodeCount = ZeroNodes;
    this->quiescentNodeCount = ZeroNodes;

    this->hashtable.incrementAge();

    this->clock.startClock();
}

void ChessSearcher::iterativeDeepeningLoop(const ChessBoard& board, ChessPrincipalVariation& principalVariation)
{
    this->initialize();

    Score alpha = -INFINITE_SCORE, beta = INFINITE_SCORE;
    Score aspirationWindowDelta = NO_SCORE;

    Depth searchDepth = Depth::TWO;

    Score mateScore = ZERO_SCORE;
    Score previousScore = NO_SCORE;

    bool isSearching = true;
    bool foundMateSolution = false;

    this->searchStack[0].moveCount = this->moveGenerator.DispatchGenerateAllMoves(board, this->rootMoveList);

    ChessMoveOrderer moveOrderer;
    moveOrderer.reorderMoves<NodeType::PV>(board, this->rootMoveList, &this->searchStack[0], this->historyTable);

    while (isSearching) {
        if (enableAspirationWindow
            && !foundMateSolution
            && searchDepth >= Depth::THREE) {
            aspirationWindowDelta = Score(256);

            alpha = std::max(previousScore - aspirationWindowDelta, -INFINITE_SCORE);
            beta = std::min(previousScore + aspirationWindowDelta, INFINITE_SCORE);
        }
        else if (foundMateSolution) {
            if (previousScore > INFINITE_SCORE - Depth::MAX) {
                alpha = WinInMaxDepth();
                beta = INFINITE_SCORE;
            }
            else {
                alpha = -INFINITE_SCORE;
                beta = LostInMaxDepth();
            }
        }

        Score score = this->rootSearch(board, principalVariation, alpha, beta, searchDepth);

        foundMateSolution = IsMateScore(score);

        while (enableAspirationWindow
//            && !foundMateSolution
            && !this->wasSearchAborted()
            && (score <= alpha || score >= beta)) {
            if (foundMateSolution) {
                const Depth distanceToMate = DistanceToWin(score);
                mateScore = score;

                if (searchDepth > distanceToMate * 3) {
                    break;
                }
            }
            else if (foundMateSolution) {
                if (previousScore > INFINITE_SCORE - Depth::MAX) {
                    alpha = WinInMaxDepth();
                    beta = INFINITE_SCORE;
                }
                else {
                    alpha = -INFINITE_SCORE;
                    beta = LostInMaxDepth();
                }
            }
            else if (std::abs(score) >= BASICALLY_WINNING_SCORE) {
                if (previousScore >= BASICALLY_WINNING_SCORE) {
                    alpha = BASICALLY_WINNING_SCORE;
                    beta = WinInMaxDepth();
                }
                else if (previousScore <= BASICALLY_WINNING_SCORE) {
                    alpha = LostInMaxDepth();
                    beta = BASICALLY_WINNING_SCORE;
                }
            }
            else if (score <= alpha) {
                alpha = std::max(score - aspirationWindowDelta, -INFINITE_SCORE);
            }
            else if (score >= beta) {
                beta = std::min(score + aspirationWindowDelta, INFINITE_SCORE);
            }

            score = this->rootSearch(board, principalVariation, alpha, beta, searchDepth);

            if (this->wasSearchAborted()) {
                break;
            }

            foundMateSolution = IsMateScore(score);

            aspirationWindowDelta += Score(256);
        }

        if (this->wasSearchAborted()) {
            break;
        }

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

        previousScore = score;
        searchDepth++;
    }

    this->searchEventHandlerList.onSearchCompleted(board);
}

template <NodeType nodeType>
Score ChessSearcher::quiescenceSearch(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
{
    assert(alpha >= -INFINITE_SCORE && beta <= INFINITE_SCORE);

    ChessPrincipalVariation& principalVariation = searchStack->principalVariation;

    //1) Check for instant abort conditions
    if (currentDepth >= (Depth::MAX - Depth::ONE)
        || !this->clock.shouldContinueSearch(Depth::ZERO, this->getNodeCount())) {
        this->abortedSearch = true;

        if constexpr (nodeType == NodeType::PV) {
            principalVariation.clear();
        }

        return NO_SCORE;
    }

    ChessPrincipalVariation& nextPrincipalVariation = (searchStack + 1)->principalVariation;

    if (nodeType == NodeType::PV
        && principalVariation.size() > 0) {
        principalVariation.copyForward(nextPrincipalVariation);

        searchStack->pvMove = principalVariation[0];
        principalVariation.clear();
    }

    this->quiescentNodeCount++;

    //5) Check Hashtable
    const Depth depthLeft = maxDepth - currentDepth;

    bool hashFound = false;

    Depth hashDepthLeft = Depth::ZERO;
    Score hashScore = ZERO_SCORE;

    if (enableQuiescenceSearchHashtable) {
        HashtableEntry hashtableEntry;
        hashFound = this->checkHashtable(board, hashtableEntry);

        if (hashFound) {
            const HashtableEntryType hashtableEntryType = hashtableEntry.getType();
            hashDepthLeft = hashtableEntry.getDepthLeft();
            hashScore = hashtableEntry.getScore(currentDepth);

            if (nodeType != NodeType::PV
                //&& !IsMateScore(hashScore)
                && hashDepthLeft >= depthLeft) {

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

        //if (searchstack->staticEvaluation + QUEEN_SCORE < alpha) {
        //    return searchstack->staticEvaluation;
        //}

        alpha = std::max(alpha, searchStack->staticEvaluation);
    }

    //5) Generate Moves.  Return if Checkmate.
    ChessMoveList& moveList = searchStack->moveList;
    const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllCaptures(board, moveList);

    if (moveCount == ZeroNodes
        && isInCheck) {
        return LostInDepth(currentDepth);
    }

    if (isInCheck) {
        this->moveOrderer.reorderMoves<nodeType>(board, moveList, searchStack, this->historyTable);
    }
    else {
        this->moveOrderer.reorderQuiescenceMoves<nodeType>(board, moveList, searchStack);
    }

    //7) MoveList loop
    Score bestScore = searchStack->staticEvaluation;
    NodeCount movesSearched = ZeroNodes;

    for (ChessMove& move : moveList) {
        const Square& src = move.src;
        const Square& dst = move.dst;

        const PieceType capturedPiece = board.pieces[dst];

        //8) If capturing this piece won't bring us close to alpha, skip the capture
        const std::uint32_t phase = board.getPhase();
        if (enableQuiescenceEarlyExit
            && !isInCheck) {
            const Score capturedPieceScore = MaterialParameters[capturedPiece](phase);
            const Score lazyScore = searchStack->staticEvaluation + capturedPieceScore;

            const Score earlyExitThreshold = quiescenceEarlyExitMargin(depthLeft, phase);
            if (lazyScore + earlyExitThreshold < alpha) {
                continue;
            }
        }

        //9) Static Exchange Evaluation
        if (enableQuiescenceStaticExchangeEvaluation
            && !isInCheck) {
            move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

            const Score staticExchangeEvaluationExitThreshold = quiescenceStaticExchangeEvaluationMargin(depthLeft, phase);
            if (move.seeScore < staticExchangeEvaluationExitThreshold) {
                continue;
            }
        }

        //10) DoMove
        ChessBoard newBoard = board;
        this->boardMover.dispatchDoMove(newBoard, move);

        searchStack->currentMove = move;

        //11) Recurse to next depth
        Score score;
        switch (nodeType) {
        case NodeType::PV:
            if (movesSearched == ZeroNodes) {
                score = -this->quiescenceSearch<NodeType::PV>(newBoard, searchStack + 1, -beta, -alpha, maxDepth, currentDepth + Depth::ONE);
            }
            else {
                score = -this->quiescenceSearch<NodeType::CUT>(newBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

                if (score > alpha
                    && score < beta) {
                    score = -this->quiescenceSearch<NodeType::PV>(newBoard, searchStack + 1, -beta, -alpha, maxDepth, currentDepth + Depth::ONE);
                }
            }

            break;
        case NodeType::CUT:
            score = -this->quiescenceSearch<NodeType::ALL>(newBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

            break;
        case NodeType::ALL:
            score = -this->quiescenceSearch<NodeType::CUT>(newBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);

            break;
        default:
            assert(0);
        }

        //12) Undo Move
        move.ordinal = static_cast<ChessMoveOrdinal>(score);

        //13) Compare returned value to alpha/beta
        if (score > bestScore) {
            bestScore = score;
            searchStack->bestMove = move;
        }

        if (score > alpha) {
            if (score >= beta) {
                //if (enableKillerMoves
                //    && (searchStack->killer1 != move)) {
                //    searchStack->killer2 = searchStack->killer1;
                //    searchStack->killer1 = move;
                //}

                return score;
            }

            assert(nodeType == NodeType::PV);

            principalVariation.copyBackward(nextPrincipalVariation, move);

            alpha = score;
        }

        movesSearched++;

        move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

        //if (move.seeScore < ZERO_SCORE
        //    && movesSearched >= 0) {
        //    break;
        //}
    }

    //12) Save to Hashtable
    if (enableQuiescenceSearchHashtable
        && nodeType == NodeType::CUT
        && !this->abortedSearch
        ) {
        this->saveToHashtable(board, searchStack->bestMove, alpha, beta, bestScore, currentDepth, depthLeft);
    }

    return bestScore;
}

void ChessSearcher::resetHashtable()
{
    this->hashtable.reset();
}

void ChessSearcher::resetMoveHistory()
{
    this->moveHistory.resetHistory();
}

Score ChessSearcher::rootSearch(const ChessBoard& board, ChessPrincipalVariation& principalVariation, Score alpha, Score beta, Depth maxDepth)
{
    Score bestScore = -INFINITE_SCORE;

    Score score;

    NodeCount movesSearched = ZeroNodes;
    constexpr NodeCount multiPV = 1;

    ChessSearchStack* searchStack = &this->searchStack[0];
    ChessPrincipalVariation& nextPrincipalVariation = (searchStack + 1)->principalVariation;

    this->rootSearchDepth = maxDepth;

    searchStack->distanceFromPv = Depth::ZERO;

    for (ChessMove& move : this->rootMoveList) { 
        ChessBoard newBoard = board;
        this->boardMover.dispatchDoMove(newBoard, move);

        this->moveHistory.addMoveToHistory(newBoard, move);

        searchStack->currentMove = NullMove;

        if (movesSearched < multiPV) {
            if (movesSearched == 0) {
                principalVariation.copyForward(nextPrincipalVariation);
            }
            else {
                nextPrincipalVariation.clear();
            }

            score = -this->search<NodeType::PV>(newBoard, searchStack + 1, -beta, -alpha, maxDepth, Depth::ONE);
        }
        else {
            score = -this->search<NodeType::CUT>(newBoard, searchStack + 1, -(alpha + 1), -alpha, maxDepth, Depth::ONE);

            if (score > alpha
                && score < beta) {
                nextPrincipalVariation.clear();
                score = -this->search<NodeType::PV>(newBoard, searchStack + 1, -beta, -alpha, maxDepth, Depth::ONE);
            }
        }

        this->moveHistory.removeSingleMove();

        if (this->abortedSearch) {
            break;
        }

        move.ordinal = ChessMoveOrdinal(score);
        movesSearched++;

        if (score > bestScore) {
            if (score >= beta) {
                std::stable_sort(this->rootMoveList.begin(), this->rootMoveList.end(), std::greater<ChessMove>());

                return score;
            }

            bestScore = score;

            principalVariation.copyBackward(nextPrincipalVariation, move);

            const NodeCount nodeCount = this->getNodeCount();
            const std::time_t time = this->clock.getElapsedTime(nodeCount);

            if (!enableAspirationWindow) {
                this->searchEventHandlerList.onLineCompleted(principalVariation, time, nodeCount, score, maxDepth);
                this->verifyPrincipalVariation(board, principalVariation, bestScore, maxDepth);
            }
        }

        if (score > alpha) {
            if (movesSearched >= multiPV) {
                alpha = score;
            }

            ChessPrincipalVariation currentPrincipalVariation;
            currentPrincipalVariation.copyBackward(nextPrincipalVariation, move);

            if (score < bestScore) {
                const NodeCount nodeCount = this->getNodeCount();
                const std::time_t time = this->clock.getElapsedTime(nodeCount);

                if (!enableAspirationWindow) {
                    this->searchEventHandlerList.onLineCompleted(currentPrincipalVariation, time, nodeCount, score, maxDepth);
                    this->verifyPrincipalVariation(board, currentPrincipalVariation, bestScore, maxDepth);
                }
            }
        }
    }

    std::stable_sort(this->rootMoveList.begin(), this->rootMoveList.end(), std::greater<ChessMove>());

    return bestScore;
}

bool ChessSearcher::saveToHashtable(const ChessBoard& board, const ChessMove& move, Score alpha, Score beta, Score score, Depth currentDepth, Depth depthLeft)
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
Score ChessSearcher::search(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
{
    ChessPrincipalVariation& principalVariation = searchStack->principalVariation;

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
    if (enableMateDistancePruning) {
        alpha = std::max(LostInDepth(currentDepth - Depth::ONE), alpha);
        beta = std::min(WinInDepth(currentDepth), beta);

        if (alpha >= beta) {
            if constexpr (nodeType == NodeType::PV) {
                principalVariation.clear();
            }

            return alpha;
        }
    }

    const Depth depthLeft = maxDepth - currentDepth;

    if (nodeType == NodeType::PV) {
        searchStack->distanceFromPv = Depth::ZERO;
    }
    else {
        searchStack->distanceFromPv = (searchStack - 1)->distanceFromPv + Depth::ONE;
    }

    //4) Quiescence Search
    const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

    if (depthLeft <= Depth::ZERO) {
        return this->quiescenceSearch<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);
    }

    assert(depthLeft > Depth::ZERO);

    this->nodeCount++;

    //5) Check Hashtable
    bool hashFound = false;

    Depth hashDepthLeft = Depth::ZERO;
    Score hashScore = ZERO_SCORE;

    if (enableSearchHashtable) {
        HashtableEntry hashtableEntry;
        hashFound = this->checkHashtable(board, hashtableEntry);

        if (hashFound) {
            const HashtableEntryType hashtableEntryType = hashtableEntry.getType();
            hashDepthLeft = hashtableEntry.getDepthLeft();
            hashScore = hashtableEntry.getScore(currentDepth);

            //if (nodeType != NodeType::PV
            //    && IsMateScore(hashScore)) {
            //    return hashScore;
            //}

            if (nodeType != NodeType::PV
                //&& !IsMateScore(hashScore)
                && hashDepthLeft >= depthLeft) {

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

    //6) Get static Evaluation
    if (isInCheck) {
        searchStack->staticEvaluation = LostInDepth(currentDepth);
        searchStack->passedPawns = this->evaluator.calculatePassedPawns(board, board.sideToMove);
    }
    else if (enableSearchHashtable) {
        if (hashFound) {
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

    const std::uint32_t phase = board.getPhase();

    const bool colorIsWhite = board.sideToMove == Color::WHITE;
    const Bitboard* piecesToMove = colorIsWhite ? board.whitePieces : board.blackPieces;

    const Bitboard pawnsAndKing = piecesToMove[PieceType::PAWN] | piecesToMove[PieceType::KING];

    if (nodeType != NodeType::PV
        && !hashFound
        && !isInCheck
        && !IsMateScore(alpha)
        && !IsDrawScore(alpha)
        && pawnsAndKing != piecesToMove[PieceType::ALL]) {

        //7) Futility Pruning
        if (enableFutilityPruning
            && depthLeft < Depth::THREE
            && searchStack->staticEvaluation < BASICALLY_WINNING_SCORE
            && searchStack->staticEvaluation >= beta + FutilityMargin(depthLeft, phase)) {
            return searchStack->staticEvaluation;
        }

        //8) Razoring
        if (enableRazoring
            && depthLeft < Depth::TWO
            && searchStack->staticEvaluation < alpha - RazorMargin(depthLeft, phase)) {
            const Score razorScore = this->quiescenceSearch<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);

            if (razorScore < alpha) {
                return razorScore;
            }
        }

        //9) Null Move Pruning
        if (enableNullMove
//            && !board.hasMadeNullMove()
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
                const Depth nullVerificationReduction = NullMoveVerificationReduction(depthLeft, phase);
                const Score verifiedNullScore = this->search<nodeType>(board, searchStack, alpha, beta, maxDepth - nullVerificationReduction, currentDepth);

                isMateThreat = IsMateScore(verifiedNullScore);
                if (!isMateThreat
                    && verifiedNullScore >= beta) {

                    return verifiedNullScore;
                }
            }
        }
    }

    //10) Generate Moves.  Return if Checkmate or Stalemate.
    MoveList<ChessMove>& moveList = searchStack->moveList;
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

    //11) Search Loop
    const Score score = this->searchLoop<nodeType>(board, searchStack, alpha, beta, maxDepth, currentDepth);

    //12) Save to Hashtable
    if (enableSearchHashtable
//        && nodeType == NodeType::CUT
        && !this->abortedSearch
        ) {
        this->saveToHashtable(board, searchStack->bestMove, alpha, beta, score, currentDepth, depthLeft);
    }

    return score;
}

template <NodeType nodeType, bool iidSearch, bool capturesOnly>
Score ChessSearcher::searchLoop(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth)
{
    MoveList<ChessMove>& moveList = searchStack->moveList;
    ChessPrincipalVariation& principalVariation = searchStack->principalVariation;

    if (nodeType == NodeType::PV
        && principalVariation.size() > 0) {
        ChessPrincipalVariation& nextPrincipalVariation = (searchStack + 1)->principalVariation;
        principalVariation.copyForward(nextPrincipalVariation);

        searchStack->pvMove = principalVariation[0];
        principalVariation.clear();
    }

    //1) Internal Iterative Deepening
    const Depth depthLeft = maxDepth - currentDepth;

    if (enableInternalIterativeDeepening
        && nodeType != NodeType::PV
        //&& nodeType == NodeType::CUT
        && depthLeft > Depth::THREE) {

        constexpr std::array<Depth, 3> IirReductionPerNodeType = { Depth::ZERO, Depth::FOUR, Depth::FOUR };
        constexpr Depth IirReduction = IirReductionPerNodeType[static_cast<std::uint32_t>(nodeType)];

        const Score iidScore = this->searchLoop<nodeType, true, capturesOnly>(board, searchStack, alpha, beta, maxDepth - IirReduction, currentDepth);

        //if (nodeType != NodeType::PV
        //    && (nodeType != NodeType::ALL
        //        || !IsMateScore(alpha))
        //    && IsMateScore(iidScore)) {
        //    return iidScore;
        //}

        std::stable_sort(moveList.begin(), moveList.end(), std::greater<MoveType>());
    }
    else {
        //Sort Moves by expected value
        this->moveOrderer.reorderMoves<nodeType>(board, moveList, searchStack, this->historyTable);
    }

    //2) Calculate Position Extensions (independent of type of move)
    Depth positionExtensions = Depth::ZERO;
    const bool isInCheck = this->attackGenerator.dispatchIsInCheck(board);

    if (enableExtensions) {
        //***Don't use this reference if currentDepth < 2***
//        SearchStack& ourLastSearchStack = this->searchStack[currentDepth - Depth::TWO];

        if (moveList.size() == 1) {
            positionExtensions += Depth::ONE;
        }

        if (isInCheck) {
            positionExtensions += Depth::ONE;
        }

        //if (!isInCheck
        //    && currentDepth >= 2
        //    && currentSearchStack.staticEvaluation >= BASICALLY_WINNING_SCORE
        //    && ourLastSearchStack.staticEvaluation < BASICALLY_WINNING_SCORE) {
        //    positionExtensions += Depth::ONE;
        //}
    }

    Score bestScore = -INFINITE_SCORE;
    NodeCount movesSearched = ZeroNodes;

    for (ChessMove& move : moveList) {
        const Square& src = move.src;
        const Square& dst = move.dst;

        const PieceType movingPiece = board.pieces[src];
        const PieceType capturedPiece = board.pieces[dst];

        const PieceType promotionPiece = move.promotionPiece;

        assert(movingPiece != PieceType::NO_PIECE);

        if (capturesOnly) {
            if (capturedPiece == PieceType::NO_PIECE) {
                continue;
            }
        }

        //3) DoMove
        ChessBoard nextBoard = board;
        this->boardMover.dispatchDoMove(nextBoard, move);

        this->hashtable.prefetch(nextBoard.hashValue);

        //if (maxDepth + extensions == currentDepth + Depth::ONE) {
        //    this->evaluator.prefetch(nextBoard.hashValue);
        //}

        searchStack->currentMove = move;
        this->moveHistory.addMoveToHistory(nextBoard, move);

        Depth extensions = positionExtensions;

        //4) Calculate Move Extensions
        const bool givesCheck = this->attackGenerator.dispatchIsInCheck(nextBoard);

        if (enableMoveExtensions) {
            //Extend Castle Moves
            if (movingPiece == PieceType::KING
                && (PieceMoves[PieceType::KING][src] & OneShiftedBy(dst)) == EmptyBitboard) {
                extensions += Depth::ONE;
            }

            //Extend a Recapture?
            //const ChessMove lastMove = (searchStack - 1)->currentMove;
            //if (lastMove.capturedPiece != PieceType::NO_PIECE
            //    && lastMove.dst == move.dst) {
            //    extensions += Depth::ONE;
            //}

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
                const bool whiteToMove = board.sideToMove == Color::WHITE;
                const Rank SixthRank = whiteToMove ? Rank::_6 : Rank::_3;

                if ((searchStack->passedPawns & OneShiftedBy(src)) != EmptyBitboard
                    && GetRank(src) == SixthRank) {
                    assert(movingPiece == PieceType::PAWN);

                    extensions += Depth::ONE;
                }

                //Extend Pieces which threaten the Queen?
                if (movingPiece == PieceType::BISHOP) {
                    const Bitboard otherQueens = whiteToMove ? nextBoard.blackPieces[PieceType::QUEEN] : nextBoard.whitePieces[PieceType::QUEEN];
                    const Bitboard dstMoves = BishopMagic(dst, nextBoard.allPieces);

                    for (const Square queenAttackDst : SquareBitboardIterator(otherQueens & dstMoves)) {
                        extensions += Depth::ONE;
                    }
                }

                //Extend if See captures a Minor Piece or more?
                //if (move.seeScore >= ROOK_SCORE) {
                //    extensions += Depth::ONE;
                //}
            }
            else {  //if (move.seeScore >= ZERO_SCORE)
                //Reduce if See captures a Rook or more
                if (nodeType != NodeType::PV
                    && move.seeScore <= -PAWN_SCORE) {
                    extensions -= Depth::ONE;
                }
            }
        }

        const std::uint32_t phase = nextBoard.getPhase();

        //4) Calculate Reductions
        if (enableReductions
            && nodeType != NodeType::PV
            //&& (nodeType != NodeType::PV || movesSearched > 30)
            && bestScore > LostInMaxDepth()
            && !givesCheck
            && !isInCheck
            && capturedPiece == PieceType::NO_PIECE
            //&& promotionPiece == PieceType::NO_PIECE
            //&& extensions == Depth::ZERO
            //&& !IsMateScore(alpha)

            //&& move != searchStack->killer1
            //&& move != searchStack->killer2
            //&& move.ordinal <= ChessMoveOrdinal::UNCLASSIFIED_MOVE

            //Don't reduce passed pawn moves
            && (searchStack->passedPawns & OneShiftedBy(src)) == EmptyBitboard) {
            //TODO: Don't reduce on certain move types

            if (nodeType != NodeType::PV
                && depthLeft < Depth::SEVEN
                && searchStack->staticEvaluation + PruningMargin(depthLeft, movesSearched, phase) < alpha) {
                this->moveHistory.removeSingleMove();
                continue;
            }

            if (nodeType == NodeType::ALL
                && depthLeft < Depth::THREE
                && movesSearched > 40) {
                this->moveHistory.removeSingleMove();
                continue;
            }

            move.seeScore = this->staticExchangeEvaluator.staticExchangeEvaluation(board, move);

            const Score staticExchangeEvaluationReductionThreshold = StaticExchangeEvaluationReductionThreshold(depthLeft, movesSearched, phase);
            if (move.seeScore <= staticExchangeEvaluationReductionThreshold) {
                if (nodeType != NodeType::PV
                    && depthLeft < Depth::TWO) {
                    this->moveHistory.removeSingleMove();
                    continue;
                }

                extensions -= Depth::ONE;
            }

            constexpr std::array<Depth, 3> LateMoveReductionDepthLeftPerNodeType = { Depth::ZERO, Depth::ONE, Depth::ONE };
            constexpr Depth LateMoveReductionDepthLeft = LateMoveReductionDepthLeftPerNodeType[static_cast<std::uint32_t>(nodeType)];

            if (movesSearched > 0
                /*&& depthLeft > LateMoveReductionDepthLeft*/) {
                extensions += Depth::ZERO - LateMoveReductions(depthLeft, movesSearched, phase);
            }
        }

        //5) Recurse
        Score score = ZERO_SCORE;

        switch (nodeType) {
        case NodeType::PV:
            if (movesSearched == ZeroNodes) {
                assert(extensions >= Depth::ZERO);
                score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
            }
            else {
                score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

                if (score > alpha
                    && score < beta) {
                    if (extensions < Depth::ZERO) {
                        extensions = Depth::ZERO;
                    }

                    score = -this->search<NodeType::PV>(nextBoard, searchStack + 1, -beta, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);
                }
            }
            break;
        case NodeType::CUT:
            //Try a reduced search with a higher window; if that cuts, cut without going to full depth?

            score = -this->search<NodeType::ALL>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

            if (score > alpha
                && extensions < Depth::ZERO) {
                score = -this->search<NodeType::ALL>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);
            }

            break;
        case NodeType::ALL:
            //Try a reduced search with a lower window; if that is still below alpha, return without going to full depth?

            score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth + extensions, currentDepth + Depth::ONE);

            if (score > alpha
                && extensions < Depth::ZERO) {
                score = -this->search<NodeType::CUT>(nextBoard, searchStack + 1, -alpha - 1, -alpha, maxDepth, currentDepth + Depth::ONE);
            }

            break;
        default:
            assert(0);
        }

        //6) Undo Move
        this->moveHistory.removeSingleMove();

        move.ordinal = static_cast<ChessMoveOrdinal>(score);

        //7) Check if the new score is a best score
        if (score > bestScore) {
            bestScore = score;
            searchStack->bestMove = move;
        }

        //8a) Check if the new score is > alpha
        if (score > alpha) {
            //9) Check if the new score > beta.  Save some statistics and return.
            if (score >= beta) {
                if (enableHistoryTable) {
                    const std::uint32_t delta = HistoryDelta(depthLeft, phase);
                    this->historyTable.add(movingPiece, dst, delta);
                }

                if (enableKillerMoves
                    && capturedPiece == PieceType::NO_PIECE
                    && promotionPiece == PieceType::NO_PIECE
                    && searchStack->killer1 != move) {
                    searchStack->killer2 = searchStack->killer1;
                    searchStack->killer1 = move;
                }

                return score;
            }

            //8b) Save Principal Variation
            assert(nodeType == NodeType::PV);

            const ChessPrincipalVariation& nextPrincipalVariation = (searchStack + 1)->principalVariation;
            principalVariation.copyBackward(nextPrincipalVariation, move);

            searchStack->pvMove = move;

            //8c) Update new alpha
            alpha = score;
        }

        movesSearched++;
    }

    assert(movesSearched > 0);

    //10) Return alpha.
    return bestScore;
}

void ChessSearcher::setClock(const Clock& clock)
{
    this->clock = clock;
}

void ChessSearcher::verifyPrincipalVariation(const ChessBoard& board, ChessPrincipalVariation& principalVariation, Score score, Depth depth)
{
    assert(principalVariation.size() > 0);
    assert(score == principalVariation[0].ordinal);

    if (score != principalVariation[0].ordinal) {
        std::printf("assert(score == principalVariation[0].ordinal)\n");
        savePosition(board, principalVariation, score, depth);
        return;
    }

    ChessBoard pvBoard = board;
    for (ChessMove& move : principalVariation) {
        this->boardMover.dispatchDoMove(pvBoard, move);

        this->moveHistory.addMoveToHistory(pvBoard, move);
    }

    if (IsDrawScore(score)) {
        const TwoPlayerGameResult gameResult = this->checkBoardGameResult(pvBoard, true, true);

        if (gameResult != TwoPlayerGameResult::DRAW) {
            const Depth pvDepth = static_cast<Depth>(principalVariation.size());
            const Score evaluatedScore = this->evaluator.evaluate(pvBoard, pvDepth, -INFINITE_SCORE, INFINITE_SCORE);

            assert(evaluatedScore == DRAW_SCORE);

            if (evaluatedScore != DRAW_SCORE) {
                std::printf("assert(evaluatedScore == DRAW_SCORE)\n");
                savePosition(board, principalVariation, score, depth);

                goto verifyend;
            }
        }
    }
    else if (IsMateScore(score)) {
        const Depth distanceToMate = DistanceToWin(score);

        assert(distanceToMate == principalVariation.size());

        if (distanceToMate != principalVariation.size()) {
            std::printf("assert(distanceToMate == principalVariation.size())\n");
            savePosition(board, principalVariation, score, depth);

            goto verifyend;
        }

        const bool isInCheck = this->attackGenerator.dispatchIsInCheck(pvBoard);

        assert(isInCheck);

        if (!isInCheck) {
            std::printf("assert(isInCheck)\n");
            savePosition(board, principalVariation, score, depth);

            goto verifyend;
        }

        MoveList<ChessMove> moveList;
        const NodeCount moveCount = this->moveGenerator.DispatchGenerateAllMoves(pvBoard, moveList);

        assert(moveCount == ZeroNodes);

        if (moveCount != ZeroNodes) {
            std::printf("assert(moveCount == ZeroNodes)\n");
            savePosition(board, principalVariation, score, depth);

            goto verifyend;
        }
    }
    else {
        assert(enableAspirationWindow || principalVariation.size() >= depth);

        if (!enableAspirationWindow
            && principalVariation.size() < depth) {
            std::printf("assert(principalVariation.size() >= depth)\n");
            savePosition(board, principalVariation, score, depth);

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
            savePosition(board, principalVariation, score, depth);

            goto verifyend;
        }
    }

verifyend:
    for (std::uint32_t i = 0; i < principalVariation.size(); i++) {
        this->moveHistory.removeSingleMove();
    }
}

bool ChessSearcher::wasSearchAborted()
{
    return this->abortedSearch;
}
