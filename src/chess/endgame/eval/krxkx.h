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

constexpr ChessEndgame::EndgameFunctionType krpkp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krpkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krpkb = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krpkr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krnkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krnkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krnkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krnkr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krbkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krbkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krbkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krbkr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krrbkrr = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krrkp = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krrkn = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krrkb = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krrkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krrkq = weakKingDrawishEndgameFunction;

constexpr ChessEndgame::EndgameFunctionType krpkrp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krppkr = weakKingEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krppkrp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krppkrpp = weakKingDrawishEndgameFunction;
constexpr ChessEndgame::EndgameFunctionType krpppkrpp = weakKingDrawishEndgameFunction;
