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

#include <cmath>

#include "../../game/personality/parametermap.h"

#include "../../game/types/color.h"
#include "../../game/types/score.h"

#include "../eval/constructor.h"

#include "../types/piecetype.h"
#include "../types/score.h"
#include "../types/square.h"

#include "parameters.h"

ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT];

QuadraticConstruct FutilityPruningMargin = {
    {  -8,  59 },
    { 182, 160 },
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
    { 415,  238 },
    { 134, -137 },
    {  66,   18 }
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
    {  401, 394 },
    { -290, -36 },
    {  129, 151 }
};

QuadraticConstruct PruningMarginSearchedMoves = {
    { -229, -276 },
    {  108, -273 },
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
    { 200, 0 },
    { 300, 0 }
};

QuadraticConstruct RazoringMargin = {
    {   0,   0 },
    {   0,   0 },
    { 600, 600 }
};

//Parameters used by Evaluation
ChessEvaluation MaterialParameters[PieceType::PIECETYPE_COUNT] = {
    {},
    {   205,   332 },
    {   823,   848 },
    {   911,   897 },
    {  1396,  1422 },
    {  2767,  2750 },
};

ChessEvaluation PstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

//Attack Parameters aren't "built" so they're hard coded here
ChessEvaluation AttackParameters[PieceType::PIECETYPE_COUNT][PieceType::PIECETYPE_COUNT] = {
    {},   //PAWN          KNIGHT          BISHOP          ROOK            QUEEN          is attacked by...
    { {}, {},             {  155,  166 }, {  148,  172 }, {  168,  173 }, {  118,  172 }, },    //PAWN
    { {}, {  -20,   44 }, {},             {   80,  105 }, {  141,  142 }, {   82,  159 }, },    //KNIGHT
    { {}, {   -7,   56 }, {   57,  113 }, {},             {  142,  153 }, {  155,  180 }, },    //BISHOP
    { {}, {  -19,   63 }, {   37,   71 }, {   64,   67 }, {},             {  174,  185 }, },    //ROOK
    { {}, {  -11,   37 }, {   13,  -37 }, {   11,   68 }, {   -9,   72 }, {}, },                //QUEEN
};

ChessEvaluation MobilityParameters[PieceType::PIECETYPE_COUNT][32];

ChessEvaluation OutpostPstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

ChessEvaluation TropismParameters[PieceType::PIECETYPE_COUNT][16];

ChessEvaluation PawnChainBackPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnChainFrontPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnDoubledPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnPassedPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnPhalanxPstParameters[Square::SQUARE_COUNT];

