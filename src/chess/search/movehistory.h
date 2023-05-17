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

#include "../../game/search/movehistory.h"

#include "../../game/types/hash.h"

#include "../board/board.h"

#include "../types/move.h"
#include "../types/piecetype.h"

struct ChessMoveHistoryStruct {
    Hash hashValue;
    PieceType capturedPiece;
    PieceType movedPiece;
};

class ChessMoveHistory : public MoveHistory<ChessMoveHistory, ChessBoard, ChessMoveHistoryStruct>
{
public:
    ChessMoveHistory() = default;
    ~ChessMoveHistory() = default;

    bool afterDuplicateHashCheckImplementation([[maybe_unused]] Hash hashValue, const MoveHistoryStructType& chessMoveHistoryStruct) const
    {
        if (chessMoveHistoryStruct.movedPiece == PieceType::PAWN) {
            return true;
        }

        if (chessMoveHistoryStruct.capturedPiece != PieceType::NO_PIECE) {
            return true;
        }

        return false;
    }

    bool beforeDuplicateHashCheckImplementation([[maybe_unused]] Hash hashValue, [[maybe_unused]] const MoveHistoryStructType& chessMoveHistoryStruct) const
    {
        return false;
    }

    void getNextMoveHistoryEntry(MoveHistoryStructType& moveHistoryStruct, const BoardType& board, const MoveType& move) const;
};
