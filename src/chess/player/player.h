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

#include <cstdint>
#include <vector>

#include "../board/board.h"

#include "../eval/evaluator.h"

#include "../../game/personality/parametermap.h"
#include "../../game/personality/personality.h"

#include "../search/movehistory.h"
#include "../search/searcher.h"

#include "../types/move.h"

class ChessPlayer// : public Player<ChessPlayer, ChessSearcher>
{
public:
    using BoardType = typename ChessSearcher::BoardType;

    using MoveGeneratorType = typename ChessSearcher::MoveGeneratorType;
    using MoveType = typename ChessSearcher::MoveType;

protected:
    ChessEvaluator evaluator;

    ChessSearcher searcher;

    const ChessMoveGenerator moveGenerator;
    ChessMoveHistory moveHistory;

    std::vector<typename ChessSearcher::BoardType> boardList;
    std::uint32_t currentBoard;

    Clock clock;
    ParameterMap parameterMap;
    Personality personality;

    ChessPrincipalVariation principalVariation;

    ChessBoardMover boardMover;

public:
	ChessPlayer();
	~ChessPlayer() = default;

    void addSearchEventHandler(SearchEventHandlerSharedPtr& searchEventHandler)
    {
        this->searcher.addSearchEventHandler(searchEventHandler);
    }

    void applyPersonality(bool strip = false);
    void stripPersonality();

    TwoPlayerGameResult checkBoardGameResult(const BoardType& board) const;

    void doMove(MoveType& move)
    {
        BoardType nextBoard = this->boardList[this->currentBoard];

        if (nextBoard.sideToMove == Color::WHITE) {
            this->boardMover.doMove<true>(nextBoard, move);
        }
        else {
            this->boardMover.doMove<false>(nextBoard, move);
        }

        this->currentBoard++;

        if (this->currentBoard >= this->boardList.size()) {
            this->boardList.push_back(nextBoard);
        }
        else {
            this->boardList[this->currentBoard] = nextBoard;
        }

        this->searcher.addMoveToHistory(nextBoard, move);

        if (this->principalVariation.size() > 0
            && move == principalVariation[0]) {
            this->principalVariation.erase(this->principalVariation.begin());
        }
    }

    Score evaluateCurrentPosition();

    const ChessBoard getBoard() const
    {
        return this->boardList[this->currentBoard - 1];
    }

    Clock& getClock()
    {
        return this->clock;
    }

    BoardType& getCurrentBoard();
    std::string getCurrentBoardFen();

    void getMove(MoveType& move)
    {
        this->applyPersonality();

        this->getMoveImplementation(move);

        this->stripPersonality();
    }

    void getMoveImplementation(MoveType& move);

    void loadPersonalityFile(std::string& personalityFileName)
    {
        std::fstream personalityFile;

        personalityFile.open(personalityFileName, std::fstream::ios_base::in);

        if (!personalityFile.is_open()) {
            return;
        }

        while (true) {
            std::string parameterName;
            Score parameterScore;

            personalityFile >> parameterName >> parameterScore;

            if (personalityFile.eof()) {
                break;
            }

            this->setParameter(parameterName, parameterScore);
        }

        personalityFile.close();
    }

    void resetHashtable();

    void resetSpecificPosition(const std::string& fen)
    {
        this->currentBoard = 0;

        BoardType board;
        board.resetSpecificPosition(fen);

        this->boardList[0] = board;

        this->searcher.resetMoveHistory();
    }

    void resetStartingPosition()
    {
        this->currentBoard = 0;

        BoardType board;
        board.resetStartingPosition();

        this->boardList[0] = board;

        this->searcher.resetMoveHistory();
    }

    void setBoard(const BoardType& board)
    {
        this->boardList[0] = board;
        this->currentBoard = 0;

        this->applyPersonality();

        this->boardList[0].materialEvaluation = this->boardList[0].calculateMaterialEvaluation();
        this->boardList[0].pstEvaluation = this->boardList[0].calculatePstEvaluation();

        this->stripPersonality();

        this->searcher.resetMoveHistory();
    }

    void setClock(Clock& clock)
    {
        this->clock = clock;
    }

    void setParameter(const std::string& name, Score score)
    {
        this->personality.setParameter(name, score);
    }

    void setPersonality(Personality& personality)
    {
        this->personality = personality;
    }

    void undoMove()
    {
        if (this->currentBoard > 0) {
            this->currentBoard--;
        }
    }
};
