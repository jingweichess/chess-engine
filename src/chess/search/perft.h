#pragma once

#include "../board/board.h"
#include "../board/boardmover.h"
#include "../board/movegenerator.h"

#include "../types/attackboards.h"
#include "../types/move.h"

#include "../../game/types/depth.h"
#include "../../game/types/nodecount.h"

//static const std::string FilePrint = "abcdefgh";
//static const std::string PiecePrint = ".pnbrqk";
//static const std::string RankPrint = "87654321";
//
//static void PrintMoveToConsole(const ChessMove& move)
//{
//    const Square src = move.src;
//    const Square dst = move.dst;
//
//    const File srcFile = GetFile(src);
//    const File dstFile = GetFile(dst);
//    const Rank srcRank = GetRank(src);
//    const Rank dstRank = GetRank(dst);
//
//    const PieceType promotionPiece = move.promotionPiece;
//
//    const char* file = FilePrint.c_str();
//    const char* rank = RankPrint.c_str();
//
//    std::cout << file[srcFile] << rank[srcRank] << file[dstFile] << rank[dstRank];
//
//    if (promotionPiece != PieceType::NO_PIECE) {
//        const char* print = PiecePrint.c_str();
//
//        std::cout << print[promotionPiece];
//    }
//}

static std::array<ChessBoard, Depth::MAX> chessBoardArray;
static std::array<ChessMoveList, Depth::MAX> chessMoveListArray;

class Perft
{
protected:
    const ChessBoardMover chessBoardMover;
    const ChessMoveGenerator moveGenerator;
public:
    constexpr Perft() = default;
    constexpr ~Perft() = default;

    constexpr NodeCount perft(const ChessBoard& board, Depth maxDepth, bool split = false) const
    {
        const bool whiteToMove = board.sideToMove == Color::WHITE;

        if (maxDepth == Depth::ONE) {
            ChessMoveList& moveList = chessMoveListArray[Depth::ZERO];

            if (whiteToMove) {
                this->moveGenerator.generateAllMoves<true>(board, moveList);
            }
            else {
                this->moveGenerator.generateAllMoves<false>(board, moveList);
            }

            if (split) {
                for (const ChessMove& move : moveList) {
                    //PrintMoveToConsole(move);

                    std::cout << std::endl;
                }
            }

            return moveList.size();
        }

        if (whiteToMove) {
            if (split) {
                return this->perft<true, true>(board, maxDepth - Depth::ONE);
            }
            else {
                return this->perft<true, false>(board, maxDepth - Depth::ONE);
            }
        }
        else {
            if (split) {
                return this->perft<false, true>(board, maxDepth - Depth::ONE);
            }
            else {
                return this->perft<false, false>(board, maxDepth - Depth::ONE);
            }
        }
    }

    template <bool whiteToMove, bool split = false>
    constexpr NodeCount perft(const ChessBoard& board, Depth depthLeft) const
    {
        ChessMoveList& moveList = chessMoveListArray[depthLeft];
        this->moveGenerator.generateAllMoves<whiteToMove>(board, moveList);

        NodeCount result = ZeroNodes;

        for (const ChessMove& move : moveList) {
            ChessBoard& currentBoard = chessBoardArray[depthLeft];

            currentBoard = board;
            this->chessBoardMover.doMove<whiteToMove>(currentBoard, move);

            if (split) {
                //PrintMoveToConsole(move);
                std::cout << ": ";
            }

            NodeCount nodeCount;

            if (depthLeft == Depth::ONE) {
                //If we're at the end of depthLeft, avoid a function call to perft again.
                ChessMoveList& moveList = chessMoveListArray[Depth::ZERO];
                nodeCount = this->moveGenerator.generateAllMoves<!whiteToMove, false, true>(currentBoard, moveList);
            }
            else {
                nodeCount = this->perft<!whiteToMove, false>(currentBoard, depthLeft - Depth::ONE);
            }

            if (split) {
                std::cout << nodeCount << std::endl;
            }

            result += nodeCount;
        }

        return result;
    }
};
