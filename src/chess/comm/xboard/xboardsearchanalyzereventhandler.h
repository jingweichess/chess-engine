#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <random>
#include <vector>

#ifdef _MSC_VER
# include <intrin.h>
#else
# include <x86intrin.h>
#endif

#include "../../search/events/searcheventhandler.h"

#include "../../../game/math/statistics.h"

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

class XBoardSearchAnalyzerSearchEventHandler : public SearchEventHandler
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
        std::printf("Saving position %s\n", this->searchAnalysis.board.saveToFen().c_str());
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

        std::ofstream positionFile;

        positionFile.open("data/search-analysis.txt", std::ofstream::out | std::ofstream::app);

        //epoch,random,fen,result,depth,score,nodecount,principal variation

        std::mt19937_64 prng(__rdtsc());

        std::uint64_t random = prng();
        SearchAnalysis& searchAnalysis = this->searchAnalysisList[random % this->searchAnalysisList.size()];

        const std::string fen = searchAnalysis.board.saveToFen();

        positionFile << "0x" << std::hex << std::setw(16) << std::setfill('0') << this->epoch << std::dec << ',';

        positionFile << "0x" << std::hex << std::setw(16) << std::setfill('0') << this->random << std::dec << ',' << fen << ',';

        switch (XBoardSearchAnalyzerSearchEventHandler::result) {
        case TwoPlayerGameResult::WIN:
            positionFile << "1.0,";
            break;
        case TwoPlayerGameResult::DRAW:
            positionFile << "0.5,";
            break;
        case TwoPlayerGameResult::LOSS:
            positionFile << "0.0,";
            break;
        default:
            assert(0);
        }

        positionFile << searchAnalysis.analysisList.size() << ',';

        const SearchAnalysisForDepth& lastAnalysis = searchAnalysis.analysisList[searchAnalysis.analysisList.size() - 1];
        positionFile << lastAnalysis.score << ',' << lastAnalysis.nodeCount << ',';

        const ChessPrincipalVariation& principalVariation = lastAnalysis.principalVariation;
        principalVariation.printToFile(positionFile);

        positionFile << std::endl;
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

        if (board.getPieceCount() > 9
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
