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

#include <sstream>

#include "board.h"

#include "../types/direction.h"

const std::string startingPositionFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static const std::string PieceToChar = " PNBRQK  pnbrqk";

void ChessBoard::initFromFen(const std::string& fen)
{
    char token;
    Square src = Square::FIRST_SQUARE;
    PieceType piece;
    std::string enPassant;

    this->clearEverything();

    std::stringstream ss;
    ss.str(fen);

    ss >> std::noskipws;

    while ((ss >> token) && !isspace(token)) {
        if (isdigit(token)) {
            src = src + Direction::RIGHT * int(token - '0');
        }
        else if (token == '/') {
        }
        else if ((piece = PieceType(PieceToChar.find(token))) != std::string::npos) {
            Color color = isupper(token) ? Color::WHITE : Color::BLACK;
            piece = color == Color::WHITE ? PieceType(piece) : PieceType(piece - 8);

            this->pieces[src] = piece;

            if (color == Color::WHITE) {
                this->whitePieces[PieceType::ALL] |= src;
            }
            else {
                this->blackPieces[PieceType::ALL] |= src;
            }

            src++;
        }
    }

    ss >> token;
    this->sideToMove = token == 'w' ? Color::WHITE : Color::BLACK;
    ss >> token;

    this->castleRights = CastleRights::CASTLE_NONE;

    while ((ss >> token) && !isspace(token)) {
        switch (token) {
        case 'K':
            this->castleRights |= CastleRights::WHITE_OO;
            break;
        case 'Q':
            this->castleRights |= CastleRights::WHITE_OOO;
            break;
        case 'k':
            this->castleRights |= CastleRights::BLACK_OO;
            break;
        case 'q':
            this->castleRights |= CastleRights::BLACK_OOO;
            break;
        case '-':
            break;
        default:
            assert(0);
        }
    }

    ss >> enPassant;

    if (enPassant != "-") {
        this->enPassant = StringToSquare(enPassant);
    }
    else {
        this->enPassant = Square::NO_SQUARE;
    }

    ss >> std::skipws >> this->fiftyMoveCount >> this->fullMoveCount;

    this->buildBitboardsFromMailbox();

    this->materialEvaluation = this->calculateMaterialEvaluation();
    this->pstEvaluation = this->calculatePstEvaluation();

    this->hashValue = this->calculateHash();
    this->materialHashValue = this->calculateMaterialHash();
    this->pawnHashValue = this->calculatePawnHash();
}

void ChessBoard::resetSpecificPositionImplementation(const std::string& fen)
{
    this->initFromFen(fen);
}

void ChessBoard::resetStartingPositionImplementation()
{
    this->initFromFen(startingPositionFen);
}

std::string ChessBoard::saveToFen() const
{
    std::ostringstream ss;

    int emptySpaces = 0;
    Rank lastRank = Rank::_8;
    for (Square src = Square::FIRST_SQUARE; src < Square::SQUARE_COUNT; src++) {
        const PieceType currentPiece = this->pieces[src];
        const Rank currentRank = GetRank(src);

        if (currentRank != lastRank) {
            if (emptySpaces > 0) {
                ss << static_cast<char>('0' + emptySpaces);
                emptySpaces = 0;
            }

            ss << '/';
        }

        lastRank = currentRank;

        if (currentPiece == PieceType::NO_PIECE) {
            emptySpaces++;
            continue;
        }

        if (emptySpaces > 0) {
            ss << static_cast<char>('0' + emptySpaces);
            emptySpaces = 0;
        }

        if ((this->whitePieces[PieceType::ALL] & OneShiftedBy(src)) != EmptyBitboard) {
            ss << PieceToChar[currentPiece];
        }
        else {
            ss << PieceToChar[currentPiece + 8];
        }
    }

    if (emptySpaces > 0) {
        ss << static_cast<char>('0' + emptySpaces);
        emptySpaces = 0;
    }

    if (this->sideToMove == Color::WHITE) {
        ss << " w ";
    }
    else {
        ss << " b ";
    }

    if ((this->castleRights & CastleRights::WHITE_OO) != CastleRights::CASTLE_NONE) {
        ss << 'K';
    }

    if ((this->castleRights & CastleRights::WHITE_OOO) != CastleRights::CASTLE_NONE) {
        ss << 'Q';
    }

    if ((this->castleRights & CastleRights::BLACK_OO) != CastleRights::CASTLE_NONE) {
        ss << 'k';
    }

    if ((this->castleRights & CastleRights::BLACK_OOO) != CastleRights::CASTLE_NONE) {
        ss << 'q';
    }

    if (this->castleRights == CastleRights::CASTLE_NONE) {
        ss << '-';
    }

    ss << ' ';

    if (this->enPassant == Square::NO_SQUARE) {
        ss << '-';
    }
    else {
        static const std::string FilePrint = "abcdefgh";
        static const std::string RankPrint = "87654321";

        const File file = GetFile(this->enPassant);
        const Rank rank = GetRank(this->enPassant);

        ss << FilePrint[file] << RankPrint[rank];
    }

    ss << ' ' << this->fiftyMoveCount;

    ss << ' ' << this->fullMoveCount;

    return ss.str();
}
