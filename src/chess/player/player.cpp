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

#include <iostream>

#include "player.h"

#include "../../game/personality/parametermap.h"

#include "../endgame/endgame.h"
#include "../endgame/eval/kxk.h"

#include "../eval/parameters.h"

#include "../hash/hash.h"

#include "../search/perft.h"
#include "../search/searcher.h"

extern ParameterMap chessEngineParameterMap;

ChessPlayer::ChessPlayer()
{
    this->currentBoard = 0;

    BoardType board;
    this->boardList.push_back(board);

    this->parameterMap = chessEngineParameterMap;

    //InitializeParameters();
}

void ChessPlayer::applyPersonality(bool strip)
{
    const std::int32_t multiplier = strip ? -1 : 1;

    for (Personality::iterator it = this->personality.begin(); it != this->personality.end(); ++it) {
        const std::string& name = it->first;
        const Score score = multiplier * it->second;

        //First, make sure the parameter exists in case we're passed bad parameters
        this->parameterMap[name];

        Score* parameterScore = this->parameterMap.find(name)->second;
        *parameterScore += score;
    }

    InitializeParameters();

    BoardType& board = this->getCurrentBoard();

    board.materialEvaluation = board.calculateMaterialEvaluation();
    board.pstEvaluation = board.calculatePstEvaluation();
}

TwoPlayerGameResult ChessPlayer::checkBoardGameResult(const BoardType& board) const
{
    return this->searcher.checkBoardGameResult(board, true, true);
}

Score ChessPlayer::evaluateCurrentPosition()
{
    this->applyPersonality();

    BoardType& board = this->getCurrentBoard();

    const Score result = this->evaluator.evaluate(board, Depth::ZERO, -WIN_SCORE, WIN_SCORE);

    this->stripPersonality();

    return result;
}

ChessSearcher::BoardType& ChessPlayer::getCurrentBoard()
{
    return this->boardList[this->currentBoard];
}

std::string ChessPlayer::getCurrentBoardFen()
{
    const BoardType& board = this->getCurrentBoard();

    return board.saveToFen();
}


void ChessPlayer::getMoveImplementation(MoveType& move)
{
    BoardType& board = this->getCurrentBoard();

    this->searcher.setClock(this->clock);
    this->searcher.iterativeDeepeningLoop(board, this->principalVariation);

    move = this->principalVariation[0];
}

void ChessPlayer::resetHashtable()
{
    this->searcher.resetHashtable();
}

void ChessPlayer::stripPersonality()
{
    this->applyPersonality(true);
}
