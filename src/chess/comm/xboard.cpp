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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "xboard.h"

#include "../board/movegenerator.h"

#include "../search/perft.h"

#include "../../game/types/depth.h"
#include "../../game/types/nodecount.h"

#include "xboard/xboardsearcheventhandler.h"

struct XBoardCommand {
    std::string command;
    void (*function)(XBoardComm* xboard, std::stringstream& cmd);
};

static void xboardEval(XBoardComm* xboard, std::stringstream& cmd)
{
    const Score score = xboard->evaluateBoard();

    std::cout << "Evaluation: " << score << std::endl;
}

static void xboardFen(XBoardComm* xboard, std::stringstream& cmd)
{
    const std::string fen = xboard->getCurrentBoardFen();

    std::cout << fen << std::endl;
}

static void xboardForce(XBoardComm* xboard, std::stringstream& cmd)
{
    xboard->setForce(true);
}

static void saveOutOfBookFen(const std::string& fen)
{
    std::ofstream outOfbookFenFile;

    outOfbookFenFile.open("data/out-of-book-fens.txt", std::ofstream::out | std::ofstream::app);
    outOfbookFenFile << fen << std::endl;
}

static void xboardGo(XBoardComm* xboard, std::stringstream& cmd)
{
    ChessMove playerMove;
    ChessPrincipalVariation principalVariation;

    if (xboard->isForced()) {
        const ChessBoard board = xboard->getPlayerBoard();
        saveOutOfBookFen(board.saveToFen());
    }

    xboard->getPlayerMove(playerMove);

    xboard->doPlayerMove(playerMove);

    std::cout << "move ";
    principalVariation.printMoveToConsole(playerMove);
    std::cout << std::endl;

    xboard->setForce(false);
}

static void xboardLevel(XBoardComm* xboard, std::stringstream& cmd)
{
    NodeCount moveCount = ZeroNodes;
    std::string tournamentTime;
    std::time_t increment = 0;

    cmd >> moveCount;
    cmd >> tournamentTime;
    cmd >> increment;

    //Arena does not give a colon, and tournamentTime is in minutes
    //Cutechess-cli gives a colon and is in the M:S format
    const char* time = tournamentTime.c_str();
    int i = 0;

    if (time[i] == '/') {
        i++;
    }

    std::time_t minutes = 0, seconds = 0;

    while (isdigit(time[i])) {
        minutes = minutes * 10 + (time[i] - '0');
        i++;
    }

    if (time[i] == ':') {
        i++;

        while (isdigit(time[i])) {
            seconds = seconds * 10 + (time[i] - '0');
            i++;
        }
    }

    seconds = seconds + minutes * 60;

    xboard->getPlayerClock().setClockLevel(moveCount, 1000 * seconds, 1000 * increment);
}

static void xboardNew(XBoardComm* xboard, std::stringstream& cmd)
{
    xboard->resetStartingPosition();
}

static void xboardNps(XBoardComm* xboard, std::stringstream& cmd)
{
    NodeCount nps;
    cmd >> nps;

    xboard->getPlayerClock().setClockNps(nps);
}

static void xboardOtim(XBoardComm* xboard, std::stringstream& cmd)
{
    std::time_t centiseconds;
    cmd >> centiseconds;

    xboard->getPlayerClock().setClockOpponentTimeLeft(centiseconds * 10);
}

static void xboardPerft(XBoardComm* xboard, std::stringstream& cmd)
{
    Clock clock;

    //There's no operator to get a Depth from a std::stringstream
    int depth;
    cmd >> depth;

    NodeCount nodeCount;
    Depth maxDepth = Depth::ONE * depth;

    std::time_t time = 0;

    if (maxDepth == Depth::ZERO) {
        nodeCount = 1;
    }
    else {
        clock.startClock();

        nodeCount = xboard->perft(maxDepth);

        time = clock.getElapsedTime(ZeroNodes);
    }

    std::cout << "Total: " << nodeCount << " Moves" << std::endl;

    NodeCount nps;

    if (time == 0) {
        nps = nodeCount;
    }
    else {
        nps = 1000 * nodeCount / time;
    }

    std::cout << "Time: " << time << " ms (" << nps << " nps)" << std::endl;
}

