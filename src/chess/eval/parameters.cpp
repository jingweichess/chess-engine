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

QuadraticConstruct FutilityPruningMargin = {
    {  -1,  7 },
    {  91,  80 },
    {  69,  97 }
};

QuadraticConstruct HistoryDeltaByDepthLeft = {
    { 0, 0 },
    { 0, 0 },
    { 1, 1 }
};

QuadraticConstruct LateMoveReductionsDepthLeft = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};

QuadraticConstruct LateMoveReductionsSearchedMoves = {
    {  78,  21 },
    { 244,  44 },
    {  37, 130 }
};

ChessEvaluation LazyEvaluationThreshold = {
    3 * PAWN_SCORE, 3 * PAWN_SCORE
};

QuadraticConstruct NullMoveMargins = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};

QuadraticConstruct NullMoveReductions = {
    {   0,   0 },
    {   0,   0 },
    { 768, 768 }
};

QuadraticConstruct NullMoveVerificationReductions = {
    {   0,   0 },
    {   0,   0 },
    { 768, 768 }
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

QuadraticConstruct QuiescenceEarlyExitMargin = {
    {   0,   0 },
    {   0,   0 },
    { 512, 512 }
};

QuadraticConstruct QuiescenceStaticExchangeEvaluationMargin = {
    {    0,    0 },
    {    0,    0 },
    { -256, -256 }
};

QuadraticConstruct ProbCutPruningMargin = {
    {   0, 0 },
    { 100, 0 },
    { 300, 0 }
};

QuadraticConstruct ProbCutReductions = {
    {    0,    0 },
    {   32,   32 },
    { 1024, 1024 }
};

QuadraticConstruct RazoringMargin = {
    {  -67,  53 },
    {   43, 134 },
    { 1050, 901 }
};

//Parameters used by Evaluation
ChessEvaluation MaterialParameters[PieceType::PIECETYPE_COUNT] = {
    {},
    {   220,   282 },
    {   838,   891 },
    {   998,   875 },
    {  1219,  1628 },
    {  2589,  2919 },
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

ChessEvaluation Tempo = { 0, 0 };

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

ChessEvaluation PawnChainBackPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnChainFrontPstParameters[Square::SQUARE_COUNT];

ChessEvaluation PawnChainBackByRank[Rank::RANK_COUNT] = {
    {}, {},           {   0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {   0,   0 }, {}
};

ChessEvaluation PawnChainFrontByRank[Rank::RANK_COUNT] = {
    {},
    {   0,   0 },
    {   0,   0 },
    {   0,   0 },
    {  15,   0 },
    {   9,   0 },
    {}, {}
};

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
        { -35, 90 }, { -22,  85 }, {  12, 54 }, { 34, 49 }
    },
    {
        {  24,  0 }, {  12,   0 }, {  66,  0 }, { 56,  0 }
    },
    {
        { -57, 13 }, {  37, -56 }, {   7,  0 }, {  6,  0 }
    },
    {
        { -11,  0 }, {   5,   0 }, {  35,  0 }, { 22,  0 }
    },
    {}, {}
};

ChessEvaluation PstByPieceAndRank[PieceType::PIECETYPE_COUNT][Rank::RANK_COUNT] = {
    {},
    {
        {},           { 107, 156 }, {  23, 111 }, { -13,  36 }, { -38,  24 }, { -77,  45 }, { -28,  32 }, {}
    },
    {
        { -40,   0 }, { -60,   0 }, {   0,  -4 }, {  51,  24 }, {  46,  28 }, {  22,  -9 }, {  20, -20 }, { -17, -17 }
    },
    {
        {-103, 0 }, {-122, 0 }, {   8, 0 }, { -41, 0 }, { -24, 0 }, {   5, 0 },{  15, 0 }, {  -5, 0 }
    },
    {
        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {
        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    },
    {
        { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    }
};

//Parameters used in construction of Parameters used by Evaluation
PstParameterConstructorSet BasePstConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {},
    {   //PAWN
        {   
            {}, {}, {}, {}, {}, {}, {}, {},
            {  90, 165 }, {  30, 183 }, {  38, 142 }, {  34, 105 }, {  30,  91 }, { -10, 115 }, { -65, 137 }, { -86, 210 },
            {  -7,  37 }, {  -6,  27 }, {  -1,  23 }, {  -1,  24 }, {   6,  19 }, {  31,   0 }, {  35,  26 }, {   4,  22 },
            { -19,  14 }, {  -6,  -6 }, { -14, -11 }, {   1, -38 }, {   3, -28 }, {   8, -31 }, {  14, -13 }, {  -6,   0 },
            { -15, -21 }, { -31,   3 }, {  -3, -34 }, { -10, -31 }, {  -7, -30 }, {  -9, -24 }, {  -4, -14 }, { -22, -25 },
            { -24, -17 }, { -21, -11 }, { -31, -27 }, { -21, -18 }, { -19, -18 }, { -25, -16 }, {  -8,  -9 }, { -19, -27 },
            { -28, -11 }, { -24,  -3 }, { -26, -21 }, { -30, -27 }, { -22,  -3 }, {  -4, -11 }, {  14, -12 }, { -24, -31 },
            {}, {}, {}, {}, {}, {}, {}, {},
        },
        {
            .rank = { { 1, 8 }, { -17, -6 }, { -13, 33 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
    {   //KNIGHT
        {
            { -334, -179 }, {  -66,  -55 }, {  -12,  -45 }, {  -24,  -30 }, {   42,  -85 }, { -150,   17 }, { -155,  -74 }, { -262, -290 },
            { -122,    2 }, {  -47,   17 }, {  -26,   45 }, {   83,    9 }, {  -12,   41 }, {   88,  -14 }, {  -66,   15 }, {  -81, -142 },
            {    5,  -37 }, {   28,    1 }, {   49,   61 }, {   83,   79 }, {  110,   83 }, {  117,   72 }, {   40,   33 }, {  -59,   -4 },
            {   14,  -38 }, {   20,   39 }, {   80,   38 }, {  110,   77 }, {   44,   62 }, {   96,   74 }, {   -1,   95 }, {   31,  -42 },
            {  -33,   29 }, {   34,   35 }, {   83,   43 }, {   63,   56 }, {   44,   77 }, {   74,   15 }, {  131,    0 }, {  -35,  -22 },
            {  -76,   -9 }, {    7,    0 }, {    2,   74 }, {   50,   46 }, {   47,   20 }, {   49,  -28 }, {   39,  -81 }, {  -10,  -92 },
            {   24,  -80 }, {  -18,   28 }, {   25,    7 }, {   31,   17 }, {   33,    2 }, {   29,  -33 }, {   14,  -10 }, {    4,  -71 },
            {  -43,  -48 }, {  -66,  -49 }, {    5,   40 }, {   -2,   43 }, {    3,   23 }, {   35,  -31 }, {  -57,  -70 }, { -108, -104 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { -5, -3 }, { 19, 65 }, { -10, 13 } },
        },
    },
    {   //BISHOP
        {
            { -100,  -11 }, {  -83,   80 }, {  -69,   42 }, { -106,   76 }, { -129,   56 }, { -127,  -37 }, {  -59,   76 }, {  -93,  -41 },
            {  -61,   23 }, {  -90,   68 }, {  -45,   41 }, {  -36,   39 }, {  -19,   11 }, {   -4,  -12 }, { -112,   61 }, {  -53,  -63 },
            {   27,  -11 }, {   12,  -25 }, {  -36,    0 }, {   22,  -13 }, {  -16,   50 }, {   76,   -3 }, {   11,   61 }, {  -30,   93 },
            {   17,  -39 }, {   -2,   33 }, {   25,   18 }, {   74,   37 }, {   44,   30 }, {   49,    6 }, {  -40,   44 }, {   11,  -34 },
            {   18,  -17 }, {  -19,   30 }, {    4,   33 }, {   50,   27 }, {   52,   10 }, {   -9,   13 }, {  -36,   17 }, {   14,  -43 },
            {   -7,   -5 }, {   44,    9 }, {   13,   22 }, {   20,   21 }, {   22,   39 }, {   28,  -11 }, {   48,  -26 }, {   22,    2 },
            {   43,  -12 }, {   70,  -49 }, {   76,  -54 }, {   46,  -34 }, {   64,  -35 }, {   80,  -92 }, {   91,  -47 }, {   78, -154 },
            {    9,  -62 }, {   58,  -91 }, {    9,  -53 }, {   -5,  -19 }, {   18,  -46 }, {  -10,  -44 }, {   22,  -56 }, {  -57,  -78 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
    {   //ROOK
        {
            {   86,   24 }, {   66,   32 }, {   59,   36 }, {   44,   35 }, {   -5,  -12 }, {    5,  -66 }, {   57,   -1 }, {   80,  -82 },
            {   72,   33 }, {   52,   47 }, {   73,   55 }, {   78,   73 }, {   58,   38 }, {   75,   65 }, {   89,   76 }, {  128,  -64 },
            {   37,  -20 }, {   24,  -27 }, {   19,   11 }, {  -16,   16 }, {   40,   24 }, {   47,   -3 }, {   93,    3 }, {   83,  -42 },
            {  -67,   19 }, { -102,   20 }, {  -23,  -20 }, {  -55,   -4 }, {  -66,  -73 }, {  -58,  -21 }, {   66,   23 }, {   92,  -55 },
            {  -79,   -4 }, { -105,   76 }, { -116,   80 }, {  -97,   43 }, {  -63,   13 }, {  -90,   -6 }, {  -44,   50 }, {  -52,   31 },
            {  -41,  -16 }, {  -24,  -33 }, {  -88,   31 }, {  -80,   39 }, {  -53,  -34 }, {  -56,    3 }, {   33,  -55 }, {  -23,  -47 },
            {  -52,  -54 }, {  -31,  -46 }, {  -13,   22 }, {  -40,   49 }, {  -42,   32 }, {  -31,  -41 }, {   29,  -33 }, {   -2,  -61 },
            {  -19,  -34 }, {   -2,  -25 }, {   20,   -9 }, {   20,    8 }, {   11,   16 }, {   -4,   28 }, {   -7,  -18 }, {  -28,  -64 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
    {   //QUEEN
        {
            {   29,  104 }, {   90,  -22 }, {  -66,   10 }, {  -86,  -67 }, {  -54, -113 }, {  -21,  -56 }, { -137,  118 }, {  -56,  145 },
            { -110,   93 }, { -132,  111 }, {  -29,   69 }, {  -28,   54 }, {  -11,   41 }, {   51,   31 }, {   82,   68 }, {  128,   88 },
            {  -86,   63 }, {  -70,   91 }, {  -83,   96 }, {  -15,   55 }, {   47,   53 }, {  100,   74 }, {  111,   83 }, {   82,   79 },
            {  -94,   64 }, {  -93,   72 }, {  -40,   79 }, {  -33,   69 }, {    7,   57 }, {   -9,   50 }, {   65,   53 }, {   31,   60 },
            {  -82,   20 }, {  -90,   77 }, {  -52,   47 }, {  -60,   77 }, {  -63,   31 }, {   25,   -1 }, {   51,   10 }, {   65,   52 },
            {   -9,  -36 }, {  -44,  -18 }, {    1,  -13 }, {  -66,  -12 }, {  -18,  -63 }, {   30,  -84 }, {   81, -110 }, {   96, -114 },
            {  -38,  -14 }, {    8,  -71 }, {   -5,  -91 }, {   -4,  -99 }, {  -11, -104 }, {  -30, -117 }, {  -72, -127 }, {  -12, -134 },
            {   92,  -95 }, {    9,  -86 }, {   17,  -91 }, {   -6,  -92 }, {  -18, -113 }, {  -98, -123 }, { -116, -135 }, {  -30,  -95 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
    {   //KING
        {
            { -133,  -80 }, {   78,   58 }, {   90,   60 }, {   74,   60 }, {   51,   30 }, {   48,   43 }, { -112,   21 }, { -205, -210 },
            {   41,   25 }, {   94,   75 }, {  103,   80 }, {   99,   77 }, {   92,   71 }, {   91,   68 }, {  104,   85 }, { -157,  -17 },
            {   11,  -48 }, {  103,   82 }, {  101,   79 }, {  105,   81 }, {  103,   81 }, {  114,   93 }, {   97,   83 }, {  -12,   15 },
            {   81, -133 }, {   66,   47 }, {  100,   76 }, {   98,   76 }, {   97,   78 }, {   88,   72 }, {   45,   55 }, { -137,  -62 },
            { -124,  -76 }, {   32,   19 }, {   76,   58 }, {   84,   64 }, {   66,   48 }, {   14,   38 }, { -135,   46 }, { -165, -152 },
            { -274,   21 }, { -101,   55 }, {  -37,   63 }, {   -8,   80 }, {   18,   62 }, {  -32,   51 }, { -104,   36 }, { -328,   76 },
            {  -59,  -42 }, {  -83,   10 }, {    8,   -2 }, {  -85,   39 }, {  -70,   35 }, {  -26,   -7 }, {   24,  -80 }, {  -57,  -99 },
            { -141, -144 }, {   24, -136 }, {  -19,  -77 }, { -156, -140 }, {  -90, -150 }, { -118, -104 }, {    6, -128 }, { -127, -154 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
};

QuadraticParameterConstructorSet MobilityConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { //KNIGHT
        //.quadraticBase = { {  -58,  -77 }, {  -38,  -56 }, {  -15,  -26 }, {   -8,   29 }, {    6,   31 }, {    7,   60 }, {   20,   39 }, {   30,   23 }, {   54,  -23 }, },
        //.quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { -6, -84 }, { 131, 455 }, { -51, -84 } },
    },
    { //BISHOP
        //.quadraticBase = { { -132, -217 }, {  -17, -123 }, {   -5,  -41 }, {    9,    2 }, {   13,   62 }, {   15,  119 }, {   35,  123 }, {   48,  143 }, {   55,  160 }, {   80,  144 }, {  150,  129 },
        //    {  153,  143 }, {  119,  185 }, {  199,  217 }, },
        //.quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 0, -52 }, { 157, 551 }, { -73, -183 } },
    },
    { //ROOK
        //.quadraticBase = { {  -82,  -85 }, {  -78,  -71 }, {  -74,  -66 }, {  -63,  -58 }, {  -63,  -29 }, {  -50,   -2 }, {  -49,   29 }, {  -22,   22 }, {   -9,   46 }, {    9,   54 }, {   34,   57 },
        //    {   45,   62 }, {   66,   65 }, {  129,   20 }, {  208,  -45 }, },
        //.quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 27, -29 }, { -52, 274 }, { -70, -115 } },
    },
    { //QUEEN
      //  .quadraticBase = { { -105, -255 }, { -128, -203 }, { -127, -236 }, {  -98, -220 }, {  -75, -187 }, {  -76, -161 }, {  -58, -152 }, {  -54, -118 }, {  -59,  -51 }, {  -31,  -92 }, {  -56,  -20 }, {  -51,   32 }, {  -65,   89 }, {  -38,   71 }, {  -42,   89 },
      //{  -29,  119 }, {    8,   86 }, {   -8,  126 }, {   63,  114 }, {   31,  147 }, {  121,  148 }, {  129,  156 }, {  146,  173 }, {  161,  136 }, {  149,  166 }, {  171,  186 }, {  237,  -45 }, { -112,  -99 }, },
      //  .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .quadraticBase = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, },
        .quadraticConstruct = { { 1, -19 }, { 65, 367 }, { -128, -328 } },
    }
};

PstParameterConstructorSet PawnChainBackPstConstructorSet = {
    {
        {}, {}, {}, {}, {}, {}, {}, {},
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {   94,   94 }, {   93,   93 }, {   89,   88 }, {   89,   90 }, {   82,   81 }, {   80,   81 }, {   75,   79 }, {   79,   87 },
        {   19,   55 }, {   37,   46 }, {   32,   61 }, {   46,   65 }, {   64,   55 }, {   51,   26 }, {   31,   12 }, {   41,   45 },
        {  -13,    7 }, {   38,    0 }, {   13,    4 }, {    7,   -1 }, {    0,   -2 }, {    2,   27 }, {  -71,   36 }, {  -14,   20 },
        {  -10,    1 }, {   -9,    0 }, {   -8,   15 }, {  -25,  -15 }, {  -25,   30 }, {    3,   18 }, {  -26,   11 }, {  -16,   -9 },
        {  -22,  -36 }, {   -4,   10 }, {  -17,  -16 }, {  -19,  -13 }, {  -22,   15 }, {    3,    1 }, {   12,  -22 }, {  -31,  -51 },
        {}, {}, {}, {}, {}, {}, {}, {},
    },
    {
        .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { 44,   76 }
};

PstParameterConstructorSet PawnChainFrontPstConstructorSet = {
    {
        {}, {}, {}, {}, {}, {}, {}, {},
        {   80,   85 }, {   70,   64 }, {   62,   64 }, {   79,   74 }, {   47,   43 }, {   16,   18 }, {   47,   40 }, {   14,   38 },
        {  -23,   -9 }, {   -7,   92 }, {    6,   71 }, {   54,  107 }, {   89,   45 }, {  129,  -88 }, {   49,  -69 }, {  -49,    4 },
        {  -52,  -49 }, {  -79,   -7 }, {  -13,   12 }, {  -36,  -15 }, {    4,  -23 }, {  -46,    3 }, {  -18,  -79 }, {  -22,  -48 },
        {  -26,  -13 }, {    4,  -26 }, {  -19,    9 }, {    8,   18 }, {    7,  -10 }, {   26,  -37 }, {   10,  -52 }, {   25,  -57 },
        {   -6,  -42 }, {    5,   47 }, {   11,   54 }, {   22,   55 }, {    2,   55 }, {   -1,   31 }, {   24,    2 }, {   33,  -42 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {}, {}, {}, {}, {}, {}, {}, {},
    },
    {
        .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { 0, 0 }
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

    { "pst-pawn-rank-quadratic-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.quadratic.mg },
    { "pst-pawn-rank-quadratic-eg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.quadratic.eg },
    { "pst-pawn-rank-slope-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.slope.mg },
    { "pst-pawn-rank-slope-eg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.slope.eg },
    { "pst-pawn-rank-yintercept-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.yintercept.mg },
    { "pst-pawn-rank-yintercept-eg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.yintercept.eg },

    { "pst-king-file-a-mg", &PstByPieceAndFile[PieceType::KING][File::_A].mg },
    { "pst-king-file-a-eg", &PstByPieceAndFile[PieceType::KING][File::_A].eg },
    { "pst-king-file-b-mg", &PstByPieceAndFile[PieceType::KING][File::_B].mg },
    { "pst-king-file-b-eg", &PstByPieceAndFile[PieceType::KING][File::_B].eg },
    { "pst-king-file-c-mg", &PstByPieceAndFile[PieceType::KING][File::_C].mg },
    { "pst-king-file-c-eg", &PstByPieceAndFile[PieceType::KING][File::_C].eg },
    { "pst-king-file-d-mg", &PstByPieceAndFile[PieceType::KING][File::_D].mg },
    { "pst-king-file-d-eg", &PstByPieceAndFile[PieceType::KING][File::_D].eg },

    { "pst-king-rank-8-mg", &PstByPieceAndRank[PieceType::KING][Rank::_8].mg },
    { "pst-king-rank-8-eg", &PstByPieceAndRank[PieceType::KING][Rank::_8].eg },
    { "pst-king-rank-7-mg", &PstByPieceAndRank[PieceType::KING][Rank::_7].mg },
    { "pst-king-rank-7-eg", &PstByPieceAndRank[PieceType::KING][Rank::_7].eg },
    { "pst-king-rank-6-mg", &PstByPieceAndRank[PieceType::KING][Rank::_6].mg },
    { "pst-king-rank-6-eg", &PstByPieceAndRank[PieceType::KING][Rank::_6].eg },
    { "pst-king-rank-5-mg", &PstByPieceAndRank[PieceType::KING][Rank::_5].mg },
    { "pst-king-rank-5-eg", &PstByPieceAndRank[PieceType::KING][Rank::_5].eg },
    { "pst-king-rank-4-mg", &PstByPieceAndRank[PieceType::KING][Rank::_4].mg },
    { "pst-king-rank-4-eg", &PstByPieceAndRank[PieceType::KING][Rank::_4].eg },
    { "pst-king-rank-3-mg", &PstByPieceAndRank[PieceType::KING][Rank::_3].mg },
    { "pst-king-rank-3-eg", &PstByPieceAndRank[PieceType::KING][Rank::_3].eg },
    { "pst-king-rank-2-mg", &PstByPieceAndRank[PieceType::KING][Rank::_2].mg },
    { "pst-king-rank-2-eg", &PstByPieceAndRank[PieceType::KING][Rank::_2].eg },
    { "pst-king-rank-1-mg", &PstByPieceAndRank[PieceType::KING][Rank::_1].mg },
    { "pst-king-rank-1-eg", &PstByPieceAndRank[PieceType::KING][Rank::_1].eg },

    { "pst-pawn-file-center-quadratic-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.quadratic.mg },
    { "pst-pawn-file-center-quadratic-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.quadratic.eg },
    { "pst-pawn-file-center-slope-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.slope.mg },
    { "pst-pawn-file-center-slope-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.slope.eg },
    { "pst-pawn-file-center-yintercept-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.yintercept.mg },
    { "pst-pawn-file-center-yintercept-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.yintercept.eg },

    { "pst-knight-rank-quadratic-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.mg },
    { "pst-knight-rank-quadratic-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.eg },
    { "pst-knight-rank-slope-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.mg },
    { "pst-knight-rank-slope-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.eg },
    { "pst-knight-rank-yintercept-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.mg },
    { "pst-knight-rank-yintercept-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.eg },

    { "pst-knight-center-quadratic-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.mg },
    { "pst-knight-center-quadratic-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.eg },
    { "pst-knight-center-slope-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.mg },
    { "pst-knight-center-slope-eg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.eg },
    { "pst-knight-center-yintercept-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.mg },
    { "pst-knight-center-yintercept-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.eg },

    { "pst-bishop-rank-quadratic-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.quadratic.mg },
    { "pst-bishop-rank-quadratic-eg",&BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.quadratic.eg },
    { "pst-bishop-rank-slope-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.slope.mg },
    { "pst-bishop-rank-slope-eg",&BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.slope.eg },
    { "pst-bishop-rank-yintercept-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.yintercept.mg },
    { "pst-bishop-rank-yintercept-eg",&BasePstConstructorSet[PieceType::BISHOP].pstConstruct.rank.yintercept.eg },

    { "pst-bishop-center-quadratic-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.quadratic.mg },
    { "pst-bishop-center-quadratic-eg",&BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.quadratic.eg },
    { "pst-bishop-center-slope-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.slope.mg },
    { "pst-bishop-center-slope-eg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.slope.eg },
    { "pst-bishop-center-yintercept-mg", &BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.yintercept.mg },
    { "pst-bishop-center-yintercept-eg",&BasePstConstructorSet[PieceType::BISHOP].pstConstruct.center.yintercept.eg },

    { "pst-rook-rank-quadratic-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.quadratic.mg },
    { "pst-rook-rank-quadratic-eg",&BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.quadratic.eg },
    { "pst-rook-rank-slope-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.slope.mg },
    { "pst-rook-rank-slope-eg",&BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.slope.eg },
    { "pst-rook-rank-yintercept-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.yintercept.mg },
    { "pst-rook-rank-yintercept-eg",&BasePstConstructorSet[PieceType::ROOK].pstConstruct.rank.yintercept.eg },

    { "pst-rook-center-quadratic-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.quadratic.mg },
    { "pst-rook-center-quadratic-eg",&BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.quadratic.eg },
    { "pst-rook-center-slope-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.slope.mg },
    { "pst-rook-center-slope-eg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.slope.eg },
    { "pst-rook-center-yintercept-mg", &BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.yintercept.mg },
    { "pst-rook-center-yintercept-eg",&BasePstConstructorSet[PieceType::ROOK].pstConstruct.center.yintercept.eg },

    { "pst-queen-rank-quadratic-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.quadratic.mg },
    { "pst-queen-rank-quadratic-eg",&BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.quadratic.eg },
    { "pst-queen-rank-slope-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.slope.mg },
    { "pst-queen-rank-slope-eg",&BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.slope.eg },
    { "pst-queen-rank-yintercept-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.yintercept.mg },
    { "pst-queen-rank-yintercept-eg",&BasePstConstructorSet[PieceType::QUEEN].pstConstruct.rank.yintercept.eg },

    { "pst-queen-center-quadratic-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.quadratic.mg },
    { "pst-queen-center-quadratic-eg",&BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.quadratic.eg },
    { "pst-queen-center-slope-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.slope.mg },
    { "pst-queen-center-slope-eg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.slope.eg },
    { "pst-queen-center-yintercept-mg", &BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.yintercept.mg },
    { "pst-queen-center-yintercept-eg",&BasePstConstructorSet[PieceType::QUEEN].pstConstruct.center.yintercept.eg },

    { "pst-king-rank-quadratic-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.rank.quadratic.mg },
    { "pst-king-rank-quadratic-eg",&BasePstConstructorSet[PieceType::KING].pstConstruct.rank.quadratic.eg },
    { "pst-king-rank-slope-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.rank.slope.mg },
    { "pst-king-rank-slope-eg",&BasePstConstructorSet[PieceType::KING].pstConstruct.rank.slope.eg },
    { "pst-king-rank-yintercept-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.rank.yintercept.mg },
    { "pst-king-rank-yintercept-eg",&BasePstConstructorSet[PieceType::KING].pstConstruct.rank.yintercept.eg },

    { "pst-king-center-quadratic-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.center.quadratic.mg },
    { "pst-king-center-quadratic-eg",&BasePstConstructorSet[PieceType::KING].pstConstruct.center.quadratic.eg },
    { "pst-king-center-slope-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.center.slope.mg },
    { "pst-king-center-slope-eg", &BasePstConstructorSet[PieceType::KING].pstConstruct.center.slope.eg },
    { "pst-king-center-yintercept-mg", &BasePstConstructorSet[PieceType::KING].pstConstruct.center.yintercept.mg },
    { "pst-king-center-yintercept-eg",&BasePstConstructorSet[PieceType::KING].pstConstruct.center.yintercept.eg },

    { "pawn-chain-back-default-mg", &PawnChainBackPstConstructorSet.defaultEvaluation.mg },
    { "pawn-chain-back-default-eg", &PawnChainBackPstConstructorSet.defaultEvaluation.eg },

    //{ "pawn-chain-back-rank-quadratic-mg", &pawnChainBackPstConstruct.rank.quadratic.mg },
    //{ "pawn-chain-back-rank-quadratic-eg", &pawnChainBackPstConstruct.rank.quadratic.eg },
    //{ "pawn-chain-back-rank-slope-mg", &pawnChainBackPstConstruct.rank.slope.mg },
    //{ "pawn-chain-back-rank-slope-eg", &pawnChainBackPstConstruct.rank.slope.eg },

    //{ "pawn-chain-back-file-center-quadratic-mg", &pawnChainBackPstConstruct.filecenter.quadratic.mg },
    //{ "pawn-chain-back-file-center-quadratic-eg", &pawnChainBackPstConstruct.filecenter.quadratic.eg },
    //{ "pawn-chain-back-file-center-slope-mg", &pawnChainBackPstConstruct.filecenter.slope.mg },
    //{ "pawn-chain-back-file-center-slope-eg", &pawnChainBackPstConstruct.filecenter.slope.eg },

    { "pawn-chain-front-default-mg", &PawnChainFrontPstConstructorSet.defaultEvaluation.mg },
    { "pawn-chain-front-default-eg", &PawnChainFrontPstConstructorSet.defaultEvaluation.eg },

    //{ "pawn-chain-front-rank-quadratic-mg", &pawnChainFrontPstConstruct.rank.quadratic.mg },
    //{ "pawn-chain-front-rank-quadratic-eg", &pawnChainFrontPstConstruct.rank.quadratic.eg },
    //{ "pawn-chain-front-rank-slope-mg", &pawnChainFrontPstConstruct.rank.slope.mg },
    //{ "pawn-chain-front-rank-slope-eg", &pawnChainFrontPstConstruct.rank.slope.eg },

    //{ "pawn-chain-front-file-center-quadratic-mg", &pawnChainFrontPstConstruct.filecenter.quadratic.mg },
    //{ "pawn-chain-front-file-center-quadratic-eg", &pawnChainFrontPstConstruct.filecenter.quadratic.eg },
    //{ "pawn-chain-front-file-center-slope-mg", &pawnChainFrontPstConstruct.filecenter.slope.mg },
    //{ "pawn-chain-front-file-center-slope-eg", &pawnChainFrontPstConstruct.filecenter.slope.eg },

    { "pawn-chain-front-rank-2-mg", &PawnChainFrontByRank[Rank::_2].mg },
    { "pawn-chain-front-rank-2-eg", &PawnChainFrontByRank[Rank::_2].eg },
    { "pawn-chain-front-rank-3-mg", &PawnChainFrontByRank[Rank::_3].mg },
    { "pawn-chain-front-rank-3-eg", &PawnChainFrontByRank[Rank::_3].eg },
    { "pawn-chain-front-rank-4-mg", &PawnChainFrontByRank[Rank::_4].mg },
    { "pawn-chain-front-rank-4-eg", &PawnChainFrontByRank[Rank::_4].eg },
    { "pawn-chain-front-rank-5-mg", &PawnChainFrontByRank[Rank::_5].mg },
    { "pawn-chain-front-rank-5-eg", &PawnChainFrontByRank[Rank::_5].eg },
    { "pawn-chain-front-rank-6-mg", &PawnChainFrontByRank[Rank::_6].mg },
    { "pawn-chain-front-rank-6-eg", &PawnChainFrontByRank[Rank::_6].eg },
    { "pawn-chain-front-rank-7-mg", &PawnChainFrontByRank[Rank::_7].mg },
    { "pawn-chain-front-rank-7-eg", &PawnChainFrontByRank[Rank::_7].eg },

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

    ////{ "search-aspiration-initial-value-mg", &AspirationWindowInitialValue.mg },
    ////{ "search-aspiration-initial-value-eg", &AspirationWindowInitialValue.eg },
    ////{ "search-aspiration-window-delta-mg", &AspirationWindowDelta.mg },
    ////{ "search-aspiration-window-delta-eg", &AspirationWindowDelta.eg },

    { "search-futility-margin-quadratic-mg", &FutilityPruningMargin.quadratic.mg },
    { "search-futility-margin-quadratic-eg", &FutilityPruningMargin.quadratic.eg },
    { "search-futility-margin-slope-mg", &FutilityPruningMargin.slope.mg },
    { "search-futility-margin-slope-eg", &FutilityPruningMargin.slope.eg },
    { "search-futility-margin-yintercept-mg", &FutilityPruningMargin.yintercept.mg },
    { "search-futility-margin-yintercept-eg", &FutilityPruningMargin.yintercept.eg },

    //{ "search-history-delta-depthleft-quadratic-mg", &HistoryDeltaByDepthLeft.quadratic.mg },
    //{ "search-history-delta-depthleft-slope-mg", &HistoryDeltaByDepthLeft.slope.mg },
    //{ "search-history-delta-depthleft-yintercept-mg", &HistoryDeltaByDepthLeft.yintercept.mg },

    { "search-lazy-evaluation-margin-mg", &LazyEvaluationThreshold.mg },
    { "search-lazy-evaluation-margin-eg", &LazyEvaluationThreshold.eg },

    { "search-nullmove-margin-quadratic-mg", &NullMoveMargins.quadratic.mg },
    { "search-nullmove-margin-quadratic-eg", &NullMoveMargins.quadratic.eg },
    { "search-nullmove-margin-slope-mg", &NullMoveMargins.slope.mg },
    { "search-nullmove-margin-slope-eg", &NullMoveMargins.slope.eg },
    { "search-nullmove-margin-yintercept-mg", &NullMoveMargins.yintercept.mg },
    { "search-nullmove-margin-yintercept-eg", &NullMoveMargins.yintercept.eg },

    { "search-nullmove-reductions-quadratic-mg", &NullMoveReductions.quadratic.mg },
    { "search-nullmove-reductions-slope-mg", &NullMoveReductions.slope.mg },
    { "search-nullmove-reductions-yintercept-mg", &NullMoveReductions.yintercept.mg },

    { "search-nullmove-verification-reductions-quadratic-mg", &NullMoveVerificationReductions.quadratic.mg },
    { "search-nullmove-verification-reductions-slope-mg", &NullMoveVerificationReductions.slope.mg },
    { "search-nullmove-verification-reductions-yintercept-mg", &NullMoveVerificationReductions.yintercept.mg },

    //{ "search-probcut-margin-quadratic-mg", &ProbCutPruningMargin.quadratic.mg },
    //{ "search-probcut-margin-slope-mg", &ProbCutPruningMargin.slope.mg },
    //{ "search-probcut-margin-yintercept-mg", &ProbCutPruningMargin.yintercept.mg },

    { "search-quiescence-early-exit-margin-quadratic-mg", &QuiescenceEarlyExitMargin.quadratic.mg },
    { "search-quiescence-early-exit-margin-quadratic-eg", &QuiescenceEarlyExitMargin.quadratic.eg },
    { "search-quiescence-early-exit-margin-slope-eg", &QuiescenceEarlyExitMargin.slope.eg },
    { "search-quiescence-early-exit-margin-slope-mg", &QuiescenceEarlyExitMargin.slope.mg },
    { "search-quiescence-early-exit-margin-yintercept-mg", &QuiescenceEarlyExitMargin.yintercept.mg },
    { "search-quiescence-early-exit-margin-yintercept-eg", &QuiescenceEarlyExitMargin.yintercept.eg },

    //{ "search-quiescence-early-see-quadratic-mg", &QuiescenceStaticExchangeEvaluationMargin.quadratic.mg },
    //{ "search-quiescence-early-see-slope-mg", &QuiescenceStaticExchangeEvaluationMargin.slope.mg },
    //{ "search-quiescence-early-see-yintercept-mg", &QuiescenceStaticExchangeEvaluationMargin.yintercept.mg },

    { "search-razoring-margin-quadratic-mg", &RazoringMargin.quadratic.mg },
    { "search-razoring-margin-quadratic-eg", &RazoringMargin.quadratic.eg },
    { "search-razoring-margin-slope-mg", &RazoringMargin.slope.mg },
    { "search-razoring-margin-slope-eg", &RazoringMargin.slope.eg },
    { "search-razoring-margin-yintercept-mg", &RazoringMargin.yintercept.mg },
    { "search-razoring-margin-yintercept-eg", &RazoringMargin.yintercept.eg },

    { "search-reductions-lmr-depthleft-quadratic-mg", &LateMoveReductionsDepthLeft.quadratic.mg },
    { "search-reductions-lmr-depthleft-quadratic-eg", &LateMoveReductionsDepthLeft.quadratic.eg },
    { "search-reductions-lmr-depthleft-slope-mg", &LateMoveReductionsDepthLeft.slope.mg },
    { "search-reductions-lmr-depthleft-slope-eg", &LateMoveReductionsDepthLeft.slope.eg },
    { "search-reductions-lmr-depthleft-yintercept-mg", &LateMoveReductionsDepthLeft.yintercept.mg },
    { "search-reductions-lmr-depthleft-yintercept-eg", &LateMoveReductionsDepthLeft.yintercept.eg },

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
        scoreConstructor.construct(PstParameters[pieceType], BasePstConstructorSet[pieceType]);

        scoreConstructor.construct(&(MobilityParameters[pieceType][0]), MobilityConstructorSet[pieceType], 32);
        scoreConstructor.construct(&(TropismParameters[pieceType][0]), TropismConstructorSet[pieceType], 16);
    }

    scoreConstructor.construct(PawnChainBackPstParameters, PawnChainBackPstConstructorSet);
    scoreConstructor.construct(PawnChainFrontPstParameters, PawnChainFrontPstConstructorSet);

    for (const Square src : SquareIterator()) {
        File file = GetFile(src);
        Rank rank = GetRank(src);

        Distance[file][rank] = std::uint32_t(std::sqrt(file * file + rank * rank));

        if (file > File::_D) {
            file = ~file;
        }

        PstParameters[PieceType::PAWN][src] = PstParameters[PieceType::PAWN][src] / 2 + PstByPieceAndRank[PieceType::PAWN][rank] + PstByPieceAndFile[PieceType::PAWN][file];

        PstParameters[PieceType::KNIGHT][src] = PstByPieceAndRank[PieceType::KNIGHT][rank] + PstByPieceAndFile[PieceType::KNIGHT][file];
        PstParameters[PieceType::BISHOP][src] = PstByPieceAndRank[PieceType::BISHOP][rank] + PstByPieceAndFile[PieceType::BISHOP][file];
        //PstParameters[PieceType::ROOK][src] = PstByPieceAndRank[PieceType::ROOK][rank] + PstByPieceAndFile[PieceType::ROOK][file];
        //PstParameters[PieceType::QUEEN][src] = PstByPieceAndRank[PieceType::QUEEN][rank] + PstByPieceAndFile[PieceType::QUEEN][file];
        //PstParameters[PieceType::KING][src] = PstByPieceAndRank[PieceType::KING][rank] + PstByPieceAndFile[PieceType::KING][file];
    }
}
