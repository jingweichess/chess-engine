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

#include <array>

#include "../../game/types/color.h"
#include "../../game/types/hash.h"

#include "../types/castlerights.h"
#include "../types/piecetype.h"
#include "../types/square.h"

extern std::array<Hash, CastleRights::CASTLERIGHTS_COUNT> CastleRightsHashList;
extern std::array<Hash, Square::SQUARE_COUNT> EnPassantHashList;
extern std::array<std::array<std::array<Hash, Square::SQUARE_COUNT>, PieceType::PIECETYPE_COUNT>, Color::COLOR_COUNT> PieceHashList;

constexpr Hash CastleRightsHash(CastleRights castleRights)
{
    return CastleRightsHashList[castleRights];
}

constexpr Hash EnPassantHash(Square src)
{
    return EnPassantHashList[src];
}

constexpr Hash PieceHash(Color color, PieceType pieceType, Square src)
{
    return PieceHashList[color][pieceType][src];
}

constexpr Hash WhiteToMoveHash = 0xa2856ba3032c76e8;