static void xboardPersonality(XBoardComm* xboard, std::stringstream& cmd)
{
    std::string personalityFileName;
    cmd >> personalityFileName;

    xboard->loadPersonalityFile(personalityFileName);
}

static void xboardPing(XBoardComm* xboard, std::stringstream& cmd)
{
	int ping;
	cmd >> ping;

	std::cout << "pong " << ping << std::endl;
}

static void xboardPositionDatabase(XBoardComm* xboard, std::stringstream& cmd)
{
    xboard->addSearchAnalyzer();
}

static void xboardQuit(XBoardComm* xboard, std::stringstream& cmd)
{
    xboard->finish();
}

static void xboardResult(XBoardComm* xboard, std::stringstream& cmd)
{
    std::string result;
    cmd >> result;

    if (result == "1-0") {
        xboard->setResult(xboard->getSideToMove() == Color::WHITE ? TwoPlayerGameResult::WIN : TwoPlayerGameResult::LOSS);
    }
    else if (result == "0-1") {
        xboard->setResult(xboard->getSideToMove() == Color::WHITE ? TwoPlayerGameResult::LOSS : TwoPlayerGameResult::WIN);
    }
    else if (result == "1/2-1/2") {
        xboard->setResult(TwoPlayerGameResult::DRAW);
    }
}

static void xboardSd(XBoardComm* xboard, std::stringstream& cmd)
{
    int depth;
    cmd >> depth;

    xboard->getPlayerClock().setClockDepth(Depth::ONE * depth);
}

static void xboardSetBoard(XBoardComm* xboard, std::stringstream& cmd)
{
    std::string setboard = cmd.str();
    setboard = setboard.substr(setboard.find_first_of(' ') + 1);

    xboard->resetSpecificPosition(setboard);
}

static void xboardSetValue(XBoardComm* xboard, std::stringstream& cmd)
{
    std::string name;
    Score score;

    cmd >> name >> score;

    xboard->setParameter(name, score);
}

static void xboardSn(XBoardComm* xboard, std::stringstream& cmd)
{
    NodeCount nodes;
    cmd >> nodes;

    xboard->getPlayerClock().setClockNodes(nodes);
}

static void xboardSt(XBoardComm* xboard, std::stringstream& cmd)
{
    std::time_t seconds;
    cmd >> seconds;

    xboard->getPlayerClock().setClockSearchTime(seconds * 1000);
}

static void xboardTime(XBoardComm* xboard, std::stringstream& cmd)
{
    std::time_t centiseconds;
    cmd >> centiseconds;

    xboard->getPlayerClock().setClockEngineTimeLeft(centiseconds * 10);
}

static void xboardUndo(XBoardComm* xboard, std::stringstream& cmd)
{
    xboard->undoPlayerMove();
}

static void xboardUserMove(XBoardComm* xboard, std::stringstream& cmd)
{
    std::string moveString;
    cmd >> moveString;

    ChessMove move;
    ChessPrincipalVariation principalVariation;

    principalVariation.stringToMove(moveString, move);

    //Totally don't verify the move coming in from the interface
    xboard->doPlayerMove(move);

    if (xboard->isForced()) {
        return;
    }

    xboardGo(xboard, cmd);
}

static void xboardXboard(XBoardComm* xboard, std::stringstream& cmd)
{
    std::cout << "feature setboard=1 usermove=1 time=1 analyze=0 myname=\"Jing Wei\" name=1 nps=1 done=1\n";

    xboardNew(xboard, cmd);
}

