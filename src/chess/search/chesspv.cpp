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

#include <iostream>

#include "chesspv.h"

#include "../types/file.h"
#include "../types/rank.h"
#include "../types/square.h"

//static const std::string FilePrint = "abcdefgh";
//static const std::string RankPrint = "87654321";

void ChessPrincipalVariation::printMoveToConsoleImplementation(const ChessMove& move) const
{
    const Square src = move.src;
    const Square dst = move.dst;

    const File srcFile = GetFile(src);
    const File dstFile = GetFile(dst);
    const Rank srcRank = GetRank(src);
    const Rank dstRank = GetRank(dst);

    const PieceType promotionPiece = move.promotionPiece;

    const char* file = FilePrintLowerCase.c_str();
    const char* rank = RankPrint.c_str();

    std::cout << file[srcFile] << rank[srcRank] << file[dstFile] << rank[dstRank];

    if (promotionPiece != PieceType::NO_PIECE) {
        const char* print = PiecePrint.c_str();

        std::cout << print[promotionPiece];
    }
}

void ChessPrincipalVariation::stringToMoveImplementation(const std::string& moveString, ChessMove& move) const
{
    const char* moveChars = moveString.c_str();

    File srcFile = static_cast<File>(FilePrintLowerCase.find(moveChars[0]));
    Rank srcRank = static_cast<Rank>(RankPrint.find(moveChars[1]));

    move.src = srcFile * srcRank;

    bool isCapture = moveChars[2] == 'x';

    File dstFile = static_cast<File>(FilePrintLowerCase.find(moveChars[isCapture ? 3 : 2]));
    Rank dstRank = static_cast<Rank>(RankPrint.find(moveChars[isCapture ? 4 : 3]));

    move.dst = dstFile * dstRank;
    
    if (char promotion = moveChars[isCapture ? 5 : 4]; 
        promotion != '\0') {
        promotion = static_cast<char>(std::tolower(promotion));

        move.promotionPiece = PieceType(PiecePrint.find(promotion));
    }
    else {
        move.promotionPiece = PieceType::NO_PIECE;
    }

    move.ordinal = ChessMoveOrdinal::NO_CHESS_MOVE_ORDINAL;
}
