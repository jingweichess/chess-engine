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

#include "../../game/types/color.h"
#include "../../game/math/shift.h"

#include "../types/bitboard.h"
#include "../types/castlerights.h"
#include "../types/direction.h"
#include "../types/move.h"
#include "../types/piecetype.h"
#include "../types/square.h"

#include "../bitboards/moves.h"

class ChessBoardMover
{
public:
    constexpr ChessBoardMover() = default;
    constexpr ~ChessBoardMover() = default;

    constexpr void assertBoard(const ChessBoard& board) const
    {
        assert(board.materialEvaluation == board.calculateMaterialEvaluation());
        assert(board.pstEvaluation == board.calculatePstEvaluation());

        assert(board.hashValue == board.calculateHash());
        assert(board.materialHashValue == board.calculateMaterialHash());
        assert(board.pawnHashValue == board.calculatePawnHash());
    }

    template <bool performPreCalculations = true>
    constexpr void dispatchDoMove(ChessBoard& board, ChessMove& move) const
    {
        const bool isWhiteToMove = board.sideToMove == Color::WHITE;

        if (isWhiteToMove) {
            this->doMove<true, performPreCalculations>(board, move);
        }
        else {
            this->doMove<false, performPreCalculations>(board, move);
        }
    }

    template <bool isWhiteToMove, bool performPreCalculations = true>
    constexpr void doMove(ChessBoard& board, ChessMove& move) const
    {
        constexpr Color colorToMove = isWhiteToMove ? Color::WHITE : Color::BLACK;
        constexpr Color otherColor = isWhiteToMove ? Color::BLACK : Color::WHITE;

        const std::int32_t multiplier = isWhiteToMove ? 1 : -1;

        const Square& src = move.src;
        const Square& dst = move.dst;

        Bitboard* piecesToMove = isWhiteToMove ? board.whitePieces : board.blackPieces;
        Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        //1) If this is en passant, move the captured pawn back one piece
        const Square oldEnPassant = board.enPassant;
        const PieceType movingPiece = board.pieces[src];

        assert(movingPiece != PieceType::NO_PIECE);

        if (dst == board.enPassant
            && movingPiece == PAWN) {
            //This direction is the direction the captured pawn is to the destination en passant square
            constexpr Direction dir = isWhiteToMove ? Direction::DOWN : Direction::UP;

            otherPieces[PieceType::PAWN] = (otherPieces[PieceType::PAWN] ^ OneShiftedBy(dst + dir)) | OneShiftedBy(dst);
            otherPieces[PieceType::ALL] = (otherPieces[PieceType::ALL] ^ OneShiftedBy(dst + dir)) | OneShiftedBy(dst);

            board.pieces[dst] = PAWN;
            board.pieces[dst + dir] = PieceType::NO_PIECE;

            if (performPreCalculations) {
                board.hashValue ^= PieceHash(otherColor, PieceType::PAWN, dst);
                board.hashValue ^= PieceHash(otherColor, PieceType::PAWN, dst + dir);

                board.pawnHashValue ^= PieceHash(otherColor, PieceType::PAWN, dst);
                board.pawnHashValue ^= PieceHash(otherColor, PieceType::PAWN, dst + dir);

                if (isWhiteToMove) {
                    board.pstEvaluation += multiplier * PstParameters[PieceType::PAWN][FlipSquareOnHorizontalLine(dst + dir)];
                    board.pstEvaluation -= multiplier * PstParameters[PieceType::PAWN][FlipSquareOnHorizontalLine(dst)];
                }
                else {
                    board.pstEvaluation += multiplier * PstParameters[PieceType::PAWN][dst + dir];
                    board.pstEvaluation -= multiplier * PstParameters[PieceType::PAWN][dst];
                }
            }
        }

        //2) If this is a capture move, save the captured piece
        const PieceType capturedPiece = board.pieces[dst];
        move.capturedPiece = capturedPiece;

        assert(capturedPiece != PieceType::KING);

        //3) Do the actual move
        board.pieces[dst] = movingPiece;
        board.pieces[src] = PieceType::NO_PIECE;

        if (performPreCalculations) {
            if (isWhiteToMove) {
                board.pstEvaluation += multiplier * PstParameters[movingPiece][dst];
                board.pstEvaluation -= multiplier * PstParameters[movingPiece][src];
            }
            else {
                board.pstEvaluation += multiplier * PstParameters[movingPiece][FlipSquareOnHorizontalLine(dst)];
                board.pstEvaluation -= multiplier * PstParameters[movingPiece][FlipSquareOnHorizontalLine(src)];
            }

            board.hashValue ^= PieceHash(colorToMove, movingPiece, src);
            board.hashValue ^= PieceHash(colorToMove, movingPiece, dst);
        }

        piecesToMove[movingPiece] = (piecesToMove[movingPiece] ^ OneShiftedBy(src)) | OneShiftedBy(dst);
        piecesToMove[PieceType::ALL] = (piecesToMove[PieceType::ALL] ^ OneShiftedBy(src)) | OneShiftedBy(dst);

        //Reset the en_passant status.  If this is an en_passant move, it will be changed later
        board.enPassant = Square::NO_SQUARE;

        //4) Perform side effects from special moves
        const CastleRights oldCastleRights = board.castleRights;

        switch (movingPiece) {
        case PieceType::PAWN:
        {
            constexpr Direction dir = isWhiteToMove ? Direction::UP : Direction::DOWN;
            constexpr Direction twoDir = isWhiteToMove ? Direction::TWO_UP : Direction::TWO_DOWN;

            const Bitboard enPassantPieces = EnPassant[src] & otherPieces[PieceType::PAWN];
            if ((src + twoDir) == dst
                && enPassantPieces != EmptyBitboard) {
                board.enPassant = src + dir;
            }

            if (performPreCalculations) {
                board.pawnHashValue ^= PieceHash(colorToMove, PieceType::PAWN, src);
                board.pawnHashValue ^= PieceHash(colorToMove, PieceType::PAWN, dst);
            }
        }
        break;
        case PieceType::KNIGHT:
        case PieceType::BISHOP:
            break;
        case PieceType::ROOK:
            if constexpr (isWhiteToMove) {
                if (src == Square::A1) {
                    board.castleRights = (board.castleRights | CastleRights::WHITE_OOO) ^ CastleRights::WHITE_OOO;
                }
                else if (src == Square::H1) {
                    board.castleRights = (board.castleRights | CastleRights::WHITE_OO) ^ CastleRights::WHITE_OO;
                }
            }
            else {
                if (src == Square::A8) {
                    board.castleRights = (board.castleRights | CastleRights::BLACK_OOO) ^ CastleRights::BLACK_OOO;
                }
                else if (src == Square::H8) {
                    board.castleRights = (board.castleRights | CastleRights::BLACK_OO) ^ CastleRights::BLACK_OO;
                }
            }
            break;
        case PieceType::QUEEN:
            break;
        case PieceType::KING:
            if constexpr (isWhiteToMove) {
                board.castleRights &= CastleRights::BLACK_ALL;

                //4b) If this is a castle, move the associated rook and destroy castle rights for that side.
                if (src == Square::E1
                    && dst == Square::G1) {
                    board.pieces[Square::F1] = PieceType::ROOK;
                    board.pieces[Square::H1] = PieceType::NO_PIECE;

                    if (performPreCalculations) {
                        board.pstEvaluation += multiplier * PstParameters[PieceType::ROOK][Square::F1];
                        board.pstEvaluation -= multiplier * PstParameters[PieceType::ROOK][Square::H1];

                        board.hashValue ^= PieceHash(Color::WHITE, PieceType::ROOK, Square::F1);
                        board.hashValue ^= PieceHash(Color::WHITE, PieceType::ROOK, Square::H1);
                    }

                    piecesToMove[PieceType::ROOK] = (piecesToMove[PieceType::ROOK] ^ OneShiftedBy(Square::H1)) | OneShiftedBy(Square::F1);
                    piecesToMove[PieceType::ALL] = (piecesToMove[PieceType::ALL] ^ OneShiftedBy(Square::H1)) | OneShiftedBy(Square::F1);
                }
                else if (src == Square::E1
                    && dst == Square::C1) {
                    board.pieces[Square::D1] = PieceType::ROOK;
                    board.pieces[Square::A1] = PieceType::NO_PIECE;

                    if (performPreCalculations) {
                        board.pstEvaluation += multiplier * PstParameters[PieceType::ROOK][Square::D1];
                        board.pstEvaluation -= multiplier * PstParameters[PieceType::ROOK][Square::A1];

                        board.hashValue ^= PieceHash(Color::WHITE, PieceType::ROOK, Square::D1);
                        board.hashValue ^= PieceHash(Color::WHITE, PieceType::ROOK, Square::A1);
                    }

                    piecesToMove[PieceType::ROOK] = (piecesToMove[PieceType::ROOK] ^ OneShiftedBy(Square::A1)) | OneShiftedBy(Square::D1);
                    piecesToMove[PieceType::ALL] = (piecesToMove[PieceType::ALL] ^ OneShiftedBy(Square::A1)) | OneShiftedBy(Square::D1);
                }
            }
            else {
                board.castleRights &= CastleRights::WHITE_ALL;

                //4b) If this is a castle, move the associated rook and destroy castle rights for that side.
                if (src == Square::E8
                    && dst == Square::G8) {
                    board.pieces[Square::F8] = PieceType::ROOK;
                    board.pieces[Square::H8] = PieceType::NO_PIECE;

                    if (performPreCalculations) {
                        board.pstEvaluation += multiplier * PstParameters[PieceType::ROOK][FlipSquareOnHorizontalLine(Square::F8)];
                        board.pstEvaluation -= multiplier * PstParameters[PieceType::ROOK][FlipSquareOnHorizontalLine(Square::H8)];

                        board.hashValue ^= PieceHash(Color::BLACK, PieceType::ROOK, Square::F8);
                        board.hashValue ^= PieceHash(Color::BLACK, PieceType::ROOK, Square::H8);
                    }

                    piecesToMove[PieceType::ROOK] = (piecesToMove[PieceType::ROOK] ^ OneShiftedBy(Square::H8)) | OneShiftedBy(Square::F8);
                    piecesToMove[PieceType::ALL] = (piecesToMove[PieceType::ALL] ^ OneShiftedBy(Square::H8)) | OneShiftedBy(Square::F8);
                }
                else if (src == Square::E8
                    && dst == Square::C8) {
                    board.pieces[Square::D8] = PieceType::ROOK;
                    board.pieces[Square::A8] = PieceType::NO_PIECE;

                    if (performPreCalculations) {
                        board.pstEvaluation += multiplier * PstParameters[PieceType::ROOK][FlipSquareOnHorizontalLine(Square::D8)];
                        board.pstEvaluation -= multiplier * PstParameters[PieceType::ROOK][FlipSquareOnHorizontalLine(Square::A8)];

                        board.hashValue ^= PieceHash(Color::BLACK, PieceType::ROOK, Square::D8);
                        board.hashValue ^= PieceHash(Color::BLACK, PieceType::ROOK, Square::A8);
                    }

                    piecesToMove[PieceType::ROOK] = (piecesToMove[PieceType::ROOK] ^ OneShiftedBy(Square::A8)) | OneShiftedBy(Square::D8);
                    piecesToMove[PieceType::ALL] = (piecesToMove[PieceType::ALL] ^ OneShiftedBy(Square::A8)) | OneShiftedBy(Square::D8);
                }
            }
            break;
        default:
            assert(0);
        }

        //5) Change the capturedpiece bitboard
        if (capturedPiece != PieceType::NO_PIECE) {
            if (performPreCalculations) {
                board.materialEvaluation += multiplier * MaterialParameters[capturedPiece];

                std::uint32_t pieceTypeCount = std::popcount(otherPieces[capturedPiece]);
                board.materialHashValue ^= PieceHash(otherColor, capturedPiece, static_cast<Square>(pieceTypeCount)) ^ PieceHash(otherColor, capturedPiece, static_cast<Square>(pieceTypeCount - 1));

                if (isWhiteToMove) {
                    board.pstEvaluation += multiplier * PstParameters[capturedPiece][FlipSquareOnHorizontalLine(dst)];
                }
                else {
                    board.pstEvaluation += multiplier * PstParameters[capturedPiece][dst];
                }

                board.hashValue ^= PieceHash(otherColor, capturedPiece, dst);
            }

            otherPieces[capturedPiece] = otherPieces[capturedPiece] ^ OneShiftedBy(dst);
            otherPieces[PieceType::ALL] = otherPieces[PieceType::ALL] ^ OneShiftedBy(dst);

            switch (capturedPiece) {
            case PieceType::PAWN:
                if (performPreCalculations) {
                    board.pawnHashValue ^= PieceHash(otherColor, PieceType::PAWN, dst);
                }
                break;
            case PieceType::ROOK:
                if constexpr (isWhiteToMove) {
                    if (dst == Square::A8) {
                        board.castleRights = (board.castleRights | CastleRights::BLACK_OOO) ^ CastleRights::BLACK_OOO;
                    }
                    else if (dst == Square::H8) {
                        board.castleRights = (board.castleRights | CastleRights::BLACK_OO) ^ CastleRights::BLACK_OO;
                    }
                }
                else {
                    if (dst == Square::A1) {
                        board.castleRights = (board.castleRights | CastleRights::WHITE_OOO) ^ CastleRights::WHITE_OOO;
                    }
                    else if (dst == Square::H1) {
                        board.castleRights = (board.castleRights | CastleRights::WHITE_OO) ^ CastleRights::WHITE_OO;
                    }
                }
                break;
            }
        }

        //6) If this is a promotion, promote the moved pawn
        const PieceType promotionPiece = move.promotionPiece;
        if (movingPiece == PieceType::PAWN
            && promotionPiece != PieceType::NO_PIECE) {
            board.pieces[dst] = promotionPiece;

            if (performPreCalculations) {
                board.materialEvaluation += multiplier * MaterialParameters[promotionPiece];
                board.materialEvaluation -= multiplier * MaterialParameters[PieceType::PAWN];

                std::uint32_t pieceTypeCount = std::popcount(piecesToMove[promotionPiece]);
                board.materialHashValue ^= PieceHash(colorToMove, promotionPiece, static_cast<Square>(pieceTypeCount)) ^ PieceHash(colorToMove, promotionPiece, static_cast<Square>(pieceTypeCount + 1));

                pieceTypeCount = std::popcount(piecesToMove[PieceType::PAWN]);
                board.materialHashValue ^= PieceHash(colorToMove, PieceType::PAWN, static_cast<Square>(pieceTypeCount)) ^ PieceHash(colorToMove, PieceType::PAWN, static_cast<Square>(pieceTypeCount - 1));

                if (isWhiteToMove) {
                    board.pstEvaluation += multiplier * PstParameters[promotionPiece][dst];
                    board.pstEvaluation -= multiplier * PstParameters[PieceType::PAWN][dst];
                }
                else {
                    board.pstEvaluation += multiplier * PstParameters[promotionPiece][FlipSquareOnHorizontalLine(dst)];
                    board.pstEvaluation -= multiplier * PstParameters[PieceType::PAWN][FlipSquareOnHorizontalLine(dst)];
                }

                board.hashValue ^= PieceHash(colorToMove, PieceType::PAWN, dst);
                board.hashValue ^= PieceHash(colorToMove, promotionPiece, dst);

                board.pawnHashValue ^= PieceHash(colorToMove, PieceType::PAWN, dst);
            }

            piecesToMove[promotionPiece] = piecesToMove[promotionPiece] | OneShiftedBy(dst);
            piecesToMove[PieceType::PAWN] = piecesToMove[PieceType::PAWN] ^ OneShiftedBy(dst);
        }

        //7) Switch side to move
        board.sideToMove = ~board.sideToMove;

        if (performPreCalculations) {
            board.hashValue ^= WhiteToMoveHash;
        }

        if (board.sideToMove == Color::WHITE) {
            board.fullMoveCount++;
        }

        //8) Update miscellaneous thingies
        if (performPreCalculations) {
            if (oldEnPassant != Square::NO_SQUARE) {
                board.hashValue ^= EnPassantHash(oldEnPassant);
            }

            if (board.enPassant != Square::NO_SQUARE) {
                board.hashValue ^= EnPassantHash(board.enPassant);
            }

            if (board.castleRights != oldCastleRights) {
                board.hashValue ^= CastleRightsHash(oldCastleRights);
                board.hashValue ^= CastleRightsHash(board.castleRights);
            }
        }

        if (capturedPiece == PieceType::NO_PIECE
            && movingPiece != PieceType::PAWN) {
            board.fiftyMoveCount++;
        }
        else {
            board.fiftyMoveCount = 0;
        }

        //9) Set all pieces bitboard
        board.allPieces = board.whitePieces[PieceType::ALL] | board.blackPieces[PieceType::ALL];

        this->assertBoard(board);

        board.nullMove = false;
    }

    bool doNullMove(ChessBoard& board) const
    {
        board.hashValue ^= WhiteToMoveHash;
        board.sideToMove = ~board.sideToMove;

        if (board.enPassant != Square::NO_SQUARE) {
            board.hashValue ^= EnPassantHash(board.enPassant);

            board.enPassant = Square::NO_SQUARE;
        }

        this->assertBoard(board);

        board.nullMove = true;

        return true;
    }
};
