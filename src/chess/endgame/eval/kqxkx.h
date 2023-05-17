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

#include "../function.h"

constexpr ChessEndgame::EndgameFunctionType kqpkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqpkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqpkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqpkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqpkq = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqnkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqnkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqnkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqnkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqnkq = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqbkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqbkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqbkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqbkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqbkq = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqrkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqrkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqrkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqrkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqrkq = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqqkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqqkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqqkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqqkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqqkq = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqknp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqknn = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqkbp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkbn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkbb = weakKingEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqkrp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkrn = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType kqkrb = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType kqpkqp = weakKingDrawishEndgameFunction;
