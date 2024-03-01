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

#include "../bitboards/inbetween.h"
#include "../bitboards/magics.h"
#include "../bitboards/moves.h"

#include "../../game/math/bitscan.h"

#include "../types/attackboards.h"
#include "../types/bitboard.h"
#include "../types/direction.h"

class ChessAttackGenerator
{
public:
    constexpr ChessAttackGenerator() = default;
    constexpr ~ChessAttackGenerator() = default;

    template <bool isWhiteToMove>
    constexpr void buildAttackBoards(const ChessBoard& board, AttackBoards& attackBoards) const
    {
        const Bitboard piecesToMove = isWhiteToMove ? board.whitePieces[PieceType::ALL] : board.blackPieces[PieceType::ALL];
        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        const Bitboard kingPieces = isWhiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
        const Square kingPosition = BitScanForward<Square>(kingPieces);

        attackBoards.pinnedPieces = EmptyBitboard;
        attackBoards.blockedPieces = EmptyBitboard;
        attackBoards.inBetweenSquares = EmptyBitboard;

        //1) Check to see if a pawn or knight is doing attacking.  If so, enter them in the attack bitboard.
        const Bitboard pawnAttacks = (isWhiteToMove ? WhitePawnCaptures[kingPosition] : BlackPawnCaptures[kingPosition]) & otherPieces[PieceType::PAWN];
        const Bitboard knightAttacks = PieceMoves[PieceType::KNIGHT][kingPosition] & otherPieces[PieceType::KNIGHT];

        attackBoards.checkingPieces = pawnAttacks | knightAttacks;

        //2) Check to see if a bishop or queen (diagonally) is doing the attacking.
        const Bitboard bishopAttacks = PieceMoves[PieceType::BISHOP][kingPosition] & (otherPieces[PieceType::BISHOP] | otherPieces[PieceType::QUEEN]);

        //Scan through our Bishop attacks to see if any of them are not blocked
        for (const Square src : SquareBitboardIterator(bishopAttacks)) {
            const Bitboard inBetween = InBetween(kingPosition, src) & board.allPieces;
            attackBoards.inBetweenSquares |= InBetween(kingPosition, src);

            if (inBetween == EmptyBitboard) {
                attackBoards.checkingPieces |= OneShiftedBy(src);
            }
            else if (std::popcount(inBetween) == 1) {
                attackBoards.blockedPieces |= OneShiftedBy(src);
                attackBoards.pinnedPieces |= inBetween;
            }
        }

        //3) Check to see if a rook or queen (straight) is doing the attacking.
        const Bitboard rookAttacks = PieceMoves[PieceType::ROOK][kingPosition] & (otherPieces[PieceType::ROOK] | otherPieces[PieceType::QUEEN]);

        //Scan through our Rook attacks to see if any of them are not blocked
        for (const Square src : SquareBitboardIterator(rookAttacks)) {
            const Bitboard inBetween = InBetween(kingPosition, src) & board.allPieces;
            attackBoards.inBetweenSquares |= InBetween(kingPosition, src);

            if (inBetween == EmptyBitboard) {
                attackBoards.checkingPieces |= OneShiftedBy(src);
            }
            else if (std::popcount(inBetween) == 1) {
                attackBoards.blockedPieces |= OneShiftedBy(src);
                attackBoards.pinnedPieces |= inBetween;
            }
        }
    }

    template <bool isWhiteToMove>
    constexpr Bitboard getAttackingPieces(const ChessBoard& board, Square dst, bool earlyExit, Bitboard attackThrough = EmptyBitboard) const
    {
        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        const Bitboard attackingPawns = (isWhiteToMove ? WhitePawnCaptures[dst] : BlackPawnCaptures[dst]) & otherPieces[PieceType::PAWN];
        const Bitboard attackingKnights = PieceMoves[PieceType::KNIGHT][dst] & otherPieces[PieceType::KNIGHT];
        const Bitboard attackingKings = PieceMoves[PieceType::KING][dst] & otherPieces[PieceType::KING];

        Bitboard attackingPieces = attackingPawns | attackingKnights | attackingKings;

        if (earlyExit
            && attackingPieces != EmptyBitboard) {
            return attackingPieces;
        }

        const Bitboard effectiveAllPieces = board.allPieces & ~attackThrough;

        //2) Check to see if a bishop or queen (diagonally) is doing the attacking.
        const Bitboard bishops = otherPieces[PieceType::BISHOP] | otherPieces[PieceType::QUEEN];
        const Bitboard bishopAttacks = BishopMagic(dst, effectiveAllPieces) & bishops;

        //Scan through our Bishop attacks to see if any of them are not blocked
        attackingPieces |= bishopAttacks;

        //If we want to exit early, go ahead and exit if an attack has been found.  The caller is simply looking for any
        //	attack on this square, not necessarily all of them.
        if (earlyExit
            && attackingPieces != EmptyBitboard) {
            return attackingPieces;
        }

        //3) Check to see if a rook or queen (straight) is doing the attacking.
        const Bitboard rooks = otherPieces[PieceType::ROOK] | otherPieces[PieceType::QUEEN];
        const Bitboard rookAttacks = RookMagic(dst, effectiveAllPieces) & rooks;

        //Scan through our Rook attacks to see if any of them are not blocked
        attackingPieces |= rookAttacks;

        //4) Return the bitboard.
        return attackingPieces;
    }

