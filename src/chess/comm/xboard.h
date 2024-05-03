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

#include <ctime>

#include "../../game/comm/comm.h"

#include "../player/player.h"

#include "../types/move.h"

#include "../../game/types/nodecount.h"
#include "../../game/types/score.h"

#include "xboard/xboardsearchanalyzereventhandler.h"
#include "xboard/xboardsearcheventhandler.h"

class XBoardComm : public Communicator<XBoardComm>
{
private:
	bool force = false;

	ChessPlayer player;
    Color sideToMove;

    bool hasAddedSearchAnalyzer = false;
    XBoardSearchAnalyzerSearchEventHandler searchAnalyzerEventHandler;
public:
	XBoardComm();
    ~XBoardComm() {}

    void addSearchAnalyzer();

	void doPlayerMove(ChessMove& playerMove);

	Score evaluateBoard();

	std::string getCurrentBoardFen();

	Clock& getPlayerClock();
	void getPlayerMove(ChessMove& playerMove);

    const ChessBoard getPlayerBoard() const;

    Color getSideToMove() const;

	bool isForced() const;

	void loadPersonalityFile(const std::string& personalityFileName);

	NodeCount perft(Depth depth);

	void processCommandImplementation(const std::string& cmd);
	
	void resetSpecificPosition(std::string& fen);
	void resetStartingPosition();

	void setForce(bool force);
	void setParameter(std::string& name, Score score);
    void setResult(TwoPlayerGameResult result);

	void undoPlayerMove();
};
