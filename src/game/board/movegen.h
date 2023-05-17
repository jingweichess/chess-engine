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

#include "board.h"

#include "../types/movelist.h"
#include "../types/nodecount.h"

template <class T, class Board>
class MoveGenerator
{
public:
	using BoardType = Board;
	using MoveType = typename BoardType::MoveType;

	constexpr MoveGenerator() = default;
	constexpr ~MoveGenerator() = default;

	constexpr NodeCount generateAllMoves(const BoardType& board, MoveList<MoveType>& moveList, bool countOnly = false) const
	{
		return static_cast<const T *>(this)->generateAllMovesImplementation(board, moveList, countOnly);
	}
};
