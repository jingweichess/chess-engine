#pragma once

#include "../../search/events/searcheventhandler.h"

class XboardSearchEventHandler : public SearchEventHandler
{
public:
    constexpr XboardSearchEventHandler() = default;
    constexpr ~XboardSearchEventHandler() = default;

    void onLineCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        //if (time < 10) {
        //    return;
        //}

        std::cout << int(depth / Depth::ONE) << ' ' << std::fixed << std::setprecision(2);
        if (IsMateScore(score)) {
            if (score > (WIN_SCORE - Depth::MAX)) {
                score = 10000 - (WIN_SCORE - score + 2);
            }
            else if (score < (-WIN_SCORE + Depth::MAX)) {
                score = -10000 + (WIN_SCORE + score);
            }

            std::cout << score / 100.0f;
        }
        else {
            std::cout << static_cast<int>(score / (PAWN_SCORE / 100.0f));
        }

        std::cout << ' ' << time / 10 << ' ' << nodeCount << ' ';

        principalVariation.print();

        std::cout << std::endl;
        //std::cout.put(std::cout.widen('\n'));
    }

    void onDepthCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        //if (time < 10) {
        //    return;
        //}

        std::cout << int(depth / Depth::ONE) << ' ' << std::fixed << std::setprecision(2);
        if (IsMateScore(score)) {
            if (score > (WIN_SCORE - Depth::MAX)) {
                score = 10000 - (WIN_SCORE - score + 2);
            }
            else if (score < (-WIN_SCORE + Depth::MAX)) {
                score = -10000 + (WIN_SCORE + score);
            }

            std::cout << score / 100.0f;
        }
        else {
            std::cout << static_cast<int>(score / (PAWN_SCORE / 100.0f));
        }

        std::cout << ' ' << time / 10 << ' ' << nodeCount << ' ';

        principalVariation.print();

        std::cout << std::endl;
        //std::cout.put(std::cout.widen('\n'));
    }

    void onSearchCompleted(const ChessBoard& board)
    {

    }
};
