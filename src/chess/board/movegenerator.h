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
#include <iostream>
#include <vector>

#include <cassert>

#include "../../game/types/depth.h"
#include "../types/move.h"
#include "../types/piecetype.h"
#include "../types/square.h"

#include "attackgenerator.h"
#include "board.h"

#include "../bitboards/magics.h"

#include "../bitboards/moves.h"

#include "../../game/math/bitscan.h"

class ChessMoveGenerator
{
protected:
    const ChessAttackGenerator attackGenerator;

    template <bool whiteToMove, bool checkBackRank, bool countOnly = false>
    constexpr NodeCount insertPawnMoves(const ChessBoard& board, ChessMoveList& moveList, Square src, Square dst) const
    {
        constexpr Rank backRank = whiteToMove ? Rank::_8 : Rank::_1;

        NodeCount result = ZeroNodes;

        if (checkBackRank
            && GetRank(dst) == backRank) {
            if (!countOnly) {
                moveList.push_back({ src, dst, PieceType::QUEEN });
                moveList.push_back({ src, dst, PieceType::ROOK });
                moveList.push_back({ src, dst, PieceType::BISHOP });
                moveList.push_back({ src, dst, PieceType::KNIGHT });
            }

            result += 4;
        }
        else {
            if (!countOnly) {
                moveList.push_back({ src, dst });
            }

            result++;
        }

        return result;
    }
public:
    constexpr ChessMoveGenerator() = default;
    constexpr ~ChessMoveGenerator() = default;

    constexpr NodeCount DispatchGenerateAllCaptures(const ChessBoard& board, ChessMoveList& moveList) const
    {
        const bool whiteToMove = board.sideToMove == Color::WHITE;

        if (whiteToMove) {
            return this->generateAllCaptures<true>(board, moveList);
        }
        else {
            return this->generateAllCaptures<false>(board, moveList);
        }
    }

    constexpr NodeCount DispatchGenerateAllMoves(const ChessBoard& board, ChessMoveList& moveList) const
    {
        const bool whiteToMove = board.sideToMove == Color::WHITE;

        if (whiteToMove) {
            return this->generateAllMoves<true>(board, moveList);
        }
        else {
            return this->generateAllMoves<false>(board, moveList);
        }
    }

    template <bool whiteToMove>
    constexpr NodeCount generateAllCaptures(const ChessBoard& board, ChessMoveList& moveList) const
    {
        AttackBoards attackBoards;
        this->attackGenerator.buildAttackBoards<whiteToMove>(board, attackBoards);

        return this->generateAllCaptures<whiteToMove>(board, moveList, attackBoards);
    }

    template <bool whiteToMove>
    constexpr NodeCount generateAllCaptures(const ChessBoard& board, ChessMoveList& moveList, const AttackBoards& attackBoards) const
    {
        return this->generateAllMoves<whiteToMove, true>(board, moveList, attackBoards);
    }

    template <bool whiteToMove, bool capturesOnly = false, bool countOnly = false>
    constexpr NodeCount generateAllMoves(const ChessBoard& board, ChessMoveList& moveList) const
    {
        AttackBoards attackBoards;
        this->attackGenerator.buildAttackBoards<whiteToMove>(board, attackBoards);

        return this->generateAllMoves<whiteToMove, capturesOnly, countOnly>(board, moveList, attackBoards);
    }