static const struct XBoardCommand XBoardCommandList[] =
{
    { "eval", xboardEval},
    { "exit", xboardQuit },
    { "fen", xboardFen },
    { "force", xboardForce },
    { "go", xboardGo },
    { "level", xboardLevel },
    { "new", xboardNew },
    { "nps", xboardNps },
    { "otim", xboardOtim },
    { "perft", xboardPerft },
    { "personality", xboardPersonality },
    { "ping", xboardPing },
    { "quit", xboardQuit },
    { "result", xboardResult },
    { "sd", xboardSd },
    { "setboard", xboardSetBoard },
    { "setvalue", xboardSetValue },
    { "sn", xboardSn },
    { "st", xboardSt },
    { "time", xboardTime },
    { "undo", xboardUndo },
    { "usermove", xboardUserMove },
    { "xboard", xboardXboard }, 

    { "position-database", xboardPositionDatabase },

    { "", nullptr }
};

XBoardComm::XBoardComm()
{
    XBoardSearchEventHandler eventHandler;
    ChessPlayer::EventHandlerSharedPtr searchEventHandler = std::make_shared<XBoardSearchEventHandler>(eventHandler);

    this->player.addSearchEventHandler(searchEventHandler);

    this->addSearchAnalyzer();
}

void XBoardComm::addSearchAnalyzer()
{
    if (!this->hasAddedSearchAnalyzer) {
        ChessPlayer::EventHandlerSharedPtr searchEventHandler = std::make_shared<XBoardSearchAnalyzerSearchEventHandler>(this->searchAnalyzerEventHandler);

        this->player.addSearchEventHandler(searchEventHandler);

        this->hasAddedSearchAnalyzer = true;
    }
}

void XBoardComm::doPlayerMove(ChessMove& playerMove)
{
    this->player.doMove(playerMove);
}

Score XBoardComm::evaluateBoard()
{
    return this->player.evaluateCurrentPosition();
}

std::string XBoardComm::getCurrentBoardFen()
{
    return this->player.getCurrentBoardFen();
}

const ChessBoard XBoardComm::getPlayerBoard() const
{
    return this->player.getBoard();
}

Clock& XBoardComm::getPlayerClock()
{
    return this->player.getClock();
}

void XBoardComm::getPlayerMove(ChessMove& playerMove)
{
    this->player.getMove(playerMove);

    this->sideToMove = this->player.getCurrentBoard().sideToMove;
}

Color XBoardComm::getSideToMove() const
{
    return this->sideToMove;
}

bool XBoardComm::isForced() const
{
    return this->force;
}

void XBoardComm::loadPersonalityFile(const std::string& personalityFileName)
{
    std::fstream personalityFile;

    personalityFile.open(personalityFileName, std::fstream::ios_base::in);

    if (!personalityFile.is_open()) {
        std::printf("could not open personality file %s\n", personalityFileName.c_str());
        return;
    }

    while (!personalityFile.eof()) {
        std::string parameterName;
        Score parameterScore;

        personalityFile >> parameterName >> parameterScore;

        if (parameterName == "") {
            break;
        }

        this->setParameter(parameterName, parameterScore);
    }

    personalityFile.close();
}

NodeCount XBoardComm::perft(Depth depth)
{
    ChessBoard& board = this->player.getCurrentBoard();

    Perft perft;
    return perft.perft(board, depth, true);
}

void XBoardComm::processCommandImplementation(const std::string& cmd)
{
	const struct XBoardCommand* c = XBoardCommandList;

	std::string command;

	std::stringstream ss(cmd);
	ss >> command;

	while (c->function != nullptr) {
		if (command == c->command) {
			(*c->function)(this, ss);

			return;
		}

		c++;
	}

	std::cout << "Unknown Command: " << command << std::endl;
}

void XBoardComm::resetSpecificPosition(std::string& fen)
{
    this->player.resetSpecificPosition(fen);
}

void XBoardComm::resetStartingPosition()
{
    this->player.resetStartingPosition();
}

void XBoardComm::setForce(bool force)
{
    this->force = force;
}

void XBoardComm::setParameter(std::string& name, Score score)
{
    this->player.setParameter(name, score);
}

void XBoardComm::setResult(TwoPlayerGameResult result)
{
    this->searchAnalyzerEventHandler.setResult(result);
}

void XBoardComm::undoPlayerMove()
{
    this->player.undoMove();
}
