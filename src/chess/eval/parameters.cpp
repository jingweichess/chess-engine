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

#include <cmath>

#include "../../game/personality/parametermap.h"

#include "../../game/types/color.h"
#include "../../game/types/score.h"

#include "../eval/constructor.h"

#include "../types/piecetype.h"
#include "../types/score.h"
#include "../types/square.h"

#include "parameters.h"

QuadraticConstruct LateMoveReductionsSearchedMoves = {
    {  78,  21 },
    { 244,  44 },
    {  37, 130 }
};

QuadraticConstruct PruningMarginDepthLeft = {
    {   50,  49 },
    { -145, -18 },
    {  129, 151 }
};

QuadraticConstruct PruningMarginSearchedMoves = {
    {  -28,  -34 },
    {   54, -137 },
    {  143,   63 }
};

//Parameters used by Evaluation
ChessEvaluation MaterialParameters[PieceType::PIECETYPE_COUNT] = {
    {},
    {   220,   282 },
    {   838,   891 },
    {   998,   875 },
    {  1219,  1628 },
    {  2560,  3101 },
};

ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { 0, 1 },
    { 44, 88 },
    { 0, 0 },
    { 0, 0 },
};

ChessEvaluation DoubledRooks = { -20, 47 };
ChessEvaluation EmptyFileRook = { 0, 0 };

ChessEvaluation PassedPawnDefended = { 0, -20 };

std::array<ChessEvaluation, 2> BishopPawns = { {
    { 0, 0 },   //Same color pawns
    { 0, 0 }    //Opposite color opponent pawns
} };

std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> KingAttacks = { {
    {}, {},
    { 0, 0 },   //KNIGHT
    { 0, 0 },   //BISHOP
    { 0, 0 },   //ROOK
    { -37, 131 }    //QUEEN
} };

std::array<ChessEvaluation, 2> KingShield = { {
    { 55, -39 },
    {  5, -29 }
} };

std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> Outpost = { {
    {}, {},
    { 0, 0 },   //KNIGHT
    { 0, 0 },   //BISHOP
    { 0, 0 },   //ROOK
    { 0, 0 }    //QUEEN
} };

std::array<ChessEvaluation, PieceType::PIECETYPE_COUNT> PassedPawnBlockedByPiece = { {
    {}, {},
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
} };

ChessEvaluation Tempo = { 15, 0 };

ChessEvaluation PstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

//Attack Parameters aren't "built" so they're hard coded here
ChessEvaluation AttackParameters[PieceType::PIECETYPE_COUNT][PieceType::PIECETYPE_COUNT] = {
    {},   //PAWN          KNIGHT          BISHOP          ROOK            QUEEN          is attacked by...
    { {}, {},             {  126,  206 }, {   84,  264 }, {  168,  173 }, {  102,  137 }, },    //PAWN
    { {}, {  -30,   56 }, {},             {   57,  102 }, {  159,  163 }, {   77,  163 }, },    //KNIGHT
    { {}, {   -7,   56 }, {   57,  113 }, {},             {  142,  153 }, {  132,  138 }, },    //BISHOP
    { {}, {  -14,   63 }, {   79,   71 }, {   64,   67 }, {},             {  174,  185 }, },    //ROOK
    { {}, {  -11,   37 }, {   13,  -37 }, {   11,   68 }, {   -9,   72 }, {}, },                //QUEEN
};

ChessEvaluation MobilityParameters[PieceType::PIECETYPE_COUNT][32];
ChessEvaluation TropismParameters[PieceType::PIECETYPE_COUNT][16];

ChessEvaluation PawnChainBack = { 32, 14 };
ChessEvaluation PawnChainFront = { 33, 9 };

ChessEvaluation PawnChainBackPerRank = { -1, -13 };
ChessEvaluation PawnChainFrontPerRank = { -1, 15 };

ChessEvaluation PawnDoubledByRank[Rank::RANK_COUNT] = {
    {}, {},           { -16,  47 }, {  23,  -1 }, { -18,  31 }, { -15, -32 }, { -36,   3 }, {}
};

ChessEvaluation PawnPassedByRank[Rank::RANK_COUNT] = {
    {}, { -68, 193 }, {  16, 111 }, { 117, 169 }, {  53,  91 }, { -33,  -8 }, {  88, -34 }, {}
};