    template <bool whiteToMove, bool capturesOnly = false, bool countOnly = false>
    constexpr NodeCount generateAllMoves(const ChessBoard& board, ChessMoveList& moveList, const AttackBoards& attackBoards) const
    {
        moveList.clear();
        NodeCount result = ZeroNodes;

        //1) If the side to move is in check, there's an optimized algorithm for generating just evasions
        if (this->attackGenerator.isInCheck(attackBoards)) {
            return this->generateCheckEvasions<whiteToMove>(board, attackBoards, moveList);
        }

        const Bitboard* piecesToMove = whiteToMove ? board.whitePieces : board.blackPieces;

        if (piecesToMove[PieceType::PAWN] != EmptyBitboard) {
            result += this->generateMovesForPawns<whiteToMove, capturesOnly, countOnly>(board, attackBoards, moveList);
        }

        if (piecesToMove[PieceType::KNIGHT] != EmptyBitboard) {
            result += this->generateMovesForPieceType<whiteToMove, PieceType::KNIGHT, capturesOnly, countOnly>(board, attackBoards, moveList);
        }

        if (piecesToMove[PieceType::BISHOP] != EmptyBitboard) {
            result += this->generateMovesForPieceType<whiteToMove, PieceType::BISHOP, capturesOnly, countOnly>(board, attackBoards, moveList);
        }

        if (piecesToMove[PieceType::ROOK] != EmptyBitboard) {
            result += this->generateMovesForPieceType<whiteToMove, PieceType::ROOK, capturesOnly, countOnly>(board, attackBoards, moveList);
        }

        if (piecesToMove[PieceType::QUEEN] != EmptyBitboard) {
            result += this->generateMovesForPieceType<whiteToMove, PieceType::QUEEN, capturesOnly, countOnly>(board, attackBoards, moveList);
        }

        result += this->generateMovesForKing<whiteToMove, capturesOnly, false, countOnly>(board, moveList);

        return result;
    }

    template <bool whiteToMove>
    constexpr NodeCount generateAttacksOnSquares(const ChessBoard& board, ChessMoveList& moveList, Bitboard dstSquares, Bitboard excludeSrcSquares) const
    {
        const Bitboard* piecesToMove = whiteToMove ? board.whitePieces : board.blackPieces;

        //REMEMBER: Here, we're scanning backwards for moves!  We're scanning from the destination to the source rather than
        //	from the source to the destination
        for (const Square dst : SquareBitboardIterator(dstSquares)) {
            for (PieceType piece = PieceType::PAWN; piece <= PieceType::KING; piece++) {
                Bitboard srcSquares;

                switch (piece) {
                case PAWN:
                    srcSquares = whiteToMove ? BlackPawnCaptures[dst] : WhitePawnCaptures[dst];
                    break;
                case PieceType::KNIGHT:
                    srcSquares = PieceMoves[piece][dst];
                    break;
                case PieceType::BISHOP:
                    srcSquares = BishopMagic(dst, board.allPieces);
                    break;
                case PieceType::ROOK:
                    srcSquares = RookMagic(dst, board.allPieces);
                    break;
                case PieceType::QUEEN:
                    srcSquares = QueenMagic(dst, board.allPieces);
                    break;
                case PieceType::KING:
                    srcSquares = PieceMoves[piece][dst];
                    break;
                default:
                    assert(0);
                }

                const Bitboard includeSrcSquares = ~excludeSrcSquares;
                srcSquares &= piecesToMove[piece] & includeSrcSquares;

                for (const Square src : SquareBitboardIterator(srcSquares)) {
                    if (piece == PieceType::PAWN) {
                        this->insertPawnMoves<whiteToMove, true>(board, moveList, src, dst);
                    }
                    else {
                        moveList.push_back({ src, dst });
                    }
                }
            }
        }

        return moveList.size();
    }

