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

#include <array>
#include <string>

#include <cassert>

#include "../../game/board/board.h"

#include "../../game/math/bitreset.h"
#include "../../game/math/bitscan.h"
#include "../../game/math/popcount.h"

#include "../../game/types/color.h"
#include "../../game/types/hash.h"
#include "../../game/types/nodecount.h"
#include "../../game/types/result.h"

#include "../../game/types/bitboard.h"

#include "../bitboards/moves.h"
#include "../bitboards/inbetween.h"

#include "../hash/hash.h"

#include "../types/castlerights.h"
#include "../types/move.h"
#include "../types/nodetype.h"
#include "../types/piecetype.h"
#include "../types/score.h"

extern ChessEvaluation MaterialParameters[PieceType::PIECETYPE_COUNT];
extern ChessEvaluation PstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

class ChessBoard : public GameBoard<ChessBoard, ChessMove>
{
public:
    std::array<PieceType, Square::SQUARE_COUNT> pieces;

    Bitboard whitePieces[PieceType::PIECETYPE_COUNT];
    Bitboard blackPieces[PieceType::PIECETYPE_COUNT];

    ChessEvaluation materialEvaluation;
    ChessEvaluation pstEvaluation;

    Bitboard allPieces;

    Hash hashValue;
    Hash materialHashValue;
    Hash pawnHashValue;
    
    NodeCount fiftyMoveCount;
    NodeCount fullMoveCount;

    CastleRights castleRights;
    Color sideToMove;

    Square enPassant;

    bool nullMove;

    constexpr ChessBoard() = default;
	constexpr ~ChessBoard() = default;

    constexpr bool buildBitboardsFromMailbox()
    {
        //Clear all bitboards
        this->whitePieces[PieceType::PAWN] = this->whitePieces[PieceType::KNIGHT] = this->whitePieces[PieceType::BISHOP] = this->whitePieces[PieceType::ROOK] = this->whitePieces[PieceType::QUEEN] = this->whitePieces[PieceType::KING] = EmptyBitboard;
        this->blackPieces[PieceType::PAWN] = this->blackPieces[PieceType::KNIGHT] = this->blackPieces[PieceType::BISHOP] = this->blackPieces[PieceType::ROOK] = this->blackPieces[PieceType::QUEEN] = this->blackPieces[PieceType::KING] = EmptyBitboard;

        //Loop through mailbox building each individual bitboard
        for (const Square src : SquareIterator()) {
            if (this->pieces[src] == PieceType::NO_PIECE) { continue; }

            const Bitboard b = OneShiftedBy(src);
            const bool whitepiece = (this->whitePieces[PieceType::ALL] & b) != EmptyBitboard;

            const PieceType piece = this->pieces[src];

            if (whitepiece) {
                this->whitePieces[piece] |= b;
            }
            else {
                this->blackPieces[piece] |= b;
            }
        }

        //Build easy bitboards
        this->allPieces = this->whitePieces[PieceType::ALL] | this->blackPieces[PieceType::ALL];

        return true;
    }

    constexpr Hash calculateHash() const
    {
        Hash result = EmptyHash;

        for (Color color = Color::COLOR_START; color < Color::COLOR_COUNT; color++) {
            const bool whiteColor = color == Color::WHITE;

            const Bitboard* sideToMovePieces = whiteColor ? this->whitePieces : this->blackPieces;

            for (PieceType piece = PieceType::PAWN; piece < PieceType::ALL; piece++) {
                const Bitboard srcSquares = sideToMovePieces[piece];

                for (const Square src : SquareBitboardIterator(srcSquares)) {
                    result ^= PieceHash(color, piece, src);
                }
            }
        }

        result ^= CastleRightsHash(this->castleRights);

        const bool isWhiteToMove = this->isWhiteToMove();
        result ^= isWhiteToMove ? WhiteToMoveHash : EmptyHash;

        if (this->enPassant != Square::NO_SQUARE) {
            result ^= EnPassantHash(this->enPassant);
        }

        return result;
    }

    constexpr Hash calculateMaterialHash() const
    {
        Hash result = EmptyHash;

        for (Color color = Color::WHITE; color < Color::COLOR_COUNT; color++) {
            const bool whiteColor = color == Color::WHITE;

            const Bitboard* sideToMovePieces = whiteColor ? this->whitePieces : this->blackPieces;

            for (PieceType pieceType = PieceType::PAWN; pieceType <= PieceType::KING; pieceType++) {
                const std::uint32_t pieceTypeCount = std::popcount(sideToMovePieces[pieceType]);

                result ^= PieceHash(color, pieceType, static_cast<Square>(pieceTypeCount));
            }
        }

        return result;
    }