ChessEvaluation PawnPhalanxByRank[Rank::RANK_COUNT] = {
    {}, { -19, -80 }, {   0,  36 }, {  36,  59 }, {  66,  59 }, {  23,  49 }, {  19,  69 }, {}
};

ChessEvaluation PstByPieceAndFile[PieceType::PIECETYPE_COUNT][File::FILE_COUNT] = {
    {},
    {
        {   0,   0 }, {   0,   0 }, {   0,  0 }, {  0,  0 }
    },
    {
        {  24,   0 }, {  12,   0 }, {  66,  0 }, { 56,  0 }
    },
    {
        { -57,  13 }, {  37, -56 }, {   7,  0 }, {  6,  0 }
    },
    {
        {  9, 29 }, {  9, -9 }, { 40, -4 }, { 14,  7 }
    },
    {
        { -26,  54 }, {  -5, -45 }, {  19,  45 }, {  36,  42 }
    },
    {
        {  -5, -72 }, {  81, -24 }, { -46, 43 }, { -49,  30 }
    }
};

ChessEvaluation PstByPieceAndRank[PieceType::PIECETYPE_COUNT][Rank::RANK_COUNT] = {
    {},
    {
        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {
        { -40,   0 }, { -60,   0 }, {   0,  -4 }, {  51,  24 }, {  46,  28 }, {  22,  -9 }, {  20, -20 }, { -17, -17 }
    },
    {
        {-103, 0 }, {-122, 0 }, {   8, 0 }, { -41, 0 }, { -24, 0 }, {   5, 0 }, {  15, 0 }, {  -5, 0 }
    },
    {
        {  12,   0 }, {  62,  36 }, {  -2,  25 }, {  31,   6 }, { -20,  42 }, { -19, -11 }, { -32,  23 }, {  20, -60 }
    },
    {
        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {
        { -20, -48 }, {  45, -26 }, {   1,   2 }, {   6,  48 }, {  40,  25 }, {  39,   7 }, { -29,   2 }, { -21, -74 }
    }
};

ChessEvaluation PawnPst[Square::SQUARE_COUNT] = {
    {   0,    0 }, {    0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {    0,   0 }, {    0,   0 },
    {  105, 352 }, {   88, 356 }, { 126, 304 }, { 146, 281 }, { 144, 274 }, { 103, 291 }, {   41, 333 }, {   18, 374 },
    {  -26, 240 }, {  -13, 230 }, {  24, 197 }, {  46, 193 }, {  49, 190 }, {  39, 186 }, {    7, 230 }, {  -21, 233 },
    {  -67, 152 }, {  -48, 137 }, { -18, 103 }, {  12,  85 }, {  13,  90 }, {  -7,  93 }, {  -38, 133 }, {  -61, 145 },
    {  -90, 121 }, {  -85, 128 }, { -37,  78 }, { -18,  75 }, { -17,  75 }, { -40,  83 }, {  -71, 119 }, {  -93, 119 },
    { -132, 143 }, { -118, 141 }, { -89, 102 }, { -62, 102 }, { -61, 102 }, { -86, 108 }, { -111, 142 }, { -130, 138 },
    {  -84, 133 }, {  -69, 132 }, { -36,  92 }, { -16,  84 }, { -12,  96 }, { -25,  97 }, {  -50, 127 }, {  -82, 123 },
    {    0,   0 }, {    0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {    0,   0 }, {    0,   0 },
};

QuadraticParameterConstructorSet MobilityConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { //KNIGHT
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { -6, -84 }, { 131, 455 }, { -51, -84 } },
    },
    { //BISHOP
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 0, -52 }, { 157, 551 }, { -73, -183 } },
    },
    { //ROOK
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 27, -29 }, { -52, 274 }, { -70, -115 } },
    },
    { //QUEEN
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 1, -19 }, { 65, 367 }, { -128, -328 } },
    }
};

