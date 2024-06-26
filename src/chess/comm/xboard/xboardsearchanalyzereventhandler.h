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

#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <vector>

#ifdef _MSC_VER
# include <intrin.h>
#else
# include <x86intrin.h>
#endif

#include "../../../game/search/events/searcheventhandler.h"

#include "../../board/boardmover.h"

#include "../../../game/math/statistics.h"

#include "../../search/chesspv.h"

struct SearchAnalysisForDepth
{
    ChessPrincipalVariation principalVariation;
    NodeCount nodeCount;
    Score score;
};

struct SearchAnalysis
{
    ChessBoard board;
    std::vector<SearchAnalysisForDepth> analysisList;
};

class XBoardSearchAnalyzerSearchEventHandler : public SearchEventHandler<ChessBoard, ChessPrincipalVariation>
{
protected:
    SearchAnalysis searchAnalysis;
    std::map<std::uint32_t, std::uint32_t> moveCounts;

    std::uint64_t epoch;
    std::uint64_t random;

    std::vector<SearchAnalysis> searchAnalysisList;

    static TwoPlayerGameResult result;

    bool shouldSave(Statistics<Score>& statistics) const
    {
        const Depth searchDepth = static_cast<Depth>(this->searchAnalysis.analysisList.size());
        //if (searchDepth < 10) {
        //    return false;
        //}

        //Don't save searches that end in draws
        const Score lastScore = this->searchAnalysis.analysisList[searchDepth - 1].score;
        if (lastScore == DRAW_SCORE
            || lastScore > (UNIT_SCORE * 15)
            || lastScore < (-UNIT_SCORE * 15)) {
            return false;
        }

        //Don't save searches that are too quick
        //const NodeCount nodeCount = this->searchAnalysis.analysisList[searchDepth - 1].nodeCount;
        //if (nodeCount < 100000) {
        //    return false;
        //}

        //Return volatile searches with many root moves in pv.
        if (this->moveCounts.size() >= 4) {
            return true;
        }

        bool result = false;

        for (Depth depth = Depth::ZERO; depth < searchDepth; depth++) {
            const SearchAnalysisForDepth& analysisForDepth = this->searchAnalysis.analysisList[depth];

            const Score score = analysisForDepth.score;

            //If the score is so high it's not going to the evaluation function, don't save
            if (std::abs(score) > (UNIT_SCORE * 15)) {
                return false;
            }

            const Score averageScore = statistics.average();
            statistics.push_back(score);

            //If the difference between the score and the average up to this depth, we want to save this position
            if (depth > Depth::FIVE
                && std::abs(score - averageScore) > 128) {
                result = true;
            }
        }

        //Save if the standard deviation is too high; this position is unstable.
        if (!result) {
            result = statistics.stddev() > 128.0f;
        }

        return result;
    }

    void save()
    {
        this->searchAnalysisList.push_back(this->searchAnalysis);
    }

public:
    XBoardSearchAnalyzerSearchEventHandler()
    {
        std::mt19937_64 prng(__rdtsc());

        this->random = prng();

        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        const std::chrono::system_clock::duration epoch = now.time_since_epoch();
        this->epoch = epoch.count();
    };

    ~XBoardSearchAnalyzerSearchEventHandler()
    {
        if (this->searchAnalysisList.size() == 0
            || this->result == TwoPlayerGameResult::NO_GAMERESULT) {
            return;
        }

        //epoch,random,fen,result,depth,score,nodecount,principal variation

        std::mt19937_64 prng(__rdtsc());
        const std::uint64_t random = prng();
        SearchAnalysis& searchAnalysis = this->searchAnalysisList[random % this->searchAnalysisList.size()];

        std::stringstream ss;

        //for (SearchAnalysis& searchAnalysis : this->searchAnalysisList) {
            std::size_t highestDepth = searchAnalysis.analysisList.size();
            SearchAnalysisForDepth& bestAnalysisForDepth = searchAnalysis.analysisList[highestDepth - 1];

            ChessBoard& board = searchAnalysis.board;

            ChessBoardMover boardMover;

            TwoPlayerGameResult gameResult = XBoardSearchAnalyzerSearchEventHandler::result;
            SearchAnalysisForDepth& lastAnalysis = searchAnalysis.analysisList[searchAnalysis.analysisList.size() - 1];

            for (ChessMove& move : bestAnalysisForDepth.principalVariation) {
                boardMover.dispatchDoMove(board, move);

                gameResult = -gameResult;
                lastAnalysis.score = -lastAnalysis.score;
            }

            ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << this->epoch << std::dec << ',';

            const std::string fen = board.saveToFen();
            ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << this->random << std::dec << ',' << fen << ',';

            switch (gameResult) {
            case TwoPlayerGameResult::WIN:
                ss << "1.0,";
                break;
            case TwoPlayerGameResult::DRAW:
                ss << "0.5,";
                break;
            case TwoPlayerGameResult::LOSS:
                ss << "0.0,";
                break;
            default:
                assert(0);
            }

            ss << searchAnalysis.analysisList.size() << ',';
            ss << lastAnalysis.score << ',' << lastAnalysis.nodeCount << ',';

            const ChessPrincipalVariation& principalVariation = lastAnalysis.principalVariation;
            principalVariation.printToStringStream(ss);

            ss << std::endl;
        //}

        std::ofstream positionFile;
        positionFile.open("data/positions.txt", std::ofstream::out | std::ofstream::app);
        positionFile << ss.str();
    }

    void onLineCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        const ChessMove& move = principalVariation[0];

        const std::uint32_t moveHash = (move.src * 64) + move.dst;

        this->moveCounts[moveHash]++;
    }

    void onDepthCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        SearchAnalysisForDepth analysis;

        analysis.principalVariation = principalVariation;
        analysis.nodeCount = nodeCount;
        analysis.score = score;

        this->searchAnalysis.analysisList.push_back(analysis);
    }

    void onSearchCompleted(const ChessBoard& board)
    {
        this->searchAnalysis.board = board;

        Statistics<Score> statistics;

        if (board.getPhase() > 9
            && this->shouldSave(statistics)) {
            this->save();
        }

        this->moveCounts.clear();
        this->searchAnalysis.analysisList.clear();
    }

    void setResult(TwoPlayerGameResult result)
    {
        XBoardSearchAnalyzerSearchEventHandler::result = result;
    }
};