//Parameters used in construction of Parameters used by Evaluation
PstParameterConstructorSet BasePstConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {},
    {   //PAWN
        {
            {}, {}, {}, {}, {}, {}, {}, {},
            {  181,  330 }, {   60,  366 }, {   77,  284 }, {   68,  210 }, {   61,  182 }, {  -21,  230 }, { -130,  274 }, { -173,  421 },
            {   61,   73 }, {   52,   71 }, {   56,   67 }, {   45,   55 }, {   43,   50 }, {   58,   52 }, {   69,   69 }, {   42,   55 },
            {  -42,   25 }, {  -38,   32 }, {  -54,  -35 }, {  -16,  -64 }, {    0,  -71 }, {   38,  -86 }, {    0,  -17 }, {  -41,  -14 },
            {  -30,  -42 }, {  -63,    7 }, {   -7,  -68 }, {  -20,  -63 }, {  -14,  -61 }, {  -18,  -48 }, {   -9,  -29 }, {  -44,  -51 },
            {  -49,  -35 }, {  -42,  -22 }, {  -63,  -54 }, {  -42,  -37 }, {  -38,  -37 }, {  -50,  -33 }, {  -16,  -18 }, {  -38,  -55 },
            {  -56,  -23 }, {  -48,   -7 }, {  -53,  -43 }, {  -61,  -54 }, {  -45,   -6 }, {   -8,  -23 }, {   29,  -25 }, {  -49,  -62 },
            {}, {}, {}, {}, {}, {}, {}, {},
        },
        {
            .rank = { { 25, 131 }, { -69, -23 }, { -27, 67 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
    },
    {   //KNIGHT
        {
            { -178, -167 }, {  -28,  -47 }, {   19,  -17 }, {  -92,    0 }, {   11,  -20 }, {  -80,  -57 }, { -188,  -39 }, { -173, -110 },
            { -122,    2 }, {  -47,   17 }, {  -26,   45 }, {   83,    9 }, {  -12,   41 }, {   88,  -14 }, {  -66,   15 }, {  -81, -142 },
            {    5,  -37 }, {   28,    1 }, {   49,   61 }, {   83,   79 }, {  110,   83 }, {  117,   72 }, {   40,   33 }, {  -59,   -4 },
            {   14,  -38 }, {   20,   39 }, {   80,   38 }, {  110,   77 }, {   44,   62 }, {   96,   74 }, {   -1,   95 }, {   31,  -42 },
            {  -33,   29 }, {   34,   35 }, {   83,   43 }, {   63,   56 }, {   44,   77 }, {   74,   15 }, {  131,    0 }, {  -35,  -22 },
            {  -76,   -9 }, {    7,    0 }, {    2,   74 }, {   50,   46 }, {   47,   20 }, {   49,  -28 }, {   39,  -81 }, {  -10,  -92 },
            {  -48,  -63 }, {  -43,   12 }, {   23,  -31 }, {   25,  -16 }, {   10,    2 }, {   13,  -17 }, {   -9,  -28 }, {    1,  -36 },
            {  -43,  -48 }, {  -66,  -49 }, {    5,   40 }, {   -2,   43 }, {    3,   23 }, {   35,  -31 }, {  -57,  -70 }, { -108, -104 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { -42 + 0, -27 - 0 }, { 38 - 0, 136 - 0 }, { -10 - 0, 13 + 0 } },
        },
    },
    {   //BISHOP
        {
            { -100,  -11 }, {  -83,   80 }, {  -69,   42 }, { -106,   76 }, { -129,   56 }, { -127,  -37 }, {  -59,   76 }, {  -93,  -41 },
            {  -61,   23 }, {  -90,   68 }, {  -45,   41 }, {  -36,   39 }, {  -19,   11 }, {   -4,  -12 }, { -112,   61 }, {  -53,  -63 },
            {    1,   14 }, {   28,   21 }, {  -36,   37 }, {   47,    0 }, {   70,   19 }, {   80,   29 }, {   79,   25 }, {   61,   18 },
            {   17,  -39 }, {   -2,   33 }, {   25,   18 }, {   74,   37 }, {   44,   30 }, {   49,    6 }, {  -40,   44 }, {   11,  -34 },
            {   18,  -17 }, {  -19,   30 }, {    4,   33 }, {   50,   27 }, {   52,   10 }, {   -9,   13 }, {  -36,   17 }, {   14,  -43 },
            {   -7,   -5 }, {   44,    9 }, {   13,   22 }, {   20,   21 }, {   22,   39 }, {   28,  -11 }, {   48,  -26 }, {   22,    2 },
            {    7,  -17 }, {    8,  -12 }, {   37,  -45 }, {    1,   18 }, {   25,  -17 }, {    5,  -72 }, {   67,  -93 }, {   -2, -163 },
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
            {  -25,  -59 }, {   15,  -58 }, {   29,  -17 }, {    4,   43 }, {   24,    1 }, {   20,   37 }, {   17,  -48 }, {  -53,  -81 },
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
            { -108,  -81 }, {  -43,   44 }, {   28,   34 }, {    9,   58 }, {   27,   45 }, {  -40,   51 }, {  -33,  -27 }, { -143, -119 },
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
        .quadraticBase = { {  -75,  -74 }, {  -59,  -64 }, {  -22,  -31 }, {  -10,   33 }, {   11,   43 }, {   16,   77 }, {   24,   59 }, {   25,   42 }, {   66,  -36 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //BISHOP
        .quadraticBase = { {  -45, -243 }, {  -17, -123 }, {   -5,  -41 }, {    9,    2 }, {   13,   62 }, {   15,  119 }, {   35,  123 }, {   48,  143 }, {   55,  160 }, {   80,  144 }, {  150,  129 },
            {  153,  143 }, {  119,  185 }, {  199,  217 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //ROOK
        .quadraticBase = { {  -82,  -85 }, {  -78,  -71 }, {  -74,  -66 }, {  -63,  -58 }, {  -63,  -29 }, {  -50,   -2 }, {  -49,   29 }, {  -22,   22 }, {   -9,   46 }, {    9,   54 }, {   34,   57 },
            {   45,   62 }, {   66,   65 }, {  129,   20 }, {  208,  -45 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //QUEEN
        .quadraticBase = { {  -91, -656 }, {  -70, -668 }, {  -74, -587 }, {  -77, -506 }, {  -76, -367 }, {  -73, -312 }, {  -79, -225 }, {  -78, -157 }, {  -77, -101 }, {  -81,  -40 }, {  -89,    8 },
            {  -91,   67 }, {  -96,  115 }, { -104,  163 }, {  -90,  179 }, { -108,  230 }, {  -81,  223 }, { -149,  301 }, {  -81,  298 }, { -117,  337 }, {   80,  264 }, {   33,  318 }, {   26,  364 },
            {  138,  270 }, {  -59,  464 }, {  474,  199 }, {  321,  172 }, {  778, -356 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    }
};

PstParameterConstructorSet OutpostPstConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    {   //KNIGHT
        {
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { -26, 56 }, { 40, -21 }, { 19, -18 } },
        },
        { 15,   23 }
    },
    {   //BISHOP
        {
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
        { 0, 0 }
    },
    {   //ROOK
        {
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
        { 0, 0 }
    },
    {   //QUEEN
        {
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
            {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        },
        {
            .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
            .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        },
        { 0, 0 }
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
        {  110,  132 }, {  125,   71 }, {  142,  109 }, {  158,  160 }, {  170,  169 }, {  101,   -4 }, {  125,   74 }, {  -21,  134 },
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

PstParameterConstructorSet PawnDoubledPstConstructorSet = {
    {
        {}, {}, {}, {}, {}, {}, {}, {},
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {}, {}, {}, {}, {}, {}, {}, {},
    },
    {
        .rank = { { 45, -254 }, { 25, -75 }, { 0, 0 } },
        .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { 6 + 0, -83 - 0 }
};

PstParameterConstructorSet PawnPassedPstConstructorSet = {
    {
        {}, {}, {}, {}, {}, {}, {}, {},
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 }, {    0,    0 },
        {}, {}, {}, {}, {}, {}, {}, {},

    },
    {
        .rank = { { 583, 211 }, { 42, 561 }, { 0, 0 } },
        .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { -1, -6 }
};

PstParameterConstructorSet PawnPhalanxPstConstructorSet = {
    {
        {}, {}, {}, {}, {}, {}, {}, {},
        {   57,   49 }, {   50,   38 }, {   59,   46 }, {   65,   55 }, {   65,   55 }, {   60,   51 }, {   28,   18 }, {    0,    0 },
        {   55,   42 }, {   24,   17 }, {   50,   38 }, {   43,   33 }, {   18,   13 }, {   55,   43 }, {   51,   45 }, {    0,    0 },
        {   54,   43 }, {   21,   48 }, {   46,   34 }, {   39,   39 }, {   49,   40 }, {   65,   49 }, {   63,   52 }, {    0,    0 },
        {  -69,   11 }, {  -42,  -62 }, {   20,   19 }, {   -5,   10 }, {   -9,  -15 }, {    5,  -39 }, {   11,   22 }, {    0,    0 },
        {  -49, -111 }, {  -61,  -15 }, {  -55,   36 }, {  -33,   19 }, {  -55,    6 }, {  -24,  -77 }, {  -51, -122 }, {    0,    0 },
        {  -26,   -9 }, {  -14,   29 }, {  -90,  -59 }, {  -50, -101 }, {  -69,   16 }, {  -48,  -42 }, {  -43, -126 }, {    0,    0 },
        {}, {}, {}, {}, {}, {}, {}, {},
    },
    {
        .rank = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .filecenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .rankcenter = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
        .center = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { 62,   87 }
};

QuadraticParameterConstructorSet TropismConstructorSet[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { //KNIGHT
        .quadraticBase = { {    0,    0 }, {  106,  -36 }, {   41,   41 }, {   52,   79 }, {    6,  110 }, {    2,   59 }, {    3,  -12 }, {  -36,  -48 }, {  -49,  -85 }, { -127, -107 }, },
        .quadraticConstruct = { { 0, 0 }, { 0, 0 }, { 0, 0 } },
    },
    { //BISHOP
        .quadraticBase = {},
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

    //{ "material-knight-pair-mg", &PiecePairs[PieceType::KNIGHT].mg},
    //{ "material-knight-pair-eg", &PiecePairs[PieceType::KNIGHT].eg},
    //{ "material-bishop-pair-mg", &PiecePairs[PieceType::BISHOP].mg},
    //{ "material-bishop-pair-eg", &PiecePairs[PieceType::BISHOP].eg},
    //{ "material-rook-pair-mg", &PiecePairs[PieceType::ROOK].mg},
    //{ "material-rook-pair-eg", &PiecePairs[PieceType::ROOK].eg},
    //{ "material-queen-pair-mg", &PiecePairs[PieceType::QUEEN].mg},
    //{ "material-queen-pair-eg", &PiecePairs[PieceType::QUEEN].eg},
     
    { "pst-pawn-rank-quadratic-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.quadratic.mg },
    { "pst-pawn-rank-quadratic-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.quadratic.eg },
    { "pst-pawn-rank-slope-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.slope.mg },
    { "pst-pawn-rank-slope-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.slope.eg },
    { "pst-pawn-rank-yintercept-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.yintercept.mg },
    { "pst-pawn-rank-yintercept-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.rank.yintercept.eg },

    { "pst-pawn-file-center-quadratic-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.quadratic.mg },
    { "pst-pawn-file-center-quadratic-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.quadratic.eg },
    { "pst-pawn-file-center-slope-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.slope.mg },
    { "pst-pawn-file-center-slope-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.slope.eg },
    { "pst-pawn-file-center-yintercept-mg", &BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.yintercept.mg },
    { "pst-pawn-file-center-yintercept-eg",&BasePstConstructorSet[PieceType::PAWN].pstConstruct.filecenter.yintercept.eg },

    //{ "pst-pawn-rank-center-quadratic-mg", &pstConstruct[PieceType::PAWN].rankcenter.quadratic.mg },
    //{ "pst-pawn-rank-center-quadratic-eg",&pstConstruct[PieceType::PAWN].rankcenter.quadratic.eg },
    //{ "pst-pawn-rank-center-slope-mg", &pstConstruct[PieceType::PAWN].rankcenter.slope.mg },
    //{ "pst-pawn-rank-center-slope-eg", &pstConstruct[PieceType::PAWN].rankcenter.slope.eg },
    //{ "pst-pawn-rank-center-yintercept-mg", &pstConstruct[PieceType::PAWN].rankcenter.yintercept.mg },
    //{ "pst-pawn-rank-center-yintercept-eg",&pstConstruct[PieceType::PAWN].rankcenter.yintercept.eg },

    //{ "pst-pawn-center-quadratic-mg", &pstConstruct[PieceType::PAWN].center.quadratic.mg },
    //{ "pst-pawn-center-quadratic-eg",&pstConstruct[PieceType::PAWN].center.quadratic.eg },
    //{ "pst-pawn-center-slope-mg", &pstConstruct[PieceType::PAWN].center.slope.mg },
    //{ "pst-pawn-center-slope-eg", &pstConstruct[PieceType::PAWN].center.slope.eg },
    //{ "pst-pawn-center-yintercept-mg", &pstConstruct[PieceType::PAWN].center.yintercept.mg },
    //{ "pst-pawn-center-yintercept-eg",&pstConstruct[PieceType::PAWN].center.yintercept.eg },

    { "pst-knight-rank-quadratic-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.mg },
    { "pst-knight-rank-quadratic-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.eg },
    { "pst-knight-rank-slope-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.mg },
    { "pst-knight-rank-slope-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.eg },
    { "pst-knight-rank-yintercept-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.mg },
    { "pst-knight-rank-yintercept-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.eg },

    //{ "pst-knight-file-center-quadratic-mg", &pstConstruct[PieceType::KNIGHT].filecenter.quadratic.mg },
    //{ "pst-knight-file-center-quadratic-eg",&pstConstruct[PieceType::KNIGHT].filecenter.quadratic.eg },
    //{ "pst-knight-file-center-slope-mg", &pstConstruct[PieceType::KNIGHT].filecenter.slope.mg },
    //{ "pst-knight-file-center-slope-eg",&pstConstruct[PieceType::KNIGHT].filecenter.slope.eg },
    //{ "pst-knight-file-center-yintercept-mg", &pstConstruct[PieceType::KNIGHT].filecenter.yintercept.mg },
    //{ "pst-knight-file-center-yintercept-eg",&pstConstruct[PieceType::KNIGHT].filecenter.yintercept.eg },

    //{ "pst-knight-rank-center-quadratic-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.quadratic.mg },
    //{ "pst-knight-rank-center-quadratic-eg",&pstConstruct[PieceType::KNIGHT].rankcenter.quadratic.eg },
    //{ "pst-knight-rank-center-slope-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.slope.mg },
    //{ "pst-knight-rank-center-slope-eg", &pstConstruct[PieceType::KNIGHT].rankcenter.slope.eg },
    //{ "pst-knight-rank-center-yintercept-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.yintercept.mg },
    //{ "pst-knight-rank-center-yintercept-eg",&pstConstruct[PieceType::KNIGHT].rankcenter.yintercept.eg },

    { "pst-knight-center-quadratic-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.mg },
    { "pst-knight-center-quadratic-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.eg },
    { "pst-knight-center-slope-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.mg },
    { "pst-knight-center-slope-eg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.eg },
    { "pst-knight-center-yintercept-mg", &BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.mg },
    { "pst-knight-center-yintercept-eg",&BasePstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.eg },

    //{ "pst-bishop-rank-quadratic-mg", &pstConstruct[PieceType::BISHOP].rank.quadratic.mg },
    //{ "pst-bishop-rank-quadratic-eg",&pstConstruct[PieceType::BISHOP].rank.quadratic.eg },
    //{ "pst-bishop-rank-slope-mg", &pstConstruct[PieceType::BISHOP].rank.slope.mg },
    //{ "pst-bishop-rank-slope-eg",&pstConstruct[PieceType::BISHOP].rank.slope.eg },
    //{ "pst-bishop-rank-yintercept-mg", &pstConstruct[PieceType::BISHOP].rank.yintercept.mg },
    //{ "pst-bishop-rank-yintercept-eg",&pstConstruct[PieceType::BISHOP].rank.yintercept.eg },

    //{ "pst-bishop-file-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].filecenter.quadratic.mg },
    //{ "pst-bishop-file-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].filecenter.quadratic.eg },
    //{ "pst-bishop-file-center-slope-mg", &pstConstruct[PieceType::BISHOP].filecenter.slope.mg },
    //{ "pst-bishop-file-center-slope-eg",&pstConstruct[PieceType::BISHOP].filecenter.slope.eg },
    //{ "pst-bishop-file-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].filecenter.yintercept.mg },
    //{ "pst-bishop-file-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].filecenter.yintercept.eg },

    //{ "pst-bishop-rank-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].rankcenter.quadratic.mg },
    //{ "pst-bishop-rank-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].rankcenter.quadratic.eg },
    //{ "pst-bishop-rank-center-slope-mg", &pstConstruct[PieceType::BISHOP].rankcenter.slope.mg },
    //{ "pst-bishop-rank-center-slope-eg", &pstConstruct[PieceType::BISHOP].rankcenter.slope.eg },
    //{ "pst-bishop-rank-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].rankcenter.yintercept.mg },
    //{ "pst-bishop-rank-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].rankcenter.yintercept.eg },

    //{ "pst-bishop-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].center.quadratic.mg },
    //{ "pst-bishop-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].center.quadratic.eg },
    //{ "pst-bishop-center-slope-mg", &pstConstruct[PieceType::BISHOP].center.slope.mg },
    //{ "pst-bishop-center-slope-eg", &pstConstruct[PieceType::BISHOP].center.slope.eg },
    //{ "pst-bishop-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].center.yintercept.mg },
    //{ "pst-bishop-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].center.yintercept.eg },

    //{ "pst-rook-rank-quadratic-mg", &pstConstruct[PieceType::ROOK].rank.quadratic.mg },
    //{ "pst-rook-rank-quadratic-eg",&pstConstruct[PieceType::ROOK].rank.quadratic.eg },
    //{ "pst-rook-rank-slope-mg", &pstConstruct[PieceType::ROOK].rank.slope.mg },
    //{ "pst-rook-rank-slope-eg",&pstConstruct[PieceType::ROOK].rank.slope.eg },
    //{ "pst-rook-rank-yintercept-mg", &pstConstruct[PieceType::ROOK].rank.yintercept.mg },
    //{ "pst-rook-rank-yintercept-eg",&pstConstruct[PieceType::ROOK].rank.yintercept.eg },

    //{ "pst-rook-file-center-quadratic-mg", &pstConstruct[PieceType::ROOK].filecenter.quadratic.mg },
    //{ "pst-rook-file-center-quadratic-eg",&pstConstruct[PieceType::ROOK].filecenter.quadratic.eg },
    //{ "pst-rook-file-center-slope-mg", &pstConstruct[PieceType::ROOK].filecenter.slope.mg },
    //{ "pst-rook-file-center-slope-eg",&pstConstruct[PieceType::ROOK].filecenter.slope.eg },
    //{ "pst-rook-file-center-yintercept-mg", &pstConstruct[PieceType::ROOK].filecenter.yintercept.mg },
    //{ "pst-rook-file-center-yintercept-eg",&pstConstruct[PieceType::ROOK].filecenter.yintercept.eg },

    //{ "pst-rook-rank-center-quadratic-mg", &pstConstruct[PieceType::ROOK].rankcenter.quadratic.mg },
    //{ "pst-rook-rank-center-quadratic-eg",&pstConstruct[PieceType::ROOK].rankcenter.quadratic.eg },
    //{ "pst-rook-rank-center-slope-mg", &pstConstruct[PieceType::ROOK].rankcenter.slope.mg },
    //{ "pst-rook-rank-center-slope-eg", &pstConstruct[PieceType::ROOK].rankcenter.slope.eg },
    //{ "pst-rook-rank-center-yintercept-mg", &pstConstruct[PieceType::ROOK].rankcenter.yintercept.mg },
    //{ "pst-rook-rank-center-yintercept-eg",&pstConstruct[PieceType::ROOK].rankcenter.yintercept.eg },

    //{ "pst-rook-center-quadratic-mg", &pstConstruct[PieceType::ROOK].center.quadratic.mg },
    //{ "pst-rook-center-quadratic-eg",&pstConstruct[PieceType::ROOK].center.quadratic.eg },
    //{ "pst-rook-center-slope-mg", &pstConstruct[PieceType::ROOK].center.slope.mg },
    //{ "pst-rook-center-slope-eg", &pstConstruct[PieceType::ROOK].center.slope.eg },
    //{ "pst-rook-center-yintercept-mg", &pstConstruct[PieceType::ROOK].center.yintercept.mg },
    //{ "pst-rook-center-yintercept-eg",&pstConstruct[PieceType::ROOK].center.yintercept.eg },

    //{ "pst-queen-rank-quadratic-mg", &pstConstruct[PieceType::QUEEN].rank.quadratic.mg },
    //{ "pst-queen-rank-quadratic-eg",&pstConstruct[PieceType::QUEEN].rank.quadratic.eg },
    //{ "pst-queen-rank-slope-mg", &pstConstruct[PieceType::QUEEN].rank.slope.mg },
    //{ "pst-queen-rank-slope-eg",&pstConstruct[PieceType::QUEEN].rank.slope.eg },
    //{ "pst-queen-rank-yintercept-mg", &pstConstruct[PieceType::QUEEN].rank.yintercept.mg },
    //{ "pst-queen-rank-yintercept-eg",&pstConstruct[PieceType::QUEEN].rank.yintercept.eg },

    //{ "pst-queen-file-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].filecenter.quadratic.mg },
    //{ "pst-queen-file-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].filecenter.quadratic.eg },
    //{ "pst-queen-file-center-slope-mg", &pstConstruct[PieceType::QUEEN].filecenter.slope.mg },
    //{ "pst-queen-file-center-slope-eg",&pstConstruct[PieceType::QUEEN].filecenter.slope.eg },
    //{ "pst-queen-file-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].filecenter.yintercept.mg },
    //{ "pst-queen-file-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].filecenter.yintercept.eg },

    //{ "pst-queen-rank-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].rankcenter.quadratic.mg },
    //{ "pst-queen-rank-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].rankcenter.quadratic.eg },
    //{ "pst-queen-rank-center-slope-mg", &pstConstruct[PieceType::QUEEN].rankcenter.slope.mg },
    //{ "pst-queen-rank-center-slope-eg", &pstConstruct[PieceType::QUEEN].rankcenter.slope.eg },
    //{ "pst-queen-rank-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].rankcenter.yintercept.mg },
    //{ "pst-queen-rank-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].rankcenter.yintercept.eg },

    //{ "pst-queen-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].center.quadratic.mg },
    //{ "pst-queen-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].center.quadratic.eg },
    //{ "pst-queen-center-slope-mg", &pstConstruct[PieceType::QUEEN].center.slope.mg },
    //{ "pst-queen-center-slope-eg", &pstConstruct[PieceType::QUEEN].center.slope.eg },
    //{ "pst-queen-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].center.yintercept.mg },
    //{ "pst-queen-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].center.yintercept.eg },

    //{ "pst-king-rank-quadratic-mg", &pstConstruct[PieceType::KING].rank.quadratic.mg },
    //{ "pst-king-rank-quadratic-eg",&pstConstruct[PieceType::KING].rank.quadratic.eg },
    //{ "pst-king-rank-slope-mg", &pstConstruct[PieceType::KING].rank.slope.mg },
    //{ "pst-king-rank-slope-eg",&pstConstruct[PieceType::KING].rank.slope.eg },
    //{ "pst-king-rank-yintercept-mg", &pstConstruct[PieceType::KING].rank.yintercept.mg },
    //{ "pst-king-rank-yintercept-eg",&pstConstruct[PieceType::KING].rank.yintercept.eg },

    //{ "pst-king-file-center-quadratic-mg", &pstConstruct[PieceType::KING].filecenter.quadratic.mg },
    //{ "pst-king-file-center-quadratic-eg",&pstConstruct[PieceType::KING].filecenter.quadratic.eg },
    //{ "pst-king-file-center-slope-mg", &pstConstruct[PieceType::KING].filecenter.slope.mg },
    //{ "pst-king-file-center-slope-eg",&pstConstruct[PieceType::KING].filecenter.slope.eg },
    //{ "pst-king-file-center-yintercept-mg", &pstConstruct[PieceType::KING].filecenter.yintercept.mg },
    //{ "pst-king-file-center-yintercept-eg",&pstConstruct[PieceType::KING].filecenter.yintercept.eg },

    //{ "pst-king-rank-center-quadratic-mg", &pstConstruct[PieceType::KING].rankcenter.quadratic.mg },
    //{ "pst-king-rank-center-quadratic-eg",&pstConstruct[PieceType::KING].rankcenter.quadratic.eg },
    //{ "pst-king-rank-center-slope-mg", &pstConstruct[PieceType::KING].rankcenter.slope.mg },
    //{ "pst-king-rank-center-slope-eg", &pstConstruct[PieceType::KING].rankcenter.slope.eg },
    //{ "pst-king-rank-center-yintercept-mg", &pstConstruct[PieceType::KING].rankcenter.yintercept.mg },
    //{ "pst-king-rank-center-yintercept-eg",&pstConstruct[PieceType::KING].rankcenter.yintercept.eg },

    //{ "pst-king-center-quadratic-mg", &pstConstruct[PieceType::KING].center.quadratic.mg },
    //{ "pst-king-center-quadratic-eg",&pstConstruct[PieceType::KING].center.quadratic.eg },
    //{ "pst-king-center-slope-mg", &pstConstruct[PieceType::KING].center.slope.mg },
    //{ "pst-king-center-slope-eg", &pstConstruct[PieceType::KING].center.slope.eg },
    //{ "pst-king-center-yintercept-mg", &pstConstruct[PieceType::KING].center.yintercept.mg },
    //{ "pst-king-center-yintercept-eg",&pstConstruct[PieceType::KING].center.yintercept.eg },

    //{ "pawn-blocked-current-default-mg", &PawnCurrentColorBlockedDefault.mg },
    //{ "pawn-blocked-current-default-eg", &PawnCurrentColorBlockedDefault.eg },
    //{ "pawn-blocked-current-rank-mg", &pawnCurrentColorBlockedPstConstruct.rank.slope.mg },
    //{ "pawn-blocked-current-rank-eg", &pawnCurrentColorBlockedPstConstruct.rank.slope.eg },
    //{ "pawn-blocked-current-file-center-mg", &pawnCurrentColorBlockedPstConstruct.filecenter.slope.mg },
    //{ "pawn-blocked-current-file-center-eg", &pawnCurrentColorBlockedPstConstruct.filecenter.slope.eg },

    //{ "pawn-blocked-other-default-mg", &PawnOtherColorBlockedDefault.mg },
    //{ "pawn-blocked-other-default-eg", &PawnOtherColorBlockedDefault.eg },
    //{ "pawn-blocked-other-rank-mg", &pawnOtherColorBlockedPstConstruct.rank.slope.mg },
    //{ "pawn-blocked-other-rank-eg", &pawnOtherColorBlockedPstConstruct.rank.slope.eg },
    //{ "pawn-blocked-other-file-center-mg", &pawnOtherColorBlockedPstConstruct.filecenter.slope.mg },
    //{ "pawn-blocked-other-file-center-eg", &pawnOtherColorBlockedPstConstruct.filecenter.slope.eg },

    //{ "pawn-chain-back-default-mg", &PawnChainBackDefault.mg },
    //{ "pawn-chain-back-default-eg", &PawnChainBackDefault.eg },

    //{ "pawn-chain-back-rank-quadratic-mg", &pawnChainBackPstConstruct.rank.quadratic.mg },
    //{ "pawn-chain-back-rank-quadratic-eg", &pawnChainBackPstConstruct.rank.quadratic.eg },
    //{ "pawn-chain-back-rank-slope-mg", &pawnChainBackPstConstruct.rank.slope.mg },
    //{ "pawn-chain-back-rank-slope-eg", &pawnChainBackPstConstruct.rank.slope.eg },

    //{ "pawn-chain-back-file-center-quadratic-mg", &pawnChainBackPstConstruct.filecenter.quadratic.mg },
    //{ "pawn-chain-back-file-center-quadratic-eg", &pawnChainBackPstConstruct.filecenter.quadratic.eg },
    //{ "pawn-chain-back-file-center-slope-mg", &pawnChainBackPstConstruct.filecenter.slope.mg },
    //{ "pawn-chain-back-file-center-slope-eg", &pawnChainBackPstConstruct.filecenter.slope.eg },

    //{ "pawn-chain-front-default-mg", &PawnChainFrontDefault.mg },
    //{ "pawn-chain-front-default-eg", &PawnChainFrontDefault.eg },

    //{ "pawn-chain-front-rank-quadratic-mg", &pawnChainFrontPstConstruct.rank.quadratic.mg },
    //{ "pawn-chain-front-rank-quadratic-eg", &pawnChainFrontPstConstruct.rank.quadratic.eg },
    //{ "pawn-chain-front-rank-slope-mg", &pawnChainFrontPstConstruct.rank.slope.mg },
    //{ "pawn-chain-front-rank-slope-eg", &pawnChainFrontPstConstruct.rank.slope.eg },

    //{ "pawn-chain-front-file-center-quadratic-mg", &pawnChainFrontPstConstruct.filecenter.quadratic.mg },
    //{ "pawn-chain-front-file-center-quadratic-eg", &pawnChainFrontPstConstruct.filecenter.quadratic.eg },
    //{ "pawn-chain-front-file-center-slope-mg", &pawnChainFrontPstConstruct.filecenter.slope.mg },
    //{ "pawn-chain-front-file-center-slope-eg", &pawnChainFrontPstConstruct.filecenter.slope.eg },

    { "pawn-doubled-default-mg", & PawnDoubledPstConstructorSet.defaultEvaluation.mg },
    { "pawn-doubled-default-eg", &PawnDoubledPstConstructorSet.defaultEvaluation.eg },

    { "pawn-doubled-rank-quadratic-mg", &PawnDoubledPstConstructorSet.pstConstruct.rank.quadratic.mg },
    { "pawn-doubled-rank-quadratic-eg",&PawnDoubledPstConstructorSet.pstConstruct.rank.quadratic.eg },
    { "pawn-doubled-rank-slope-mg", &PawnDoubledPstConstructorSet.pstConstruct.rank.slope.mg },
    { "pawn-doubled-rank-slope-eg",&PawnDoubledPstConstructorSet.pstConstruct.rank.slope.eg },

    { "pawn-doubled-file-center-quadratic-mg", &PawnDoubledPstConstructorSet.pstConstruct.filecenter.quadratic.mg },
    { "pawn-doubled-file-center-quadratic-eg",&PawnDoubledPstConstructorSet.pstConstruct.filecenter.quadratic.eg },
    { "pawn-doubled-file-center-slope-mg", &PawnDoubledPstConstructorSet.pstConstruct.filecenter.slope.mg },
    { "pawn-doubled-file-center-slope-eg",&PawnDoubledPstConstructorSet.pstConstruct.filecenter.slope.eg },

    { "pawn-passed-default-mg", &PawnPassedPstConstructorSet.defaultEvaluation.mg },
    { "pawn-passed-default-eg", &PawnPassedPstConstructorSet.defaultEvaluation.eg },

    { "pawn-passed-rank-quadratic-mg", &PawnPassedPstConstructorSet.pstConstruct.rank.quadratic.mg },
    { "pawn-passed-rank-quadratic-eg",&PawnPassedPstConstructorSet.pstConstruct.rank.quadratic.eg },
    { "pawn-passed-rank-slope-mg", &PawnPassedPstConstructorSet.pstConstruct.rank.slope.mg },
    { "pawn-passed-rank-slope-eg",&PawnPassedPstConstructorSet.pstConstruct.rank.slope.eg },

    { "pawn-passed-file-center-quadratic-mg", &PawnPassedPstConstructorSet.pstConstruct.filecenter.quadratic.mg },
    { "pawn-passed-file-center-quadratic-eg",&PawnPassedPstConstructorSet.pstConstruct.filecenter.quadratic.eg },
    { "pawn-passed-file-center-slope-mg", &PawnPassedPstConstructorSet.pstConstruct.filecenter.slope.mg },
    { "pawn-passed-file-center-slope-eg",&PawnPassedPstConstructorSet.pstConstruct.filecenter.slope.eg },

    //{ "pawn-phalanx-default-mg", &PawnPhalanxDefault.mg },
    //{ "pawn-phalanx-default-eg", &PawnPhalanxDefault.eg },

    //{ "pawn-phalanx-rank-quadratic-mg", &pawnPhalanxPstConstruct.rank.quadratic.mg },
    //{ "pawn-phalanx-rank-quadratic-eg", &pawnPhalanxPstConstruct.rank.quadratic.eg },
    //{ "pawn-phalanx-rank-slope-mg", &pawnPhalanxPstConstruct.rank.slope.mg },
    //{ "pawn-phalanx-rank-slope-eg", &pawnPhalanxPstConstruct.rank.slope.eg },

    //{ "pawn-phalanx-file-center-quadratic-mg", &pawnPhalanxPstConstruct.filecenter.quadratic.mg },
    //{ "pawn-phalanx-file-center-quadratic-eg", &pawnPhalanxPstConstruct.filecenter.quadratic.eg },
    //{ "pawn-phalanx-file-center-slope-mg", &pawnPhalanxPstConstruct.filecenter.slope.mg },
    //{ "pawn-phalanx-file-center-slope-eg", &pawnPhalanxPstConstruct.filecenter.slope.eg },

    //{ "pawn-unstoppable-default-mg", &PawnUnstoppableDefault.mg },
    //{ "pawn-unstoppable-default-eg", &PawnUnstoppableDefault.eg },
    //{ "pawn-unstoppable-rank-mg", &pawnUnstoppablePstConstruct.rank.slope.mg },
    //{ "pawn-unstoppable-rank-eg", &pawnUnstoppablePstConstruct.rank.slope.eg },
    //{ "pawn-unstoppable-file-center-mg", &pawnUnstoppablePstConstruct.filecenter.slope.mg },
    //{ "pawn-unstoppable-file-center-eg", &pawnUnstoppablePstConstruct.filecenter.slope.eg },

    { "outpost-knight-center-quadratic-mg", & OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.mg },
    { "outpost-knight-center-quadratic-eg", & OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.quadratic.eg },
    { "outpost-knight-center-slope-mg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.mg },
    { "outpost-knight-center-slope-eg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.slope.eg },
    { "outpost-knight-center-yintercept-mg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.mg },
    { "outpost-knight-center-yintercept-eg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.center.yintercept.eg },

    { "outpost-knight-rank-quadratic-mg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.mg },
    { "outpost-knight-rank-quadratic-eg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.quadratic.eg },
    { "outpost-knight-rank-slope-mg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.mg },
    { "outpost-knight-rank-slope-eg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.slope.eg },
    { "outpost-knight-rank-yintercept-mg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.mg },
    { "outpost-knight-rank-yintercept-eg", &OutpostPstConstructorSet[PieceType::KNIGHT].pstConstruct.rank.yintercept.eg },
        
    //{ "mobility-knight-quadratic-mg", &mobilityConstructor[PieceType::KNIGHT].quadratic.mg },
    //{ "mobility-knight-quadratic-eg", &mobilityConstructor[PieceType::KNIGHT].quadratic.eg },
    //{ "mobility-knight-slope-mg", &mobilityConstructor[PieceType::KNIGHT].slope.mg },
    //{ "mobility-knight-slope-eg", &mobilityConstructor[PieceType::KNIGHT].slope.eg },
    //{ "mobility-knight-yintercept-mg", &mobilityConstructor[PieceType::KNIGHT].yintercept.mg },
    //{ "mobility-knight-yintercept-eg", &mobilityConstructor[PieceType::KNIGHT].yintercept.eg },

    //{ "mobility-bishop-quadratic-mg", &mobilityConstructor[PieceType::BISHOP].quadratic.mg },
    //{ "mobility-bishop-quadratic-eg", &mobilityConstructor[PieceType::BISHOP].quadratic.eg },
    //{ "mobility-bishop-slope-mg", &mobilityConstructor[PieceType::BISHOP].slope.mg },
    //{ "mobility-bishop-slope-eg", &mobilityConstructor[PieceType::BISHOP].slope.eg },
    //{ "mobility-bishop-yintercept-mg", &mobilityConstructor[PieceType::BISHOP].yintercept.mg },
    //{ "mobility-bishop-yintercept-eg", &mobilityConstructor[PieceType::BISHOP].yintercept.eg },

    //{ "mobility-rook-quadratic-mg", &mobilityConstructor[PieceType::ROOK].quadratic.mg },
    //{ "mobility-rook-quadratic-eg", &mobilityConstructor[PieceType::ROOK].quadratic.eg },
    //{ "mobility-rook-slope-mg", &mobilityConstructor[PieceType::ROOK].slope.mg },
    //{ "mobility-rook-slope-eg", &mobilityConstructor[PieceType::ROOK].slope.eg },
    //{ "mobility-rook-yintercept-mg", &mobilityConstructor[PieceType::ROOK].yintercept.mg },
    //{ "mobility-rook-yintercept-eg", &mobilityConstructor[PieceType::ROOK].yintercept.eg },

    //{ "mobility-queen-quadratic-mg", &mobilityConstructor[PieceType::QUEEN].quadratic.mg },
    //{ "mobility-queen-quadratic-eg", &mobilityConstructor[PieceType::QUEEN].quadratic.eg },
    //{ "mobility-queen-slope-mg", &mobilityConstructor[PieceType::QUEEN].slope.mg },
    //{ "mobility-queen-slope-eg", &mobilityConstructor[PieceType::QUEEN].slope.eg },
    //{ "mobility-queen-yintercept-mg", &mobilityConstructor[PieceType::QUEEN].yintercept.mg },
    //{ "mobility-queen-yintercept-eg", &mobilityConstructor[PieceType::QUEEN].yintercept.eg },

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

    //{ "search-quiescence-early-exit-margin-quadratic-mg", &QuiescenceEarlyExitMargin.quadratic.mg },
    //{ "search-quiescence-early-exit-margin-slope-mg", &QuiescenceEarlyExitMargin.slope.mg },
    //{ "search-quiescence-early-exit-margin-yintercept-mg", &QuiescenceEarlyExitMargin.yintercept.mg },

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

    //{ "tropism-knight-quadratic-mg", &tropismConstructor[PieceType::KNIGHT].quadratic.mg },
    //{ "tropism-knight-quadratic-eg", &tropismConstructor[PieceType::KNIGHT].quadratic.eg },
    //{ "tropism-knight-slope-mg", &tropismConstructor[PieceType::KNIGHT].slope.mg },
    //{ "tropism-knight-slope-eg", &tropismConstructor[PieceType::KNIGHT].slope.eg },
    //{ "tropism-knight-yintercept-mg", &tropismConstructor[PieceType::KNIGHT].yintercept.mg },
    //{ "tropism-knight-yintercept-eg", &tropismConstructor[PieceType::KNIGHT].yintercept.eg },

    //{ "tropism-bishop-quadratic-mg", &tropismConstructor[PieceType::BISHOP].quadratic.mg },
    //{ "tropism-bishop-quadratic-eg", &tropismConstructor[PieceType::BISHOP].quadratic.eg },
    //{ "tropism-bishop-slope-mg", &tropismConstructor[PieceType::BISHOP].slope.mg },
    //{ "tropism-bishop-slope-eg", &tropismConstructor[PieceType::BISHOP].slope.eg },
    //{ "tropism-bishop-yintercept-mg", &tropismConstructor[PieceType::BISHOP].yintercept.mg },
    //{ "tropism-bishop-yintercept-eg", &tropismConstructor[PieceType::BISHOP].yintercept.eg },

    //{ "tropism-rook-quadratic-mg", &tropismConstructor[PieceType::ROOK].quadratic.mg },
    //{ "tropism-rook-quadratic-eg", &tropismConstructor[PieceType::ROOK].quadratic.eg },
    //{ "tropism-rook-slope-mg", &tropismConstructor[PieceType::ROOK].slope.mg },
    //{ "tropism-rook-slope-eg", &tropismConstructor[PieceType::ROOK].slope.eg },
    //{ "tropism-rook-yintercept-mg", &tropismConstructor[PieceType::ROOK].yintercept.mg },
    //{ "tropism-rook-yintercept-eg", &tropismConstructor[PieceType::ROOK].yintercept.eg },

    //{ "tropism-queen-quadratic-mg", &tropismConstructor[PieceType::QUEEN].quadratic.mg },
    //{ "tropism-queen-quadratic-eg", &tropismConstructor[PieceType::QUEEN].quadratic.eg },
    //{ "tropism-queen-slope-mg", &tropismConstructor[PieceType::QUEEN].slope.mg },
    //{ "tropism-queen-slope-eg", &tropismConstructor[PieceType::QUEEN].slope.eg },
    //{ "tropism-queen-yintercept-mg", &tropismConstructor[PieceType::QUEEN].yintercept.mg },
    //{ "tropism-queen-yintercept-eg", &tropismConstructor[PieceType::QUEEN].yintercept.eg },

    //{ "doubled-rooks-mg", &DoubledRooks.mg },
    //{ "doubled-rooks-eg", &DoubledRooks.eg },

    //{ "empty-file-queen-mg", &EmptyFileQueen.mg },
    //{ "empty-file-queen-eg", &EmptyFileQueen.eg },

    //{ "empty-file-rook-mg", &EmptyFileRook.mg },
    //{ "empty-file-rook-eg", &EmptyFileRook.eg },

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
        scoreConstructor.construct(OutpostPstParameters[pieceType], OutpostPstConstructorSet[pieceType]);

        scoreConstructor.construct(&(MobilityParameters[pieceType][0]), MobilityConstructorSet[pieceType], 32);
        scoreConstructor.construct(&(TropismParameters[pieceType][0]), TropismConstructorSet[pieceType], 16);
    }

    scoreConstructor.construct(PawnChainBackPstParameters, PawnChainBackPstConstructorSet);
    scoreConstructor.construct(PawnChainFrontPstParameters, PawnChainFrontPstConstructorSet);
    scoreConstructor.construct(PawnDoubledPstParameters, PawnDoubledPstConstructorSet);
    scoreConstructor.construct(PawnPassedPstParameters, PawnPassedPstConstructorSet);
    scoreConstructor.construct(PawnPhalanxPstParameters, PawnPhalanxPstConstructorSet);

    for (const Square src : SquareIterator()) {
        const File file = GetFile(src);
        const Rank rank = GetRank(src);

        Distance[file][rank] = std::uint32_t(std::sqrt(file * file + rank * rank));
    }
}