    constexpr bool isInCheck(const AttackBoards& attackBoards) const
    {
        return attackBoards.checkingPieces != EmptyBitboard;
    }

    constexpr bool dispatchIsInCheck(const ChessBoard& board, bool otherSide = false) const
    {
        const bool isWhiteToMove = board.isWhiteToMove();

        if (isWhiteToMove) {
            return this->isInCheck<true>(board, otherSide);
        }
        else {
            return this->isInCheck<false>(board, otherSide);
        }
    }

    template <bool isWhiteToMove>
    constexpr bool isInCheck(const ChessBoard& board, bool otherSide = false) const
    {
        const Bitboard kingPieces = isWhiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
        const Square kingPosition = BitScanForward<Square>(kingPieces);

        const Bitboard* otherPieces = isWhiteToMove ? board.blackPieces : board.whitePieces;

        //This may be confusing at first.  These captures are compared against the opposite side pawns to see if they can "capture" the king
        const Bitboard ourPawnAttacks = isWhiteToMove ? WhitePawnCaptures[kingPosition] : BlackPawnCaptures[kingPosition];

        //Do an easy check to see if a knight is giving check
        if ((PieceMoves[PieceType::KNIGHT][kingPosition] & otherPieces[PieceType::KNIGHT]) != EmptyBitboard) {
            return true;
        }

        //Do an easy check to see if a pawn is giving check
        if ((ourPawnAttacks & otherPieces[PieceType::PAWN]) != EmptyBitboard) {
            return true;
        }

        //Sliding pieces are different.  We must test the bishop moves for bishop/queen checks and rook moves for rook/queen checks.
        const Bitboard bishops = otherPieces[PieceType::BISHOP] | otherPieces[PieceType::QUEEN];
        const Bitboard bishopMoves = BishopMagic(kingPosition, board.allPieces) & bishops;

        if (bishopMoves != EmptyBitboard) {
            return true;
        }

        const Bitboard rooks = otherPieces[PieceType::ROOK] | otherPieces[PieceType::QUEEN];
        const Bitboard rookMoves = RookMagic(kingPosition, board.allPieces) & rooks;

        if (rookMoves != EmptyBitboard) {
            return true;
        }

        return false;
    }

    template <bool isWhiteToMove>
    constexpr bool isSquareAttacked(const ChessBoard& board, Square dst, Bitboard attackThrough = EmptyBitboard) const
    {
        return this->getAttackingPieces<isWhiteToMove>(board, dst, true, attackThrough) != EmptyBitboard;
    }

    template <bool isWhiteToMove>
    constexpr Bitboard pawnAttacks(Bitboard pawns, Bitboard otherPieces) const
    {
        const Direction left = isWhiteToMove ? Direction::UP_LEFT : Direction::DOWN_LEFT;
        const Direction right = isWhiteToMove ? Direction::UP_RIGHT : Direction::DOWN_RIGHT;

        const Bitboard attacks = ((pawns & ~FileBitboard[File::_A]) + left)
            | ((pawns & ~FileBitboard[File::_H]) + right);

        return attacks & otherPieces;
    }

    template <bool isWhiteToMove>
    constexpr Bitboard pawnDefenders(Bitboard pawns, Bitboard pieces) const
    {
        const Direction left = isWhiteToMove ? Direction::DOWN_LEFT : Direction::UP_LEFT;
        const Direction right = isWhiteToMove ? Direction::DOWN_RIGHT : Direction::UP_RIGHT;

        const Bitboard attacks = ((pawns & ~FileBitboard[File::_A]) + left)
            | ((pawns & ~FileBitboard[File::_H]) + right);

        return attacks & pieces;
    }

    constexpr Bitboard unsafeSquares(Color color, const Bitboard* otherPieces) const
    {
        const bool colorIsWhite = color == Color::WHITE;

        const Direction left = colorIsWhite ? Direction::DOWN_LEFT : Direction::UP_LEFT;
        const Direction right = colorIsWhite ? Direction::DOWN_RIGHT : Direction::UP_RIGHT;

        return ((otherPieces[PieceType::PAWN] & ~FileBitboard[File::_A]) + left)
            | ((otherPieces[PieceType::PAWN] & ~FileBitboard[File::_H]) + right);
    }
};
