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

#include <array>

constexpr bool enableAllSearchFeatures = true;

constexpr bool enableAspirationWindow = enableAllSearchFeatures && true;

constexpr bool enableFutilityPruning = enableAllSearchFeatures && true;
constexpr bool enableReverseFutilityPruning = enableAllSearchFeatures && true;
constexpr bool enableHistoryTable = enableAllSearchFeatures && true;
constexpr bool enableInternalIterativeDeepening = enableAllSearchFeatures && true;
constexpr bool enableKillerMoves = enableAllSearchFeatures && true;
constexpr bool enableMateAtAGlance = enableAllSearchFeatures && false;
constexpr bool enableMateDistancePruning = enableAllSearchFeatures && true;
constexpr bool enableMateHistoryTable = enableAllSearchFeatures && true;
constexpr bool enableMateKillerMoves = enableAllSearchFeatures && true;
constexpr bool enableMoveExtensions = enableAllSearchFeatures && true;
constexpr bool enableNullMove = enableAllSearchFeatures && true;
constexpr bool enableNullMoveVerification = enableAllSearchFeatures && false;
constexpr bool enablePositionExtensions = enableAllSearchFeatures && true;
constexpr bool enableQuiescenceStaticExchangeEvaluation = enableAllSearchFeatures && true;
constexpr bool enableRazoring = enableAllSearchFeatures && true;
constexpr bool enableReductions = enableAllSearchFeatures && true;
constexpr bool enableProbcut = enableAllSearchFeatures && true;

constexpr bool enableSearchHashtable = enableAllSearchFeatures && true;
constexpr bool enableQuiescenceSearchHashtable = enableAllSearchFeatures && enableSearchHashtable && true;

constexpr bool enableQuiescenceEarlyExit = enableAllSearchFeatures && false;

#include "../board/attackgenerator.h"
#include "../board/board.h"
#include "../board/boardmover.h"
#include "../board/movegenerator.h"
#include "../board/moveorderer.h"
#include "../board/see.h"

#include "../eval/evaluator.h"

//#include "../hash/chesshashtable.h"
#include "../../game/search/hashtable.h"

#include "../../game/types/depth.h"

#include "movehistory.h"

#include "../types/nodetype.h"
#include "../types/score.h"
#include "../types/searchstack.h"

#include "history.h"

#include "chesspv.h"

#include "../../game/search/events/searcheventhandler.h"

//#include "../../game/search/pvs.h"

constexpr std::uint32_t SearchStackSize = Depth::MAX + 3;

constexpr std::uint32_t HASH_MEGABYTES = 2;
constexpr std::uint32_t HASH_SIZE = HASH_MEGABYTES * 65536;

//class ChessPrincipalVariationSearcher : public PrincipalVariationSearcher<ChessPrincipalVariationSearcher, ChessEvaluator, ChessMoveGenerator, ChessPrincipalVariation, ChessSearchStack, ChessBoardMover, ChessMoveOrderer, ChessHistoryTable, ChessStaticExchangeEvaluator>
//{
//public:
//    using BoardType = typename ChessEvaluator::BoardType;
//    using EvaluationType = typename ChessEvaluator::EvaluationType;
//    using MoveGeneratorType = ChessMoveGenerator;
//    using MoveType = typename BoardType::MoveType;
//    using PrincipalVariationType = ChessPrincipalVariation;
//
//    ChessPrincipalVariationSearcher() = default;
//    ~ChessPrincipalVariationSearcher() = default;
//
//    constexpr void initializeSearchImplementation(const BoardType& board) const
//    {
//
//    }
//};

class ChessSearcher
{
protected:
    Clock clock;

    ChessEvaluator evaluator;

    ChessAttackGenerator attackGenerator;
    ChessMoveGenerator moveGenerator;
    ChessMoveOrderer moveOrderer;
    const ChessStaticExchangeEvaluator staticExchangeEvaluator;

    const ChessBoardMover boardMover;

    Hashtable hashtable;

    ChessMoveHistory moveHistory;
    MoveList<ChessMove> rootMoveList;

    NodeCount nodeCount = 0;
    NodeCount quiescentNodeCount = 0;

    std::array<ChessSearchStack, SearchStackSize> searchStack;

    SearchEventHandlerList<ChessBoard, ChessPrincipalVariation> searchEventHandlerList;

    ChessHistoryTable continuationHistoryTable;
    ChessHistoryTable historyTable;
    ChessHistoryTable mateHistoryTable;

    Depth rootSearchDepth;

    bool abortedSearch = false;

    bool checkHashtable(const ChessBoard& board, HashtableEntry& hashtableEntry) const;

    template <NodeType nodeType>
    Score quiescenceSearch(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth);

    Score rootSearch(const ChessBoard& board, ChessPrincipalVariation& principalVariation, Score alpha, Score beta, Depth maxDepth);

    bool saveToHashtable(const ChessBoard& board, const ChessMove& move, Score alpha, Score beta, Score score, Depth currentDepth, Depth depthLeft);

    template <NodeType nodeType>
    Score search(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth);

    template <NodeType nodeType, bool iidSearch = false, bool capturesOnly = false>
    Score searchLoop(ChessBoard& board, ChessSearchStack* searchStack, Score alpha, Score beta, Depth maxDepth, Depth currentDepth);

    void verifyPrincipalVariation(const ChessBoard& board, ChessPrincipalVariation& principalVariation, Score score, Depth depth);
public:
    using BoardType = typename ChessEvaluator::BoardType;
    using EvaluationType = typename ChessEvaluator::EvaluationType;
    using MoveGeneratorType = ChessMoveGenerator;
    using MoveType = typename BoardType::MoveType;
    using PrincipalVariationType = ChessPrincipalVariation;

    using EventHandler = SearchEventHandler<BoardType, PrincipalVariationType>;
    using EventHandlerSharedPtr = SearchEventHandlerSharedPtr<BoardType, PrincipalVariationType>;

    ChessSearcher();
    ~ChessSearcher() = default;

    constexpr void initializeSearchImplementation(const BoardType& board) const
    {

    }

    void addSearchEventHandler(SearchEventHandlerSharedPtr<BoardType, PrincipalVariationType>& searchEventHandler)
    {
        this->searchEventHandlerList.push_back(searchEventHandler);
    }

    void addMoveToHistory(ChessBoard& board, ChessMove& move);

    TwoPlayerGameResult checkBoardGameResult(const ChessBoard& board, bool checkMoveCount, bool isPrincipalVariation) const;

    NodeCount getNodeCount();

    void initialize();

    void iterativeDeepeningLoop(const ChessBoard& board, ChessPrincipalVariation& principalVariation);

    void resetHashtable();
    void resetMoveHistory();

    void setClock(const Clock& clock);

    bool wasSearchAborted();
};