    template <bool whiteToMove>
    constexpr NodeCount generateMovesToSquares(const ChessBoard& board, ChessMoveList& moveList, Bitboard dstSquares, Bitboard excludeSrcSquares) const
    {
        const Bitboard* piecesToMove = whiteToMove ? board.whitePieces : board.blackPieces;
        const Bitboard* otherPieces = whiteToMove ? board.blackPieces : board.whitePieces;

        //REMEMBER: Here, we're scanning backwards for moves!  We're scanning from the destination to the source rather than
        //	from the source to the destination
        for (const Square dst : SquareBitboardIterator(dstSquares)) {
            for (PieceType piece = PieceType::PAWN; piece <= PieceType::KING; piece++) {
                Bitboard srcSquares = EmptyBitboard;

                switch (piece) {
                case PieceType::PAWN:
                {
                    const Bitboard pawnCaptures = whiteToMove ? BlackPawnCaptures[dst] : WhitePawnCaptures[dst];
                    const Direction dir = whiteToMove ? Direction::DOWN : Direction::UP;
                    const Direction dir2 = whiteToMove ? Direction::TWO_DOWN : Direction::TWO_UP;

                    //Since we're in reverse, we want the black pawn captures from the destination square to the white pawns
                    srcSquares = (otherPieces[PieceType::ALL] & OneShiftedBy(dst)) & (pawnCaptures & piecesToMove[PieceType::PAWN]);

                    if (whiteToMove) {
                        //We cannot, however, generate pawn moves in the same manner.
                        if (GetRank(dst) == Rank::_4) {
                            srcSquares |= OneShiftedBy(dst + dir) | OneShiftedBy(dst + dir2);
                        }
                        else if (GetRank(dst) == Rank::_1) {
                            //If the destination square is on Rank 1, a white pawn cannot move to it.
                        }
                        else {
                            srcSquares |= OneShiftedBy(dst + dir);
                        }
                    }
                    else {
                        //We cannot, however, generate pawn moves in the same manner.
                        if (GetRank(dst) == Rank::_5) {
                            srcSquares |= OneShiftedBy(dst + dir) | OneShiftedBy(dst + dir2);
                        }
                        else if (GetRank(dst) == Rank::_8) {
                            //If the destination square is on Rank 8, a black pawn cannot move to it.
                        }
                        else {
                            srcSquares |= OneShiftedBy(dst + dir);
                        }
                    }
                    break;
                }
                case PieceType::KNIGHT:
                    srcSquares = PieceMoves[piece][dst];
                    break;
                case PieceType::BISHOP:
                    srcSquares = BishopMagic(dst, board.allPieces);
                    break;
                case PieceType::ROOK:
                    srcSquares = RookMagic(dst, board.allPieces);
                    break;
                case PieceType::QUEEN:
                    srcSquares = QueenMagic(dst, board.allPieces);
                    break;
                case PieceType::KING:
                    srcSquares = PieceMoves[piece][dst];
                    break;
                default:
                    assert(0);
                }

                const Bitboard includeSrcSquares = ~excludeSrcSquares;
                srcSquares &= piecesToMove[piece] & includeSrcSquares;

                for (const Square src : SquareBitboardIterator(srcSquares)) {
                    //If there's actually one of our pieces at the source, and nothing in between, allow the move
                    if (piece == PAWN) {
                        if ((InBetween(src, dst) & board.allPieces) == EmptyBitboard) {
                            this->insertPawnMoves<whiteToMove, true>(board, moveList, src, dst);
                        }
                    }
                    else {
                        moveList.push_back({ src, dst });
                    }
                }
            }
        }

        return moveList.size();
    }

    template <bool whiteToMove>
    constexpr NodeCount generateCheckEvasions(const ChessBoard& board, const AttackBoards& attackBoards, ChessMoveList& moveList) const
    {
        const Bitboard kingPieces = whiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
        const Square kingPosition = BitScanForward<Square>(kingPieces);

        const Bitboard* piecesToMove = whiteToMove ? board.whitePieces : board.blackPieces;

        //1) Add all king moves which evade check.
        this->generateMovesForKing<whiteToMove, false, true>(board, moveList);

        //2) Find which pieces are attacking the king.  If two pieces are checking the king, return the current list since
        //	only moves which move the king himself are able to evade two checking pieces.
        if (std::popcount(attackBoards.checkingPieces) == 2) {
            return moveList.size();
        }

        //3) From here, we know that only one piece is checking; Find it.  Add moves which capture the piece doing the checking,
        // except for king captures.  Those have already been generated.
        const Square checkingPosition = BitScanForward<Square>(attackBoards.checkingPieces);

        //Generate the pawn attacks which capture the en_passant pawn
        //The pawn cannot be captured unless
        if (board.enPassant != Square::NO_SQUARE) {
            const Direction dir = whiteToMove ? Direction::DOWN : Direction::UP;

            //Ensure the pawn being captured is actually the piece doing the checking
            if ((OneShiftedBy(board.enPassant + dir) & attackBoards.checkingPieces) != EmptyBitboard) {
                //The positions from which pawns can capture onto the en passant square
                const Bitboard pawnCaptures = whiteToMove ? BlackPawnCaptures[board.enPassant] : WhitePawnCaptures[board.enPassant];
                //Ensure the moving pawns are not pinned
                const Bitboard srcPawns = pawnCaptures & piecesToMove[PieceType::PAWN] & ~attackBoards.pinnedPieces;

                for (const Square src : SquareBitboardIterator(srcPawns)) {
                    moveList.push_back({ src, board.enPassant });
                }
            }
        }

        //If the checking piece is a pawn or a knight, or the checking piece was next to the king, then it cannot be blocked.
        //	Return only moves which can attack the piece.
        const Bitboard& ourKing = whiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];

