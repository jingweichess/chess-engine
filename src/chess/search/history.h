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

#include <cstdint>

#include "../../game/search/historytable.h"

#include "../types/piecetype.h"
#include "../types/square.h"

#include "../eval/constructor.h"

using PieceTypeSquareHistoryTable = HistoryTable<PieceType::PIECETYPE_COUNT, Square::SQUARE_COUNT>;
using SquareSquareHistoryTable = HistoryTable<Square::SQUARE_COUNT, Square::SQUARE_COUNT>;

//extern QuadraticConstruct HistoryDeltaByDepthLeft;
//
//constexpr std::uint32_t HistoryDelta(Depth depthLeft, std::uint32_t phase)
//{
//    return HistoryDeltaByDepthLeft(depthLeft)(phase);
//}
