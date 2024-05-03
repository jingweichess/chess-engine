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

#include "../function.h"

#include "kxxk.h"

constexpr ChessEndgame::EndgameFunctionType kbkpp = kbkp;
constexpr ChessEndgame::EndgameFunctionType kbkppp = kbkp;

constexpr ChessEndgame::EndgameFunctionType kbpkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpkb = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpkr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kbnkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbnkn = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbnkb = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbnkr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kbbkp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbbkn = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbbkb = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbbkr = weakKingDrawishEndgameFunction;

//TODO: kbppkb
constexpr ChessEndgame::EndgameFunctionType kbppkb = kbpk;

constexpr ChessEndgame::EndgameFunctionType kbpkbp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbppkbp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbppkbpp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpppkbp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpppkbpp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kbpppkbppp = weakKingDrawishEndgameFunction;