QuadraticParameterConstructorSet TropismConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { //KNIGHT
        .quadraticBase = { {    0,    0 }, {  106,  -36 }, {   41,   41 }, {   52,   79 }, {    6,  110 }, {    2,   59 }, {    3,  -12 }, {  -36,  -48 }, {  -49,  -85 }, { -127, -107 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //BISHOP
        .quadraticBase = { {    0,    0 }, {  171, -137 }, {   81,  -42 }, {   12,    5 }, {  -28,   38 }, {  -39,   31 }, {  -47,   32 }, {  -57,   40 }, {  -41,  -16 }, {  -49,   46 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //ROOK
        .quadraticBase = {},
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //QUEEN
        .quadraticBase = {},
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    }
};

std::uint32_t Distance[File::FILE_COUNT][Rank::RANK_COUNT];

extern Evaluation AspirationWindowInitialValue;
extern Evaluation AspirationWindowDelta;

ParameterMap chessEngineParameterMap = {
    { "material-pawn-mg", &MaterialParameters[PieceType::PAWN].mg },
    { "material-pawn-eg", &MaterialParameters[PieceType::PAWN].eg },
    { "material-knight-mg", &MaterialParameters[PieceType::KNIGHT].mg },
    { "material-knight-eg", &MaterialParameters[PieceType::KNIGHT].eg },
    { "material-bishop-mg", &MaterialParameters[PieceType::BISHOP].mg },
    { "material-bishop-eg", &MaterialParameters[PieceType::BISHOP].eg },
    { "material-rook-mg", &MaterialParameters[PieceType::ROOK].mg },
    { "material-rook-eg", &MaterialParameters[PieceType::ROOK].eg },
    { "material-queen-mg", &MaterialParameters[PieceType::QUEEN].mg },
    { "material-queen-eg", &MaterialParameters[PieceType::QUEEN].eg },

    { "material-knight-pair-mg", &PiecePairs[PieceType::KNIGHT].mg},
    { "material-knight-pair-eg", &PiecePairs[PieceType::KNIGHT].eg},
    { "material-bishop-pair-mg", &PiecePairs[PieceType::BISHOP].mg},
    { "material-bishop-pair-eg", &PiecePairs[PieceType::BISHOP].eg},
    { "material-rook-pair-mg", &PiecePairs[PieceType::ROOK].mg},
    { "material-rook-pair-eg", &PiecePairs[PieceType::ROOK].eg},
    { "material-queen-pair-mg", &PiecePairs[PieceType::QUEEN].mg},
    { "material-queen-pair-eg", &PiecePairs[PieceType::QUEEN].eg},

    { "pst-queen-file-a-mg", &PstByPieceAndFile[PieceType::QUEEN][File::_A].mg },
    { "pst-queen-file-a-eg", &PstByPieceAndFile[PieceType::QUEEN][File::_A].eg },
    { "pst-queen-file-b-mg", &PstByPieceAndFile[PieceType::QUEEN][File::_B].mg },
    { "pst-queen-file-b-eg", &PstByPieceAndFile[PieceType::QUEEN][File::_B].eg },
    { "pst-queen-file-c-mg", &PstByPieceAndFile[PieceType::QUEEN][File::_C].mg },
    { "pst-queen-file-c-eg", &PstByPieceAndFile[PieceType::QUEEN][File::_C].eg },
    { "pst-queen-file-d-mg", &PstByPieceAndFile[PieceType::QUEEN][File::_D].mg },
    { "pst-queen-file-d-eg", &PstByPieceAndFile[PieceType::QUEEN][File::_D].eg },

    { "pst-queen-rank-8-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_8].mg },
    { "pst-queen-rank-8-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_8].eg },
    { "pst-queen-rank-7-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_7].mg },
    { "pst-queen-rank-7-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_7].eg },
    { "pst-queen-rank-6-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_6].mg },
    { "pst-queen-rank-6-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_6].eg },
    { "pst-queen-rank-5-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_5].mg },
    { "pst-queen-rank-5-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_5].eg },
    { "pst-queen-rank-4-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_4].mg },
    { "pst-queen-rank-4-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_4].eg },
    { "pst-queen-rank-3-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_3].mg },
    { "pst-queen-rank-3-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_3].eg },
    { "pst-queen-rank-2-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_2].mg },
    { "pst-queen-rank-2-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_2].eg },
    { "pst-queen-rank-1-mg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_1].mg },
    { "pst-queen-rank-1-eg", &PstByPieceAndRank[PieceType::QUEEN][Rank::_1].eg },

    { "pawn-chain-back-default-mg", &PawnChainBack.mg },
    { "pawn-chain-back-default-eg", &PawnChainBack.eg },

    { "pawn-chain-front-default-mg", &PawnChainFront.mg },
    { "pawn-chain-front-default-eg", &PawnChainFront.eg },

    { "pawn-chain-back-per-rank-mg", &PawnChainBackPerRank.mg },
    { "pawn-chain-back-per-rank-eg", &PawnChainBackPerRank.eg },

    { "pawn-chain-front-per-rank-mg", &PawnChainFrontPerRank.mg },
    { "pawn-chain-front-per-rank-eg", &PawnChainFrontPerRank.eg },

    { "pawn-doubled-rank-2-mg", &PawnDoubledByRank[Rank::_2].mg },
    { "pawn-doubled-rank-2-eg", &PawnDoubledByRank[Rank::_2].eg },
    { "pawn-doubled-rank-3-mg", &PawnDoubledByRank[Rank::_3].mg },
    { "pawn-doubled-rank-3-eg", &PawnDoubledByRank[Rank::_3].eg },
    { "pawn-doubled-rank-4-mg", &PawnDoubledByRank[Rank::_4].mg },
    { "pawn-doubled-rank-4-eg", &PawnDoubledByRank[Rank::_4].eg },
    { "pawn-doubled-rank-5-mg", &PawnDoubledByRank[Rank::_5].mg },
    { "pawn-doubled-rank-5-eg", &PawnDoubledByRank[Rank::_5].eg },
    { "pawn-doubled-rank-6-mg", &PawnDoubledByRank[Rank::_6].mg },
    { "pawn-doubled-rank-6-eg", &PawnDoubledByRank[Rank::_6].eg },

    { "pawn-passed-defended-mg", &PassedPawnDefended.mg },
    { "pawn-passed-defended-eg", &PassedPawnDefended.eg },

    { "pawn-passed-rank-2-mg", &PawnPassedByRank[Rank::_2].mg },
    { "pawn-passed-rank-2-eg", &PawnPassedByRank[Rank::_2].eg },
    { "pawn-passed-rank-3-mg", &PawnPassedByRank[Rank::_3].mg },
    { "pawn-passed-rank-3-eg", &PawnPassedByRank[Rank::_3].eg },
    { "pawn-passed-rank-4-mg", &PawnPassedByRank[Rank::_4].mg },
    { "pawn-passed-rank-4-eg", &PawnPassedByRank[Rank::_4].eg },
    { "pawn-passed-rank-5-mg", &PawnPassedByRank[Rank::_5].mg },
    { "pawn-passed-rank-5-eg", &PawnPassedByRank[Rank::_5].eg },
    { "pawn-passed-rank-6-mg", &PawnPassedByRank[Rank::_6].mg },
    { "pawn-passed-rank-6-eg", &PawnPassedByRank[Rank::_6].eg },
    { "pawn-passed-rank-7-mg", &PawnPassedByRank[Rank::_7].mg },
    { "pawn-passed-rank-7-eg", &PawnPassedByRank[Rank::_7].eg },

    { "pawn-phalanx-rank-2-mg", &PawnPhalanxByRank[Rank::_2].mg },
    { "pawn-phalanx-rank-2-eg", &PawnPhalanxByRank[Rank::_2].eg },
    { "pawn-phalanx-rank-3-mg", &PawnPhalanxByRank[Rank::_3].mg },
    { "pawn-phalanx-rank-3-eg", &PawnPhalanxByRank[Rank::_3].eg },
    { "pawn-phalanx-rank-4-mg", &PawnPhalanxByRank[Rank::_4].mg },
    { "pawn-phalanx-rank-4-eg", &PawnPhalanxByRank[Rank::_4].eg },
    { "pawn-phalanx-rank-5-mg", &PawnPhalanxByRank[Rank::_5].mg },
    { "pawn-phalanx-rank-5-eg", &PawnPhalanxByRank[Rank::_5].eg },
    { "pawn-phalanx-rank-6-mg", &PawnPhalanxByRank[Rank::_6].mg },
    { "pawn-phalanx-rank-6-eg", &PawnPhalanxByRank[Rank::_6].eg },
    { "pawn-phalanx-rank-7-mg", &PawnPhalanxByRank[Rank::_7].mg },
    { "pawn-phalanx-rank-7-eg", &PawnPhalanxByRank[Rank::_7].eg },

    { "mobility-knight-quadratic-mg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.quadratic.mg },
    { "mobility-knight-quadratic-eg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.quadratic.eg },
    { "mobility-knight-slope-mg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.slope.mg },
    { "mobility-knight-slope-eg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.slope.eg },
    { "mobility-knight-yintercept-mg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.yintercept.mg },
    { "mobility-knight-yintercept-eg", &MobilityConstructorSet[PieceType::KNIGHT].quadraticConstruct.yintercept.eg },

    { "mobility-bishop-quadratic-mg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.quadratic.mg },
    { "mobility-bishop-quadratic-eg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.quadratic.eg },
    { "mobility-bishop-slope-mg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.slope.mg },
    { "mobility-bishop-slope-eg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.slope.eg },
    { "mobility-bishop-yintercept-mg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.yintercept.mg },
    { "mobility-bishop-yintercept-eg", &MobilityConstructorSet[PieceType::BISHOP].quadraticConstruct.yintercept.eg },

    { "mobility-rook-quadratic-mg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.quadratic.mg },
    { "mobility-rook-quadratic-eg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.quadratic.eg },
    { "mobility-rook-slope-mg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.slope.mg },
    { "mobility-rook-slope-eg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.slope.eg },
    { "mobility-rook-yintercept-mg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.yintercept.mg },
    { "mobility-rook-yintercept-eg", &MobilityConstructorSet[PieceType::ROOK].quadraticConstruct.yintercept.eg },

    { "mobility-queen-quadratic-mg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.quadratic.mg },
    { "mobility-queen-quadratic-eg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.quadratic.eg },
    { "mobility-queen-slope-mg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.slope.mg },
    { "mobility-queen-slope-eg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.slope.eg },
    { "mobility-queen-yintercept-mg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.yintercept.mg },
    { "mobility-queen-yintercept-eg", &MobilityConstructorSet[PieceType::QUEEN].quadraticConstruct.yintercept.eg },

    { "attack-pawn-knight-mg", &AttackParameters[PieceType::PAWN][PieceType::KNIGHT].mg },
    { "attack-pawn-knight-eg", &AttackParameters[PieceType::PAWN][PieceType::KNIGHT].eg },
    { "attack-pawn-bishop-mg", &AttackParameters[PieceType::PAWN][PieceType::BISHOP].mg },
    { "attack-pawn-bishop-eg", &AttackParameters[PieceType::PAWN][PieceType::BISHOP].eg },
    { "attack-pawn-rook-mg", &AttackParameters[PieceType::PAWN][PieceType::ROOK].mg },
    { "attack-pawn-rook-eg", &AttackParameters[PieceType::PAWN][PieceType::ROOK].eg },
    { "attack-pawn-queen-mg", &AttackParameters[PieceType::PAWN][PieceType::QUEEN].mg },
    { "attack-pawn-queen-eg", &AttackParameters[PieceType::PAWN][PieceType::QUEEN].eg },

    { "attack-knight-pawn-mg", &AttackParameters[PieceType::KNIGHT][PieceType::PAWN].mg },
    { "attack-knight-pawn-eg", &AttackParameters[PieceType::KNIGHT][PieceType::PAWN].eg },
    { "attack-knight-bishop-mg", &AttackParameters[PieceType::KNIGHT][PieceType::BISHOP].mg },
    { "attack-knight-bishop-eg", &AttackParameters[PieceType::KNIGHT][PieceType::BISHOP].eg },
    { "attack-knight-rook-mg", &AttackParameters[PieceType::KNIGHT][PieceType::ROOK].mg },
    { "attack-knight-rook-eg", &AttackParameters[PieceType::KNIGHT][PieceType::ROOK].eg },
    { "attack-knight-queen-mg", &AttackParameters[PieceType::KNIGHT][PieceType::QUEEN].mg },
    { "attack-knight-queen-eg", &AttackParameters[PieceType::KNIGHT][PieceType::QUEEN].eg },

    { "attack-bishop-pawn-mg", &AttackParameters[PieceType::BISHOP][PieceType::PAWN].mg },
    { "attack-bishop-pawn-eg", &AttackParameters[PieceType::BISHOP][PieceType::PAWN].eg },
    { "attack-bishop-knight-mg", &AttackParameters[PieceType::BISHOP][PieceType::KNIGHT].mg },
    { "attack-bishop-knight-eg", &AttackParameters[PieceType::BISHOP][PieceType::KNIGHT].eg },
    { "attack-bishop-rook-mg", &AttackParameters[PieceType::BISHOP][PieceType::ROOK].mg },
    { "attack-bishop-rook-eg", &AttackParameters[PieceType::BISHOP][PieceType::ROOK].eg },
    { "attack-bishop-queen-mg", &AttackParameters[PieceType::BISHOP][PieceType::QUEEN].mg },
    { "attack-bishop-queen-eg", &AttackParameters[PieceType::BISHOP][PieceType::QUEEN].eg },

    { "attack-rook-pawn-mg", &AttackParameters[PieceType::ROOK][PieceType::PAWN].mg },
    { "attack-rook-pawn-eg", &AttackParameters[PieceType::ROOK][PieceType::PAWN].eg },
    { "attack-rook-knight-mg", &AttackParameters[PieceType::ROOK][PieceType::KNIGHT].mg },
    { "attack-rook-knight-eg", &AttackParameters[PieceType::ROOK][PieceType::KNIGHT].eg },
    { "attack-rook-bishop-mg", &AttackParameters[PieceType::ROOK][PieceType::BISHOP].mg },
    { "attack-rook-bishop-eg", &AttackParameters[PieceType::ROOK][PieceType::BISHOP].eg },
    { "attack-rook-queen-mg", &AttackParameters[PieceType::ROOK][PieceType::QUEEN].mg },
    { "attack-rook-queen-eg", &AttackParameters[PieceType::ROOK][PieceType::QUEEN].eg },

    { "attack-queen-pawn-mg", &AttackParameters[PieceType::QUEEN][PieceType::PAWN].mg },
    { "attack-queen-pawn-eg", &AttackParameters[PieceType::QUEEN][PieceType::PAWN].eg },
    { "attack-queen-knight-mg", &AttackParameters[PieceType::QUEEN][PieceType::KNIGHT].mg },
    { "attack-queen-knight-eg", &AttackParameters[PieceType::QUEEN][PieceType::KNIGHT].eg },
    { "attack-queen-bishop-mg", &AttackParameters[PieceType::QUEEN][PieceType::BISHOP].mg },
    { "attack-queen-bishop-eg", &AttackParameters[PieceType::QUEEN][PieceType::BISHOP].eg },
    { "attack-queen-rook-mg", &AttackParameters[PieceType::QUEEN][PieceType::ROOK].mg },
    { "attack-queen-rook-eg", &AttackParameters[PieceType::QUEEN][PieceType::ROOK].eg },

    { "search-reductions-lmr-searchedmoves-quadratic-mg", &LateMoveReductionsSearchedMoves.quadratic.mg },
    { "search-reductions-lmr-searchedmoves-quadratic-eg", &LateMoveReductionsSearchedMoves.quadratic.eg },
    { "search-reductions-lmr-searchedmoves-slope-mg", &LateMoveReductionsSearchedMoves.slope.mg },
    { "search-reductions-lmr-searchedmoves-slope-eg", &LateMoveReductionsSearchedMoves.slope.eg },
    { "search-reductions-lmr-searchedmoves-yintercept-mg", &LateMoveReductionsSearchedMoves.yintercept.mg },
    { "search-reductions-lmr-searchedmoves-yintercept-eg", &LateMoveReductionsSearchedMoves.yintercept.eg },

    { "search-reductions-pruning-margin-depthleft-quadratic-mg", &PruningMarginDepthLeft.quadratic.mg },
    { "search-reductions-pruning-margin-depthleft-quadratic-eg", &PruningMarginDepthLeft.quadratic.eg },
    { "search-reductions-pruning-margin-depthleft-slope-mg", &PruningMarginDepthLeft.slope.mg },
    { "search-reductions-pruning-margin-depthleft-slope-eg", &PruningMarginDepthLeft.slope.eg },
    { "search-reductions-pruning-margin-depthleft-yintercept-mg", &PruningMarginDepthLeft.yintercept.mg },
    { "search-reductions-pruning-margin-depthleft-yintercept-eg", &PruningMarginDepthLeft.yintercept.eg },

    { "search-reductions-pruning-margin-searchedmoves-quadratic-mg", &PruningMarginSearchedMoves.quadratic.mg },
    { "search-reductions-pruning-margin-searchedmoves-quadratic-eg", &PruningMarginSearchedMoves.quadratic.eg },
    { "search-reductions-pruning-margin-searchedmoves-slope-mg", &PruningMarginSearchedMoves.slope.mg },
    { "search-reductions-pruning-margin-searchedmoves-slope-eg", &PruningMarginSearchedMoves.slope.eg },
    { "search-reductions-pruning-margin-searchedmoves-yintercept-mg", &PruningMarginSearchedMoves.yintercept.mg },
    { "search-reductions-pruning-margin-searchedmoves-yintercept-eg", &PruningMarginSearchedMoves.yintercept.eg },

    { "tropism-knight-quadratic-mg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.quadratic.mg },
    { "tropism-knight-quadratic-eg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.quadratic.eg },
    { "tropism-knight-slope-mg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.slope.mg },
    { "tropism-knight-slope-eg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.slope.eg },
    { "tropism-knight-yintercept-mg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.yintercept.mg },
    { "tropism-knight-yintercept-eg", &TropismConstructorSet[PieceType::KNIGHT].quadraticConstruct.yintercept.eg },

    { "tropism-bishop-quadratic-mg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.quadratic.mg },
    { "tropism-bishop-quadratic-eg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.quadratic.eg },
    { "tropism-bishop-slope-mg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.slope.mg },
    { "tropism-bishop-slope-eg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.slope.eg },
    { "tropism-bishop-yintercept-mg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.yintercept.mg },
    { "tropism-bishop-yintercept-eg", &TropismConstructorSet[PieceType::BISHOP].quadraticConstruct.yintercept.eg },

    { "tropism-rook-quadratic-mg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.quadratic.mg },
    { "tropism-rook-quadratic-eg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.quadratic.eg },
    { "tropism-rook-slope-mg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.slope.mg },
    { "tropism-rook-slope-eg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.slope.eg },
    { "tropism-rook-yintercept-mg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.yintercept.mg },
    { "tropism-rook-yintercept-eg", &TropismConstructorSet[PieceType::ROOK].quadraticConstruct.yintercept.eg },

    { "tropism-queen-quadratic-mg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.quadratic.mg },
    { "tropism-queen-quadratic-eg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.quadratic.eg },
    { "tropism-queen-slope-mg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.slope.mg },
    { "tropism-queen-slope-eg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.slope.eg },
    { "tropism-queen-yintercept-mg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.yintercept.mg },
    { "tropism-queen-yintercept-eg", &TropismConstructorSet[PieceType::QUEEN].quadraticConstruct.yintercept.eg },

    { "doubled-rooks-mg", &DoubledRooks.mg },
    { "doubled-rooks-eg", &DoubledRooks.eg },

    { "tempo-mg", &Tempo.mg },
    { "tempo-eg", &Tempo.eg },

    { "bishop-current-pawns-mg", &BishopPawns[0].mg },
    { "bishop-current-pawns-eg", &BishopPawns[0].eg },
    { "bishop-other-pawns-mg", &BishopPawns[1].mg },
    { "bishop-other-pawns-eg", &BishopPawns[1].eg },

    //{ "empty-file-queen-mg", &EmptyFileQueen.mg },
    //{ "empty-file-queen-eg", &EmptyFileQueen.eg },

    { "empty-file-rook-mg", &EmptyFileRook.mg },
    { "empty-file-rook-eg", &EmptyFileRook.eg },

    { "king-shield-0-mg", &KingShield[0].mg },
    { "king-shield-0-eg", &KingShield[0].eg },
    { "king-shield-1-mg", &KingShield[1].mg },
    { "king-shield-1-eg", &KingShield[1].eg },

    { "king-attacks-knight-mg", &KingAttacks[PieceType::KNIGHT].mg },
    { "king-attacks-knight-eg", &KingAttacks[PieceType::KNIGHT].eg },
    { "king-attacks-bishop-mg", &KingAttacks[PieceType::BISHOP].mg },
    { "king-attacks-bishop-eg", &KingAttacks[PieceType::BISHOP].eg },
    { "king-attacks-rook-mg", &KingAttacks[PieceType::ROOK].mg },
    { "king-attacks-rook-eg", &KingAttacks[PieceType::ROOK].eg },
    { "king-attacks-queen-mg", &KingAttacks[PieceType::QUEEN].mg },
    { "king-attacks-queen-eg", &KingAttacks[PieceType::QUEEN].eg },

    //{ "queen-behind-passed-pawn-default-mg", &queenBehindPassedPawnDefault.mg },
    //{ "queen-behind-passed-pawn-default-eg", &queenBehindPassedPawnDefault.eg },
    //{ "queen-behind-passed-pawn-rank-mg", &queenBehindPassedPawnPstConstruct.rank.slope.mg },
    //{ "queen-behind-passed-pawn-rank-eg", &queenBehindPassedPawnPstConstruct.rank.slope.eg },
    //{ "queen-behind-passed-pawn-file-center-mg", &queenBehindPassedPawnPstConstruct.filecenter.slope.mg },
    //{ "queen-behind-passed-pawn-file-center-eg", &queenBehindPassedPawnPstConstruct.filecenter.slope.eg },

    //{ "rook-behind-passed-pawn-default-mg", &rookBehindPassedPawnDefault.mg },
    //{ "rook-behind-passed-pawn-default-eg", &rookBehindPassedPawnDefault.eg },
    //{ "rook-behind-passed-pawn-rank-mg", &rookBehindPassedPawnPstConstruct.rank.slope.mg },
    //{ "rook-behind-passed-pawn-rank-eg", &rookBehindPassedPawnPstConstruct.rank.slope.eg },
    //{ "rook-behind-passed-pawn-file-center-mg", &rookBehindPassedPawnPstConstruct.filecenter.slope.mg },
    //{ "rook-behind-passed-pawn-file-center-eg", &rookBehindPassedPawnPstConstruct.filecenter.slope.eg },
};

void InitializeParameters()
{
    const ScoreConstructor scoreConstructor;

    for (PieceType pieceType = PieceType::PAWN; pieceType < PieceType::PIECETYPE_COUNT; pieceType++) {
        scoreConstructor.construct(&(MobilityParameters[pieceType][0]), MobilityConstructorSet[pieceType], 32);
        scoreConstructor.construct(&(TropismParameters[pieceType][0]), TropismConstructorSet[pieceType], 16);
    }

    for (const Square src : SquareIterator()) {
        File file = GetFile(src);
        Rank rank = GetRank(src);

        Distance[file][rank] = std::uint32_t(std::sqrt(file * file + rank * rank));

        if (file > File::_D) {
            file = ~file;
        }

        PstParameters[PieceType::PAWN][src] = PawnPst[src] + PstByPieceAndRank[PieceType::PAWN][rank] + PstByPieceAndFile[PieceType::PAWN][file];

        //std::printf("{ %d, %d }, ", PstParameters[PieceType::PAWN][src].mg, PstParameters[PieceType::PAWN][src].eg);

        PstParameters[PieceType::KNIGHT][src] = PstByPieceAndRank[PieceType::KNIGHT][rank] + PstByPieceAndFile[PieceType::KNIGHT][file];
        PstParameters[PieceType::BISHOP][src] = PstByPieceAndRank[PieceType::BISHOP][rank] + PstByPieceAndFile[PieceType::BISHOP][file];
        PstParameters[PieceType::ROOK][src] = PstByPieceAndRank[PieceType::ROOK][rank] + PstByPieceAndFile[PieceType::ROOK][file];
        PstParameters[PieceType::QUEEN][src] = PstByPieceAndRank[PieceType::QUEEN][rank] + PstByPieceAndFile[PieceType::QUEEN][file];
        PstParameters[PieceType::KING][src] = PstByPieceAndRank[PieceType::KING][rank] + PstByPieceAndFile[PieceType::KING][file];
    }

    //std::printf("\n");
    //int a = 0;
}
