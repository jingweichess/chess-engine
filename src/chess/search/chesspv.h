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

#include <fstream>

#include "../../game/search/pv.h"

#include "../types/move.h"

class ChessPrincipalVariation : public PrincipalVariation<ChessPrincipalVariation, ChessMove>
{
public:
	constexpr ChessPrincipalVariation() = default;
    constexpr ~ChessPrincipalVariation() = default;

    void printToFile(std::ofstream& file) const
    {
        for (const MoveType& move : this->moveList) {

            const Square src = move.src;
            const Square dst = move.dst;

            const std::string moveAsString = SquareToString(src) + SquareToString(dst);

            file << moveAsString;

            const PieceType promotionPiece = move.promotionPiece;

            if (promotionPiece != PieceType::NO_PIECE) {
                const char* print = PiecePrint.c_str();

                file << print[promotionPiece];
            }

            file << ' ';
        }

    }

	void printMoveToConsoleImplementation(const ChessMove& move) const;
	void stringToMoveImplementation(const std::string& moveString, ChessMove& move) const;
};