        if (board.pieces[checkingPosition] <= KNIGHT
            || (PieceMoves[PieceType::KING][kingPosition] & attackBoards.checkingPieces) != EmptyBitboard) {
            this->generateAttacksOnSquares<whiteToMove>(board, moveList, attackBoards.checkingPieces, ourKing | attackBoards.pinnedPieces);

            return moveList.size();
        }

        //If there's no way to block the attacking piece, simply return
        if (attackBoards.inBetweenSquares == EmptyBitboard) {
            return moveList.size();
        }

        //4) Generate all moves which either attack the checking piece, or block it
        this->generateAttacksOnSquares<whiteToMove>(board, moveList, attackBoards.checkingPieces, ourKing | attackBoards.pinnedPieces);

        //A piece that is already pinned cannot block another piece by moving, so exclude them
        const Bitboard inBetweenSquares = InBetween(kingPosition, checkingPosition);
        this->generateMovesToSquares<whiteToMove>(board, moveList, inBetweenSquares, ourKing | attackBoards.pinnedPieces);

        return moveList.size();
    }

    template <bool whiteToMove, bool capturesOnly = false, bool isInCheck = false, bool countOnly = false>
    constexpr NodeCount generateMovesForKing(const ChessBoard& board, ChessMoveList& moveList, Bitboard dstSquares = FullBitboard) const
    {
        const Bitboard piecesToMove = whiteToMove ? board.whitePieces[PieceType::ALL] : board.blackPieces[PieceType::ALL];
        const Bitboard otherPieces = whiteToMove ? board.blackPieces[PieceType::ALL] : board.whitePieces[PieceType::ALL];

        const Bitboard kingPieces = whiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
        const Square kingPosition = BitScanForward<Square>(kingPieces);

        Bitboard dstMoves = PieceMoves[PieceType::KING][kingPosition];

        if (capturesOnly) {
            //Only generate captures of other pieces
            dstMoves &= otherPieces;
        }
        else {
            //Ensure we can't capture our own pieces
            dstMoves &= ~piecesToMove;
        }

        //Only generate moves to the destination squares
        dstMoves &= dstSquares;

        NodeCount result = ZeroNodes;

        for (const Square dst : SquareBitboardIterator(dstMoves)) {
            if (!this->attackGenerator.isSquareAttacked<whiteToMove>(board, dst, kingPieces)) {
                if (!countOnly) {
                    moveList.push_back({ kingPosition, dst });
                }

                result++;
            }
        }

        //Special castle processing here
        if (!isInCheck
            && kingPosition == (whiteToMove ? Square::E1 : Square::E8)) {
            if (whiteToMove) {
                //Check castling rights and open availability
                if ((board.castleRights & CastleRights::WHITE_OOO) != CastleRights::CASTLE_NONE) {
                    //If all of the needed spaces are empty, and we're not moving THROUGH check...
                    if ((board.allPieces & 0x0e00000000000000ull) == EmptyBitboard
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::D1)
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::C1)) {
                        //We will test to see if we're put INTO check later...
                        if (!countOnly) {
                            moveList.push_back({ kingPosition, Square::C1 });
                        }

                        result++;
                    }
                }
                if ((board.castleRights & CastleRights::WHITE_OO) != CastleRights::CASTLE_NONE) {
                    if ((board.allPieces & 0x6000000000000000ull) == EmptyBitboard
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::F1)
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::G1)) {
                        if (!countOnly) {
                            moveList.push_back({ kingPosition, Square::G1 });
                        }

                        result++;
                    }
                }
            }
            else {
                //Check castling rights and open availability
                if ((board.castleRights & CastleRights::BLACK_OOO) != CastleRights::CASTLE_NONE) {
                    if ((board.allPieces & 0x000000000000000eull) == EmptyBitboard
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::D8)
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::C8)) {
                        if (!countOnly) {
                            moveList.push_back({ kingPosition, Square::C8 });
                        }

                        result++;
                    }
                }
                if ((board.castleRights & CastleRights::BLACK_OO) != CastleRights::CASTLE_NONE) {
                    if ((board.allPieces & 0x0000000000000060ull) == EmptyBitboard
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::F8)
                        && !this->attackGenerator.isSquareAttacked<whiteToMove>(board, Square::G8)) {
                        if (!countOnly) {
                            moveList.push_back({ kingPosition, Square::G8 });
                        }

                        result++;
                    }
                }
            }
        }

        return result;
    }

    template <bool whiteToMove, bool capturesOnly = false, bool countOnly = false>
    constexpr NodeCount generateMovesForPawns(const ChessBoard& board, const AttackBoards& attackBoards, ChessMoveList& moveList) const
    {
        constexpr Rank backRank = whiteToMove ? Rank::_8 : Rank::_1;
        constexpr Rank thirdRank = whiteToMove ? Rank::_3 : Rank::_6;

        const Bitboard pawnsToMove = whiteToMove ? board.whitePieces[PieceType::PAWN] : board.blackPieces[PieceType::PAWN];

        const Bitboard otherPieces = (whiteToMove ? board.blackPieces : board.whitePieces)[PieceType::ALL];

        const Bitboard* pawnCaptures = whiteToMove ? WhitePawnCaptures : BlackPawnCaptures;

        const Bitboard kingPieces = whiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
        const Square kingPosition = BitScanForward<Square>(kingPieces);

        //1) Get Pawn Captures
        const Bitboard captures = this->attackGenerator.pawnAttacks<whiteToMove>(pawnsToMove, otherPieces);
        const Bitboard attackers = this->attackGenerator.pawnDefenders<whiteToMove>(captures, pawnsToMove);

        NodeCount result = ZeroNodes;

        for (const Square src : SquareBitboardIterator(attackers)) {
            Bitboard dstMoves = pawnCaptures[src] & captures;

            if (attackBoards.pinnedPieces != EmptyBitboard
                && (attackBoards.pinnedPieces & OneShiftedBy(src)) != EmptyBitboard) {
                const Bitboard blockedPieces = attackBoards.blockedPieces;

                for (const Square blockedPieceSrc : SquareBitboardIterator(blockedPieces)) {
                    const Bitboard inBetween = InBetween(blockedPieceSrc, kingPosition);
                    if ((inBetween & OneShiftedBy(src)) != EmptyBitboard) {
                        dstMoves &= InBetween(blockedPieceSrc, kingPosition) | OneShiftedBy(blockedPieceSrc);
                        break;
                    }
                }
            }

            for (const Square dst : SquareBitboardIterator(dstMoves)) {
                result += this->insertPawnMoves<whiteToMove, true, countOnly>(board, moveList, src, dst);
            }
        }

        //Special en passant processing here
        if (board.enPassant != Square::NO_SQUARE) {
            const Bitboard* backwardsPawnCaptures = whiteToMove ? BlackPawnCaptures : WhitePawnCaptures;
            const Bitboard enPassantPawns = backwardsPawnCaptures[board.enPassant] & pawnsToMove;

            for (const Square src : SquareBitboardIterator(enPassantPawns)) {
                //If the source pawn is identified as pinned, it can only move along an "in between" line from another
                //  piece and the enemy king.
                if (attackBoards.pinnedPieces != EmptyBitboard
                    && (attackBoards.pinnedPieces & OneShiftedBy(src)) != EmptyBitboard) {
                    for (const Square blockedPieceSrc : SquareBitboardIterator(attackBoards.blockedPieces)) {
                        const Bitboard inBetween = InBetween(blockedPieceSrc, kingPosition);
                        if ((inBetween & OneShiftedBy(src)) != EmptyBitboard
                            && (inBetween & OneShiftedBy(board.enPassant)) != EmptyBitboard) {
                            result += this->insertPawnMoves<whiteToMove, false, countOnly>(board, moveList, src, board.enPassant);
                            break;
                        }
                    }

                    continue;
                }

                constexpr Rank exceptionalKingRank = whiteToMove ? Rank::_5 : Rank::_4;
                constexpr Bitboard exceptionalKingRankBitboard = RankBitboard[exceptionalKingRank];

                //If the king to move is not on the same Rank as the src pawn, the pawn can safely take the en passant.
                if ((kingPieces & exceptionalKingRankBitboard) == EmptyBitboard) {
                    result += this->insertPawnMoves<whiteToMove, false, countOnly>(board, moveList, src, board.enPassant);
                    continue;
                }

                //If the king to move is on the same Rank as the src pawn, but there aren't any enemy kings or rooks,
                //  then we can safely take the en passant.
                const Bitboard* other = whiteToMove ? board.blackPieces : board.whitePieces;
                Bitboard otherRooksAndQueens = (other[PieceType::ROOK] | other[PieceType::QUEEN]) & exceptionalKingRankBitboard;

                if (otherRooksAndQueens == EmptyBitboard) {
                    result += this->insertPawnMoves<whiteToMove, false, countOnly>(board, moveList, src, board.enPassant);
                    continue;
                }

                //If we got here, then this is actually pretty complicated.  The King on the side to move is on the
                // same Rank as the en passant pawn doing the capturing and the en passant pawn being captured.  We
                // need to make sure there's not a hidden pin on those two pieces because pin detection does not detect
                // this situation.

                //Foreach enemy rook or queen (there may be more than one) on this Rank, ensure that either
                //  a) The two pawns in question are not the only pieces between the given rook or queen and the king
                //  b) There is a third piece in between the given rook or queen and the king
                for (const Square rookOrQueenSrc : SquareBitboardIterator(otherRooksAndQueens)) {
                    const Bitboard inBetween = InBetween(rookOrQueenSrc, kingPosition);
                    if ((inBetween & OneShiftedBy(src)) == EmptyBitboard
                        || std::popcount(inBetween & board.allPieces) > 2) {

                        //If either a or b are true, this rook or queen will not attack the king and does not pin the
                        //  pawns.
                        otherRooksAndQueens ^= OneShiftedBy(rookOrQueenSrc);
                    }
                    else {
                        //We found a two piece pin situation; we cannot take the en passant.  Continue to the outer
                        //  loop.
                        break;

                        //Waiting for something like this feature in Php to hit C++
                        //continue 2;
                    }
                }

                //If all rooks and queens in question are of either a or b condition, then this move is valid.
                if (otherRooksAndQueens == EmptyBitboard) {
                    result += this->insertPawnMoves<whiteToMove, false, countOnly>(board, moveList, src, board.enPassant);
                }
            }
        }

        //If we're generating captures only, return
        if (capturesOnly) {
            return result;
        }

        //2) Get Pawn Pushes
        const Direction pushDirection = whiteToMove ? Direction::UP : Direction::DOWN;

        const Bitboard pawnPushes = (pawnsToMove + pushDirection) & ~board.allPieces;
        const Bitboard pushers = pawnPushes - pushDirection;

        for (const Square src : SquareBitboardIterator(pushers)) {
            Bitboard dstMoves = OneShiftedBy(src + pushDirection);

            if (attackBoards.pinnedPieces != EmptyBitboard
                && (attackBoards.pinnedPieces & OneShiftedBy(src)) != EmptyBitboard) {
                for (const Square blockedPieceSrc : SquareBitboardIterator(attackBoards.blockedPieces)) {
                    const Bitboard inBetween = InBetween(blockedPieceSrc, kingPosition);
                    if ((inBetween & OneShiftedBy(src)) != EmptyBitboard) {
                        dstMoves &= InBetween(blockedPieceSrc, kingPosition) | OneShiftedBy(blockedPieceSrc);
                        break;
                    }
                }
            }

            if (dstMoves != EmptyBitboard) {
                result += this->insertPawnMoves<whiteToMove, true, countOnly>(board, moveList, src, src + pushDirection);
            }
        }

        //3) Get Double Pushes
        const Bitboard doublePushes = ((pawnPushes & RankBitboard[thirdRank]) + pushDirection) & ~board.allPieces;
        const Bitboard doublePushers = doublePushes - pushDirection;

        for (const Square src : SquareBitboardIterator(doublePushers)) {
            Bitboard dstMoves = OneShiftedBy(src + pushDirection);

            if (attackBoards.pinnedPieces != EmptyBitboard
                && (attackBoards.pinnedPieces & OneShiftedBy(src - pushDirection)) != EmptyBitboard) {
                for (const Square blockedPieceSrc : SquareBitboardIterator(attackBoards.blockedPieces)) {
                    const Bitboard inBetween = InBetween(blockedPieceSrc, kingPosition);
                    if ((inBetween & OneShiftedBy(src - pushDirection)) != EmptyBitboard) {
                        dstMoves &= InBetween(blockedPieceSrc, kingPosition) | OneShiftedBy(blockedPieceSrc);
                        break;
                    }
                }
            }

            if (dstMoves != EmptyBitboard) {
                result += this->insertPawnMoves<whiteToMove, false, countOnly>(board, moveList, src - pushDirection, src + pushDirection);
            }
        }

        return result;
    }

    template <bool whiteToMove, PieceType pieceType, bool capturesOnly = false, bool countOnly = false>
    constexpr NodeCount generateMovesForPieceType(const ChessBoard& board, const AttackBoards& attackBoards, ChessMoveList& moveList) const
    {
        const Bitboard piecesToMove = whiteToMove ? board.whitePieces[PieceType::ALL] : board.blackPieces[PieceType::ALL];
        const Bitboard otherPieces = whiteToMove ? board.blackPieces[PieceType::ALL] : board.whitePieces[PieceType::ALL];

        const Bitboard srcPieces = whiteToMove ? board.whitePieces[pieceType] : board.blackPieces[pieceType];

        NodeCount result = ZeroNodes;

        for (const Square src : SquareBitboardIterator(srcPieces)) {
            Bitboard dstMoves = EmptyBitboard;

            switch (pieceType) {
            case PieceType::KNIGHT:
                dstMoves = PieceMoves[PieceType::KNIGHT][src];
                break;
            case PieceType::BISHOP:
                dstMoves = BishopMagic(src, board.allPieces);
                break;
            case PieceType::ROOK:
                dstMoves = RookMagic(src, board.allPieces);
                break;
            case PieceType::QUEEN:
                dstMoves = QueenMagic(src, board.allPieces);
                break;
            default:
                assert(0);
            }

            if (capturesOnly) {
                //Only generate captures of other pieces
                dstMoves &= otherPieces;
            }
            else {
                //Ensure we can't capture our own pieces
                dstMoves &= ~piecesToMove;
            }

            //If this piece is pinned, it's destination moves can only be other squares in between attackers (in this case, blocked pieces)
            if (attackBoards.pinnedPieces != EmptyBitboard
                && (attackBoards.pinnedPieces & OneShiftedBy(src)) != EmptyBitboard) {
                //If knights are pinned, they cannot move.
                if (pieceType == PieceType::KNIGHT) {
                    continue;
                }

                const Bitboard kingPieces = whiteToMove ? board.whitePieces[PieceType::KING] : board.blackPieces[PieceType::KING];
                const Square kingPosition = BitScanForward<Square>(kingPieces);

                for (const Square blockedPieceSrc : SquareBitboardIterator(attackBoards.blockedPieces)) {
                    const Bitboard inBetween = InBetween(blockedPieceSrc, kingPosition);
                    if ((inBetween & OneShiftedBy(src)) != EmptyBitboard) {
                        dstMoves &= InBetween(blockedPieceSrc, kingPosition) | OneShiftedBy(blockedPieceSrc);
                        break;
                    }
                }
            }

            result += std::popcount(dstMoves);

            if (!countOnly) {
                for (const Square dst : SquareBitboardIterator(dstMoves)) {
                    moveList.push_back({ src, dst });
                }
            }
        }

        return result;
    }
};