    constexpr ChessEvaluation calculateMaterialEvaluation() const
    {
        ChessEvaluation result = { ZERO_SCORE, ZERO_SCORE };

        for (PieceType piece = PieceType::PAWN; piece < PieceType::KING; piece++) {
            result += MaterialParameters[piece] * std::popcount(this->whitePieces[piece]);
            result -= MaterialParameters[piece] * std::popcount(this->blackPieces[piece]);
        }

        return result;
    }

    constexpr Hash calculatePawnHash() const
    {
        Hash result = EmptyHash;

        for (Color color = Color::COLOR_START; color < Color::COLOR_COUNT; color++) {
            const bool whiteColor = color == Color::WHITE;

            const Bitboard* sideToMovePieces = whiteColor ? this->whitePieces : this->blackPieces;

            const Bitboard srcSquares = sideToMovePieces[PieceType::PAWN];

            for (const Square src : SquareBitboardIterator(srcSquares)) {
                result ^= PieceHash(color, PieceType::PAWN, src);
            }
        }

        return result;
    }

    constexpr ChessEvaluation calculatePstEvaluation() const
    {
        ChessEvaluation result = { ZERO_SCORE, ZERO_SCORE };

        for (Color color = Color::COLOR_START; color < Color::COLOR_COUNT; color++) {
            const bool whiteColor = color == Color::WHITE;
            const std::int32_t multiplier = whiteColor ? 1 : -1;

            const Bitboard* sideToMovePieces = whiteColor ? this->whitePieces : this->blackPieces;

            for (PieceType piece = PieceType::PAWN; piece < PieceType::ALL; piece++) {
                const Bitboard srcSquares = sideToMovePieces[piece];

                for (const Square src : SquareBitboardIterator(srcSquares)) {
                    const Square evaluatedSrc = whiteColor ? src : FlipSquareOnHorizontalLine(src);

                    result += multiplier * PstParameters[piece][evaluatedSrc];
                }
            }
        }

        return result;
    }

    constexpr void clearEverything()
    {
        for (const Square src : SquareIterator()) {
            this->pieces[src] = PieceType::NO_PIECE;
        }

        for (PieceType piece = PieceType::NO_PIECE; piece <= PieceType::ALL; piece++) {
            this->whitePieces[piece] = EmptyBitboard;
            this->blackPieces[piece] = EmptyBitboard;
        }

        this->allPieces = EmptyBitboard;

        this->sideToMove = Color::WHITE;
        this->castleRights = CastleRights::CASTLE_ALL;
        this->enPassant = Square::NO_SQUARE;
        this->fiftyMoveCount = 0;
        this->fullMoveCount = 1;

        this->hashValue = EmptyHash;
        this->materialHashValue = EmptyHash;
        this->pawnHashValue = EmptyHash;

        this->materialEvaluation = { ZERO_SCORE, ZERO_SCORE };
        this->pstEvaluation = { ZERO_SCORE, ZERO_SCORE };
    }

    constexpr bool hasMadeNullMove() const
    {
        return this->nullMove;
    }

    constexpr bool hasNonPawnMaterial() const
    {
        const bool colorIsWhite = this->sideToMove == Color::WHITE;
        const Bitboard* piecesToMove = colorIsWhite ? this->whitePieces : this->blackPieces;

        const Bitboard pawnsAndKing = piecesToMove[PieceType::PAWN] | piecesToMove[PieceType::KING];
        return pawnsAndKing != piecesToMove[PieceType::ALL];
    }

    void initFromFen(const std::string& fen);

    constexpr bool isWhiteToMove() const
    {
        return this->sideToMove == Color::WHITE;
    }

    void resetSpecificPositionImplementation(const std::string& fen);
    void resetStartingPositionImplementation();

    std::string saveToFen() const;

    constexpr std::uint32_t getPhase() const
    {
        return std::popcount(this->allPieces);
    }

    constexpr std::uint32_t getPieceCount() const
    {
        return std::popcount(this->allPieces);
    }

    constexpr Square blackKingPosition() const
    {
        const Bitboard kingPieces = this->blackPieces[PieceType::KING];
        return BitScanForward<Square>(kingPieces);
    }

    constexpr Square otherKingPosition(Color color) const
    {
        return (color == Color::WHITE) ? this->blackKingPosition() : this->whiteKingPosition();
    }

    constexpr Square whiteKingPosition() const
    {
        const Bitboard kingPieces = this->whitePieces[PieceType::KING];
        return BitScanForward<Square>(kingPieces);
    }
};
