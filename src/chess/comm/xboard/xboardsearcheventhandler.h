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

#include <iomanip>

#include "../../../game/search/events/searcheventhandler.h"

#include "../../board/board.h"
#include "../../search/chesspv.h"

class XBoardSearchEventHandler : public SearchEventHandler<ChessBoard, ChessPrincipalVariation>
{
public:
    XBoardSearchEventHandler() = default;
    ~XBoardSearchEventHandler() = default;

    void onLineCompleted(const ChessPrincipalVariation& principalVariation, std::time_t time, NodeCount nodeCount, Score score, Depth depth)
    {
        //if (time < 10) {
        //    return;
        //}

        std::cout << int(depth / Depth::ONE) << ' ' << std::fixed << std::setprecision(2);
        if (IsMateScore(score)) {
            if (score > (WIN_SCORE - Depth::MAX)) {
                score = 10000 - (WIN_SCORE - score);
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
                score = 10000 - (WIN_SCORE - score);
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
