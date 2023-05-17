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

ChessEvaluation MaterialParameters[PieceType::PIECETYPE_COUNT] = {
	{ },
	{ PAWN_SCORE, PAWN_SCORE },
	{ KNIGHT_SCORE, KNIGHT_SCORE },
	{ BISHOP_SCORE, BISHOP_SCORE },
	{ ROOK_SCORE, ROOK_SCORE },
	{ QUEEN_SCORE, QUEEN_SCORE }
};

ChessEvaluation PiecePairs[PieceType::PIECETYPE_COUNT];

QuadraticConstruct FutilityPruningMargin = {
    {  12,  12 },
    { 442, 442 },
    { 291, 291 }
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
    { 158, 158 },
    { 122,  26 },
    {  96,  20 }
};

ChessEvaluation LazyEvaluationThreshold = {
	3 * PAWN_SCORE, 3 * PAWN_SCORE
};

QuadraticConstruct NullMoveMargins = {
	{   -9,   -9 },
	{    9,   -9 },
	{ -119, -119 }
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
    { 0, 0 },
    { 0, 0 },
    { 512, 512 }
};

QuadraticConstruct PruningMarginSearchedMoves = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};

QuadraticConstruct QuiescenceEarlyExitMargin = {
	{ 0, 0 },
	{ 0, 0 },
	{ 512, 512 }
};

QuadraticConstruct QuiescenceStaticExchangeEvaluationMargin = {
	{ 0, 0 },
	{ 0, 0 },
	{ -256, -256 }
};

QuadraticConstruct ProbCutPruningMargin = {
	{ 0, 0 },
	{ 200, 0 },
	{ 300, 0 }
};

QuadraticConstruct RazoringMargin = {
	{ 0, 0 },
	{ 0, 0 },
	{ 600, 600 }
};

ChessEvaluation DoubledRooks;
ChessEvaluation EmptyFileQueen;
ChessEvaluation EmptyFileRook;
ChessEvaluation QueenBehindPassedPawnPst[Square::SQUARE_COUNT];
ChessEvaluation RookBehindPassedPawnPst[Square::SQUARE_COUNT];

ChessEvaluation PawnBlockedPstParameters[Color::COLOR_COUNT][Square::SQUARE_COUNT];
ChessEvaluation PawnChainBackPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnChainFrontPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnDoubledPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnPassedPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnPhalanxPstParameters[Square::SQUARE_COUNT];
ChessEvaluation PawnUnstoppablePstParameters[Square::SQUARE_COUNT];

ChessEvaluation PersonalityPstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];
ChessEvaluation PstParameters[PieceType::PIECETYPE_COUNT][Square::SQUARE_COUNT];

PstConstruct pstConstruct[PieceType::PIECETYPE_COUNT] = {
    {},
    { //PAWN
        .rank =       { {  26,  894 }, {  222,  722 }, {  -3, -46 } },
        .filecenter = { {   9,  200 }, {  212, -353 }, {  52,  62 } },
        .rankcenter = { { 104, -113 }, {  -55,  -95 }, {   4,  -6 } },
        .center =     { {   1,    9 }, { -466, -218 }, { -10,  23 } },
    },
    { //KNIGHT
        .rank =       { {  -60,   3 }, {  -82, -123 }, {  46,  69 } },
        .filecenter = { {   65, -23 }, {   87,  -30 }, {   0,   0 } },
        .rankcenter = { { -143,  53 }, {  227,   64 }, {   0,   0 } },
        .center =     { { -152,  55 }, {  428,   73 }, { -76, -15 } },
    },
    { //BISHOP
        .rank =       { { -511, -204 }, {  313,  299 }, {  55,  52 } },
        .filecenter = { {    1,  -51 }, { -141, -103 }, {   0,   0 } },
        .rankcenter = { {   -5,  -48 }, { -123,  -67 }, { -35,  -7 } },
        .center =     { {  -62,    8 }, {  591,  551 }, {   7, -31 } },
    },
    { //ROOK
        .rank =       { { 201, 173 }, {  94, -44 }, { -124,   55 } },
        .filecenter = { {  24, -47 }, {  82,  66 }, {  135, -189 } },
        .rankcenter = { { -50,  34 }, { -73,  71 }, {    0,    0 } },
        .center =     { {  49, 116 }, { -85,  35 }, {  228, -241 } },
    },
    { //QUEEN
        .rank =       { {  61, -94 }, {  68, 227 }, { -141, 237 } },
        .filecenter = { { -18, -15 }, { 162,  47 }, {   0,    0 } },
        .rankcenter = { { -96, 197 }, { 110,  24 }, {   8, -236 } },
        .center =     { { 151, -12 }, { -50,  37 }, {  18, -183 } },
    },
    { //KING
        .rank =       { {  17, 271 }, { -177,  47 }, { 0, 0 } },
        .filecenter = { {  -1, -34 }, {  -53, 108 }, { 0, 0 } },
        .rankcenter = { {  -4,  15 }, {   10, 176 }, { 0, 0 } },
        .center =     { { -53, 140 }, { -170, 131 }, { 0, 0 } },
    },
};

ChessEvaluation MobilityParameters[PieceType::PIECETYPE_COUNT][32];
QuadraticConstruct mobilityConstructor[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { { -286, -390 }, { 546, 599 }, {  -95, -124 } }, //KNIGHT
    { { -325, -124 }, { 531, 535 }, {  -46, -197 } }, //BISHOP
    { { -253, -246 }, { 561, 754 }, { -156, -121 } }, //ROOK
    { { -164, -151 }, { 462, 478 }, { -114, -166 } }, //QUEEN
};

ChessEvaluation AttackParameters[PieceType::PIECETYPE_COUNT][PieceType::PIECETYPE_COUNT] = {
    {},   //PAWN        KNIGHT        BISHOP       ROOK          QUEEN          is attacked by...
//    { {}, {}, { 93, 252 }, { 58, 314 }, { -26, 523 }, { -40, 544 }, },
    { {}, {}, { 258, -52 }, { 495, 5 }, { 274, -63 }, { 232, 255 }, },
    { {}, { -16, 27 }, {}, { 28, 125 }, { 79, 150 }, { -39, 423 }, },
    { {}, { -9, 44 }, { 21, 159 }, {}, { 37, 299 }, { -20, 744 }, },
    { {}, { -1, 39 }, { 16, 81 }, { 35, 81 }, {}, { -4, 689 }, },
    { {}, { -4, 9 }, { 2, 16 }, { 8, 43 }, { -35, 111 }, {}, },
};

ChessEvaluation TropismParameters[PieceType::PIECETYPE_COUNT][16];
QuadraticConstruct tropismConstructor[PieceType::PIECETYPE_COUNT] = {
    {}, {},
    { {  14,  99 }, {   -6, -36 }, {   57,   26 } }, //KNIGHT
    { {  47, -20 }, {  214,  63 }, {  -68,   59 } }, //BISHOP
    { { -22, 204 }, { -218, 136 }, {  199, -129 } }, //ROOK
    { {  68,  87 }, { -332, -78 }, { -144,  -44 } }, //QUEEN
};

ChessEvaluation PawnCurrentColorBlockedDefault;
PstConstruct pawnCurrentColorBlockedPstConstruct;

ChessEvaluation PawnOtherColorBlockedDefault;
PstConstruct pawnOtherColorBlockedPstConstruct;

ChessEvaluation PawnChainBackDefault = { 3, -7 };
PstConstruct pawnChainBackPstConstruct = {
    .rank = { { -15, 117 }, { 97, -49 } }
};

ChessEvaluation PawnChainFrontDefault = { 36, 19 };
PstConstruct pawnChainFrontPstConstruct = {
    .rank = { { -133, -46 }, { -30, -96 } }
};

ChessEvaluation PawnDoubledDefault = { -84, -65 };
PstConstruct pawnDoubledPstConstruct = {
    .rank = { { 144, -281 }, { -24, 116 } }
};

ChessEvaluation PawnPassedDefault = { -77, 28 };
PstConstruct pawnPassedPstConstruct = {
    .rank = { { 118, 157 }, { 134, 203 } }
};

ChessEvaluation PawnPhalanxDefault = { 29, -24 };
PstConstruct pawnPhalanxPstConstruct = {
    .rank = { { 25, -63 }, { -61, 16 } }
};

ChessEvaluation PawnUnstoppableDefault;
PstConstruct pawnUnstoppablePstConstruct;

ChessEvaluation queenBehindPassedPawnDefault;
PstConstruct queenBehindPassedPawnPstConstruct;

ChessEvaluation rookBehindPassedPawnDefault;
PstConstruct rookBehindPassedPawnPstConstruct;

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

    { "pst-pawn-rank-quadratic-mg", &pstConstruct[PieceType::PAWN].rank.quadratic.mg },
    { "pst-pawn-rank-quadratic-eg",&pstConstruct[PieceType::PAWN].rank.quadratic.eg },
    { "pst-pawn-rank-slope-mg", &pstConstruct[PieceType::PAWN].rank.slope.mg },
    { "pst-pawn-rank-slope-eg",&pstConstruct[PieceType::PAWN].rank.slope.eg },
    { "pst-pawn-rank-yintercept-mg", &pstConstruct[PieceType::PAWN].rank.yintercept.mg },
    { "pst-pawn-rank-yintercept-eg",&pstConstruct[PieceType::PAWN].rank.yintercept.eg },

	{ "pst-pawn-file-center-quadratic-mg", &pstConstruct[PieceType::PAWN].filecenter.quadratic.mg },
    { "pst-pawn-file-center-quadratic-eg",&pstConstruct[PieceType::PAWN].filecenter.quadratic.eg },
    { "pst-pawn-file-center-slope-mg", &pstConstruct[PieceType::PAWN].filecenter.slope.mg },
    { "pst-pawn-file-center-slope-eg",&pstConstruct[PieceType::PAWN].filecenter.slope.eg },
    { "pst-pawn-file-center-yintercept-mg", &pstConstruct[PieceType::PAWN].filecenter.yintercept.mg },
    { "pst-pawn-file-center-yintercept-eg",&pstConstruct[PieceType::PAWN].filecenter.yintercept.eg },

	{ "pst-pawn-rank-center-quadratic-mg", &pstConstruct[PieceType::PAWN].rankcenter.quadratic.mg },
    { "pst-pawn-rank-center-quadratic-eg",&pstConstruct[PieceType::PAWN].rankcenter.quadratic.eg },
    { "pst-pawn-rank-center-slope-mg", &pstConstruct[PieceType::PAWN].rankcenter.slope.mg },
    { "pst-pawn-rank-center-slope-eg", &pstConstruct[PieceType::PAWN].rankcenter.slope.eg },
    { "pst-pawn-rank-center-yintercept-mg", &pstConstruct[PieceType::PAWN].rankcenter.yintercept.mg },
    { "pst-pawn-rank-center-yintercept-eg",&pstConstruct[PieceType::PAWN].rankcenter.yintercept.eg },
    
    { "pst-pawn-center-quadratic-mg", &pstConstruct[PieceType::PAWN].center.quadratic.mg },
    { "pst-pawn-center-quadratic-eg",&pstConstruct[PieceType::PAWN].center.quadratic.eg },
    { "pst-pawn-center-slope-mg", &pstConstruct[PieceType::PAWN].center.slope.mg },
    { "pst-pawn-center-slope-eg", &pstConstruct[PieceType::PAWN].center.slope.eg },
    { "pst-pawn-center-yintercept-mg", &pstConstruct[PieceType::PAWN].center.yintercept.mg },
    { "pst-pawn-center-yintercept-eg",&pstConstruct[PieceType::PAWN].center.yintercept.eg },

	{ "pst-pawn-square-a2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A2].mg },
	{ "pst-pawn-square-a2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A2].eg },
	{ "pst-pawn-square-b2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B2].mg },
	{ "pst-pawn-square-b2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B2].eg },
	{ "pst-pawn-square-c2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C2].mg },
	{ "pst-pawn-square-c2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C2].eg },
	{ "pst-pawn-square-d2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D2].mg },
	{ "pst-pawn-square-d2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D2].eg },
	{ "pst-pawn-square-e2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E2].mg },
	{ "pst-pawn-square-e2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E2].eg },
	{ "pst-pawn-square-f2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F2].mg },
	{ "pst-pawn-square-f2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F2].eg },
	{ "pst-pawn-square-g2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G2].mg },
	{ "pst-pawn-square-g2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G2].eg },
	{ "pst-pawn-square-h2-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H2].mg },
	{ "pst-pawn-square-h2-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H2].eg },

	{ "pst-pawn-square-a3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A3].mg },
	{ "pst-pawn-square-a3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A3].eg },
	{ "pst-pawn-square-b3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B3].mg },
	{ "pst-pawn-square-b3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B3].eg },
	{ "pst-pawn-square-c3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C3].mg },
	{ "pst-pawn-square-c3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C3].eg },
	{ "pst-pawn-square-d3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D3].mg },
	{ "pst-pawn-square-d3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D3].eg },
	{ "pst-pawn-square-e3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E3].mg },
	{ "pst-pawn-square-e3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E3].eg },
	{ "pst-pawn-square-f3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F3].mg },
	{ "pst-pawn-square-f3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F3].eg },
	{ "pst-pawn-square-g3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G3].mg },
	{ "pst-pawn-square-g3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G3].eg },
	{ "pst-pawn-square-h3-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H3].mg },
	{ "pst-pawn-square-h3-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H3].eg },

	{ "pst-pawn-square-a4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A4].mg },
	{ "pst-pawn-square-a4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A4].eg },
	{ "pst-pawn-square-b4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B4].mg },
	{ "pst-pawn-square-b4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B4].eg },
	{ "pst-pawn-square-c4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C4].mg },
	{ "pst-pawn-square-c4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C4].eg },
	{ "pst-pawn-square-d4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D4].mg },
	{ "pst-pawn-square-d4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D4].eg },
	{ "pst-pawn-square-e4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E4].mg },
	{ "pst-pawn-square-e4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E4].eg },
	{ "pst-pawn-square-f4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F4].mg },
	{ "pst-pawn-square-f4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F4].eg },
	{ "pst-pawn-square-g4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G4].mg },
	{ "pst-pawn-square-g4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G4].eg },
	{ "pst-pawn-square-h4-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H4].mg },
	{ "pst-pawn-square-h4-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H4].eg },

	{ "pst-pawn-square-a5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A5].mg },
	{ "pst-pawn-square-a5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A5].eg },
	{ "pst-pawn-square-b5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B5].mg },
	{ "pst-pawn-square-b5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B5].eg },
	{ "pst-pawn-square-c5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C5].mg },
	{ "pst-pawn-square-c5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C5].eg },
	{ "pst-pawn-square-d5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D5].mg },
	{ "pst-pawn-square-d5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D5].eg },
	{ "pst-pawn-square-e5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E5].mg },
	{ "pst-pawn-square-e5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E5].eg },
	{ "pst-pawn-square-f5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F5].mg },
	{ "pst-pawn-square-f5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F5].eg },
	{ "pst-pawn-square-g5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G5].mg },
	{ "pst-pawn-square-g5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G5].eg },
	{ "pst-pawn-square-h5-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H5].mg },
	{ "pst-pawn-square-h5-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H5].eg },

	{ "pst-pawn-square-a6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A6].mg },
	{ "pst-pawn-square-a6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A6].eg },
	{ "pst-pawn-square-b6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B6].mg },
	{ "pst-pawn-square-b6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B6].eg },
	{ "pst-pawn-square-c6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C6].mg },
	{ "pst-pawn-square-c6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C6].eg },
	{ "pst-pawn-square-d6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D6].mg },
	{ "pst-pawn-square-d6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D6].eg },
	{ "pst-pawn-square-e6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E6].mg },
	{ "pst-pawn-square-e6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E6].eg },
	{ "pst-pawn-square-f6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F6].mg },
	{ "pst-pawn-square-f6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F6].eg },
	{ "pst-pawn-square-g6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G6].mg },
	{ "pst-pawn-square-g6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G6].eg },
	{ "pst-pawn-square-h6-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H6].mg },
	{ "pst-pawn-square-h6-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H6].eg },

	{ "pst-pawn-square-a7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::A7].mg },
	{ "pst-pawn-square-a7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::A7].eg },
	{ "pst-pawn-square-b7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::B7].mg },
	{ "pst-pawn-square-b7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::B7].eg },
	{ "pst-pawn-square-c7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::C7].mg },
	{ "pst-pawn-square-c7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::C7].eg },
	{ "pst-pawn-square-d7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::D7].mg },
	{ "pst-pawn-square-d7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::D7].eg },
	{ "pst-pawn-square-e7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::E7].mg },
	{ "pst-pawn-square-e7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::E7].eg },
	{ "pst-pawn-square-f7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::F7].mg },
	{ "pst-pawn-square-f7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::F7].eg },
	{ "pst-pawn-square-g7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::G7].mg },
	{ "pst-pawn-square-g7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::G7].eg },
	{ "pst-pawn-square-h7-mg", &PersonalityPstParameters[PieceType::PAWN][Square::H7].mg },
	{ "pst-pawn-square-h7-eg", &PersonalityPstParameters[PieceType::PAWN][Square::H7].eg },

	{ "pst-knight-rank-quadratic-mg", &pstConstruct[PieceType::KNIGHT].rank.quadratic.mg },
    { "pst-knight-rank-quadratic-eg",&pstConstruct[PieceType::KNIGHT].rank.quadratic.eg },
    { "pst-knight-rank-slope-mg", &pstConstruct[PieceType::KNIGHT].rank.slope.mg },
    { "pst-knight-rank-slope-eg",&pstConstruct[PieceType::KNIGHT].rank.slope.eg },
    { "pst-knight-rank-yintercept-mg", &pstConstruct[PieceType::KNIGHT].rank.yintercept.mg },
    { "pst-knight-rank-yintercept-eg",&pstConstruct[PieceType::KNIGHT].rank.yintercept.eg },

	{ "pst-knight-file-center-quadratic-mg", &pstConstruct[PieceType::KNIGHT].filecenter.quadratic.mg },
    { "pst-knight-file-center-quadratic-eg",&pstConstruct[PieceType::KNIGHT].filecenter.quadratic.eg },
    { "pst-knight-file-center-slope-mg", &pstConstruct[PieceType::KNIGHT].filecenter.slope.mg },
    { "pst-knight-file-center-slope-eg",&pstConstruct[PieceType::KNIGHT].filecenter.slope.eg },
    { "pst-knight-file-center-yintercept-mg", &pstConstruct[PieceType::KNIGHT].filecenter.yintercept.mg },
    { "pst-knight-file-center-yintercept-eg",&pstConstruct[PieceType::KNIGHT].filecenter.yintercept.eg },

	{ "pst-knight-rank-center-quadratic-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.quadratic.mg },
    { "pst-knight-rank-center-quadratic-eg",&pstConstruct[PieceType::KNIGHT].rankcenter.quadratic.eg },
    { "pst-knight-rank-center-slope-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.slope.mg },
    { "pst-knight-rank-center-slope-eg", &pstConstruct[PieceType::KNIGHT].rankcenter.slope.eg },
    { "pst-knight-rank-center-yintercept-mg", &pstConstruct[PieceType::KNIGHT].rankcenter.yintercept.mg },
    { "pst-knight-rank-center-yintercept-eg",&pstConstruct[PieceType::KNIGHT].rankcenter.yintercept.eg },

	{ "pst-knight-center-quadratic-mg", &pstConstruct[PieceType::KNIGHT].center.quadratic.mg },
    { "pst-knight-center-quadratic-eg",&pstConstruct[PieceType::KNIGHT].center.quadratic.eg },
    { "pst-knight-center-slope-mg", &pstConstruct[PieceType::KNIGHT].center.slope.mg },
    { "pst-knight-center-slope-eg", &pstConstruct[PieceType::KNIGHT].center.slope.eg },
    { "pst-knight-center-yintercept-mg", &pstConstruct[PieceType::KNIGHT].center.yintercept.mg },
    { "pst-knight-center-yintercept-eg",&pstConstruct[PieceType::KNIGHT].center.yintercept.eg },

	{ "pst-knight-square-a1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A1].mg },
	{ "pst-knight-square-a1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A1].eg },
	{ "pst-knight-square-b1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B1].mg },
	{ "pst-knight-square-b1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B1].eg },
	{ "pst-knight-square-c1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C1].mg },
	{ "pst-knight-square-c1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C1].eg },
	{ "pst-knight-square-d1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D1].mg },
	{ "pst-knight-square-d1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D1].eg },
	{ "pst-knight-square-e1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E1].mg },
	{ "pst-knight-square-e1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E1].eg },
	{ "pst-knight-square-f1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F1].mg },
	{ "pst-knight-square-f1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F1].eg },
	{ "pst-knight-square-g1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G1].mg },
	{ "pst-knight-square-g1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G1].eg },
	{ "pst-knight-square-h1-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H1].mg },
	{ "pst-knight-square-h1-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H1].eg },

	{ "pst-knight-square-a2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A2].mg },
	{ "pst-knight-square-a2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A2].eg },
	{ "pst-knight-square-b2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B2].mg },
	{ "pst-knight-square-b2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B2].eg },
	{ "pst-knight-square-c2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C2].mg },
	{ "pst-knight-square-c2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C2].eg },
	{ "pst-knight-square-d2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D2].mg },
	{ "pst-knight-square-d2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D2].eg },
	{ "pst-knight-square-e2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E2].mg },
	{ "pst-knight-square-e2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E2].eg },
	{ "pst-knight-square-f2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F2].mg },
	{ "pst-knight-square-f2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F2].eg },
	{ "pst-knight-square-g2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G2].mg },
	{ "pst-knight-square-g2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G2].eg },
	{ "pst-knight-square-h2-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H2].mg },
	{ "pst-knight-square-h2-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H2].eg },

	{ "pst-knight-square-a3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A3].mg },
	{ "pst-knight-square-a3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A3].eg },
	{ "pst-knight-square-b3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B3].mg },
	{ "pst-knight-square-b3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B3].eg },
	{ "pst-knight-square-c3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C3].mg },
	{ "pst-knight-square-c3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C3].eg },
	{ "pst-knight-square-d3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D3].mg },
	{ "pst-knight-square-d3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D3].eg },
	{ "pst-knight-square-e3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E3].mg },
	{ "pst-knight-square-e3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E3].eg },
	{ "pst-knight-square-f3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F3].mg },
	{ "pst-knight-square-f3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F3].eg },
	{ "pst-knight-square-g3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G3].mg },
	{ "pst-knight-square-g3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G3].eg },
	{ "pst-knight-square-h3-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H3].mg },
	{ "pst-knight-square-h3-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H3].eg },

	{ "pst-knight-square-a4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A4].mg },
	{ "pst-knight-square-a4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A4].eg },
	{ "pst-knight-square-b4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B4].mg },
	{ "pst-knight-square-b4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B4].eg },
	{ "pst-knight-square-c4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C4].mg },
	{ "pst-knight-square-c4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C4].eg },
	{ "pst-knight-square-d4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D4].mg },
	{ "pst-knight-square-d4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D4].eg },
	{ "pst-knight-square-e4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E4].mg },
	{ "pst-knight-square-e4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E4].eg },
	{ "pst-knight-square-f4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F4].mg },
	{ "pst-knight-square-f4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F4].eg },
	{ "pst-knight-square-g4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G4].mg },
	{ "pst-knight-square-g4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G4].eg },
	{ "pst-knight-square-h4-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H4].mg },
	{ "pst-knight-square-h4-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H4].eg },

	{ "pst-knight-square-a5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A5].mg },
	{ "pst-knight-square-a5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A5].eg },
	{ "pst-knight-square-b5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B5].mg },
	{ "pst-knight-square-b5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B5].eg },
	{ "pst-knight-square-c5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C5].mg },
	{ "pst-knight-square-c5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C5].eg },
	{ "pst-knight-square-d5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D5].mg },
	{ "pst-knight-square-d5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D5].eg },
	{ "pst-knight-square-e5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E5].mg },
	{ "pst-knight-square-e5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E5].eg },
	{ "pst-knight-square-f5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F5].mg },
	{ "pst-knight-square-f5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F5].eg },
	{ "pst-knight-square-g5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G5].mg },
	{ "pst-knight-square-g5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G5].eg },
	{ "pst-knight-square-h5-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H5].mg },
	{ "pst-knight-square-h5-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H5].eg },

	{ "pst-knight-square-a6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A6].mg },
	{ "pst-knight-square-a6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A6].eg },
	{ "pst-knight-square-b6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B6].mg },
	{ "pst-knight-square-b6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B6].eg },
	{ "pst-knight-square-c6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C6].mg },
	{ "pst-knight-square-c6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C6].eg },
	{ "pst-knight-square-d6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D6].mg },
	{ "pst-knight-square-d6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D6].eg },
	{ "pst-knight-square-e6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E6].mg },
	{ "pst-knight-square-e6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E6].eg },
	{ "pst-knight-square-f6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F6].mg },
	{ "pst-knight-square-f6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F6].eg },
	{ "pst-knight-square-g6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G6].mg },
	{ "pst-knight-square-g6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G6].eg },
	{ "pst-knight-square-h6-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H6].mg },
	{ "pst-knight-square-h6-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H6].eg },

	{ "pst-knight-square-a7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A7].mg },
	{ "pst-knight-square-a7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A7].eg },
	{ "pst-knight-square-b7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B7].mg },
	{ "pst-knight-square-b7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B7].eg },
	{ "pst-knight-square-c7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C7].mg },
	{ "pst-knight-square-c7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C7].eg },
	{ "pst-knight-square-d7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D7].mg },
	{ "pst-knight-square-d7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D7].eg },
	{ "pst-knight-square-e7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E7].mg },
	{ "pst-knight-square-e7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E7].eg },
	{ "pst-knight-square-f7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F7].mg },
	{ "pst-knight-square-f7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F7].eg },
	{ "pst-knight-square-g7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G7].mg },
	{ "pst-knight-square-g7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G7].eg },
	{ "pst-knight-square-h7-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H7].mg },
	{ "pst-knight-square-h7-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H7].eg },

	{ "pst-knight-square-a8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A8].mg },
	{ "pst-knight-square-a8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::A8].eg },
	{ "pst-knight-square-b8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B8].mg },
	{ "pst-knight-square-b8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::B8].eg },
	{ "pst-knight-square-c8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C8].mg },
	{ "pst-knight-square-c8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::C8].eg },
	{ "pst-knight-square-d8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D8].mg },
	{ "pst-knight-square-d8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::D8].eg },
	{ "pst-knight-square-e8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E8].mg },
	{ "pst-knight-square-e8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::E8].eg },
	{ "pst-knight-square-f8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F8].mg },
	{ "pst-knight-square-f8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::F8].eg },
	{ "pst-knight-square-g8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G8].mg },
	{ "pst-knight-square-g8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::G8].eg },
	{ "pst-knight-square-h8-mg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H8].mg },
	{ "pst-knight-square-h8-eg", &PersonalityPstParameters[PieceType::KNIGHT][Square::H8].eg },

	{ "pst-bishop-rank-quadratic-mg", &pstConstruct[PieceType::BISHOP].rank.quadratic.mg },
    { "pst-bishop-rank-quadratic-eg",&pstConstruct[PieceType::BISHOP].rank.quadratic.eg },
    { "pst-bishop-rank-slope-mg", &pstConstruct[PieceType::BISHOP].rank.slope.mg },
    { "pst-bishop-rank-slope-eg",&pstConstruct[PieceType::BISHOP].rank.slope.eg },
    { "pst-bishop-rank-yintercept-mg", &pstConstruct[PieceType::BISHOP].rank.yintercept.mg },
    { "pst-bishop-rank-yintercept-eg",&pstConstruct[PieceType::BISHOP].rank.yintercept.eg },

	{ "pst-bishop-file-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].filecenter.quadratic.mg },
    { "pst-bishop-file-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].filecenter.quadratic.eg },
    { "pst-bishop-file-center-slope-mg", &pstConstruct[PieceType::BISHOP].filecenter.slope.mg },
    { "pst-bishop-file-center-slope-eg",&pstConstruct[PieceType::BISHOP].filecenter.slope.eg },
    { "pst-bishop-file-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].filecenter.yintercept.mg },
    { "pst-bishop-file-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].filecenter.yintercept.eg },

	{ "pst-bishop-rank-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].rankcenter.quadratic.mg },
    { "pst-bishop-rank-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].rankcenter.quadratic.eg },
    { "pst-bishop-rank-center-slope-mg", &pstConstruct[PieceType::BISHOP].rankcenter.slope.mg },
    { "pst-bishop-rank-center-slope-eg", &pstConstruct[PieceType::BISHOP].rankcenter.slope.eg },
    { "pst-bishop-rank-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].rankcenter.yintercept.mg },
    { "pst-bishop-rank-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].rankcenter.yintercept.eg },

	{ "pst-bishop-center-quadratic-mg", &pstConstruct[PieceType::BISHOP].center.quadratic.mg },
    { "pst-bishop-center-quadratic-eg",&pstConstruct[PieceType::BISHOP].center.quadratic.eg },
    { "pst-bishop-center-slope-mg", &pstConstruct[PieceType::BISHOP].center.slope.mg },
    { "pst-bishop-center-slope-eg", &pstConstruct[PieceType::BISHOP].center.slope.eg },
    { "pst-bishop-center-yintercept-mg", &pstConstruct[PieceType::BISHOP].center.yintercept.mg },
    { "pst-bishop-center-yintercept-eg",&pstConstruct[PieceType::BISHOP].center.yintercept.eg },

	{ "pst-rook-rank-quadratic-mg", &pstConstruct[PieceType::ROOK].rank.quadratic.mg },
    { "pst-rook-rank-quadratic-eg",&pstConstruct[PieceType::ROOK].rank.quadratic.eg },
    { "pst-rook-rank-slope-mg", &pstConstruct[PieceType::ROOK].rank.slope.mg },
    { "pst-rook-rank-slope-eg",&pstConstruct[PieceType::ROOK].rank.slope.eg },
    { "pst-rook-rank-yintercept-mg", &pstConstruct[PieceType::ROOK].rank.yintercept.mg },
    { "pst-rook-rank-yintercept-eg",&pstConstruct[PieceType::ROOK].rank.yintercept.eg },

	{ "pst-rook-file-center-quadratic-mg", &pstConstruct[PieceType::ROOK].filecenter.quadratic.mg },
    { "pst-rook-file-center-quadratic-eg",&pstConstruct[PieceType::ROOK].filecenter.quadratic.eg },
    { "pst-rook-file-center-slope-mg", &pstConstruct[PieceType::ROOK].filecenter.slope.mg },
    { "pst-rook-file-center-slope-eg",&pstConstruct[PieceType::ROOK].filecenter.slope.eg },
    { "pst-rook-file-center-yintercept-mg", &pstConstruct[PieceType::ROOK].filecenter.yintercept.mg },
    { "pst-rook-file-center-yintercept-eg",&pstConstruct[PieceType::ROOK].filecenter.yintercept.eg },

	{ "pst-rook-rank-center-quadratic-mg", &pstConstruct[PieceType::ROOK].rankcenter.quadratic.mg },
    { "pst-rook-rank-center-quadratic-eg",&pstConstruct[PieceType::ROOK].rankcenter.quadratic.eg },
    { "pst-rook-rank-center-slope-mg", &pstConstruct[PieceType::ROOK].rankcenter.slope.mg },
    { "pst-rook-rank-center-slope-eg", &pstConstruct[PieceType::ROOK].rankcenter.slope.eg },
    { "pst-rook-rank-center-yintercept-mg", &pstConstruct[PieceType::ROOK].rankcenter.yintercept.mg },
    { "pst-rook-rank-center-yintercept-eg",&pstConstruct[PieceType::ROOK].rankcenter.yintercept.eg },

	{ "pst-rook-center-quadratic-mg", &pstConstruct[PieceType::ROOK].center.quadratic.mg },
    { "pst-rook-center-quadratic-eg",&pstConstruct[PieceType::ROOK].center.quadratic.eg },
    { "pst-rook-center-slope-mg", &pstConstruct[PieceType::ROOK].center.slope.mg },
    { "pst-rook-center-slope-eg", &pstConstruct[PieceType::ROOK].center.slope.eg },
    { "pst-rook-center-yintercept-mg", &pstConstruct[PieceType::ROOK].center.yintercept.mg },
    { "pst-rook-center-yintercept-eg",&pstConstruct[PieceType::ROOK].center.yintercept.eg },

	{ "pst-queen-rank-quadratic-mg", &pstConstruct[PieceType::QUEEN].rank.quadratic.mg },
    { "pst-queen-rank-quadratic-eg",&pstConstruct[PieceType::QUEEN].rank.quadratic.eg },
    { "pst-queen-rank-slope-mg", &pstConstruct[PieceType::QUEEN].rank.slope.mg },
    { "pst-queen-rank-slope-eg",&pstConstruct[PieceType::QUEEN].rank.slope.eg },
    { "pst-queen-rank-yintercept-mg", &pstConstruct[PieceType::QUEEN].rank.yintercept.mg },
    { "pst-queen-rank-yintercept-eg",&pstConstruct[PieceType::QUEEN].rank.yintercept.eg },

	{ "pst-queen-file-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].filecenter.quadratic.mg },
    { "pst-queen-file-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].filecenter.quadratic.eg },
    { "pst-queen-file-center-slope-mg", &pstConstruct[PieceType::QUEEN].filecenter.slope.mg },
    { "pst-queen-file-center-slope-eg",&pstConstruct[PieceType::QUEEN].filecenter.slope.eg },
    { "pst-queen-file-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].filecenter.yintercept.mg },
    { "pst-queen-file-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].filecenter.yintercept.eg },

	{ "pst-queen-rank-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].rankcenter.quadratic.mg },
    { "pst-queen-rank-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].rankcenter.quadratic.eg },
    { "pst-queen-rank-center-slope-mg", &pstConstruct[PieceType::QUEEN].rankcenter.slope.mg },
    { "pst-queen-rank-center-slope-eg", &pstConstruct[PieceType::QUEEN].rankcenter.slope.eg },
    { "pst-queen-rank-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].rankcenter.yintercept.mg },
    { "pst-queen-rank-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].rankcenter.yintercept.eg },

	{ "pst-queen-center-quadratic-mg", &pstConstruct[PieceType::QUEEN].center.quadratic.mg },
    { "pst-queen-center-quadratic-eg",&pstConstruct[PieceType::QUEEN].center.quadratic.eg },
    { "pst-queen-center-slope-mg", &pstConstruct[PieceType::QUEEN].center.slope.mg },
    { "pst-queen-center-slope-eg", &pstConstruct[PieceType::QUEEN].center.slope.eg },
    { "pst-queen-center-yintercept-mg", &pstConstruct[PieceType::QUEEN].center.yintercept.mg },
    { "pst-queen-center-yintercept-eg",&pstConstruct[PieceType::QUEEN].center.yintercept.eg },

	{ "pst-king-rank-quadratic-mg", &pstConstruct[PieceType::KING].rank.quadratic.mg },
    { "pst-king-rank-quadratic-eg",&pstConstruct[PieceType::KING].rank.quadratic.eg },
    { "pst-king-rank-slope-mg", &pstConstruct[PieceType::KING].rank.slope.mg },
    { "pst-king-rank-slope-eg",&pstConstruct[PieceType::KING].rank.slope.eg },
    { "pst-king-rank-yintercept-mg", &pstConstruct[PieceType::KING].rank.yintercept.mg },
    { "pst-king-rank-yintercept-eg",&pstConstruct[PieceType::KING].rank.yintercept.eg },

	{ "pst-king-file-center-quadratic-mg", &pstConstruct[PieceType::KING].filecenter.quadratic.mg },
    { "pst-king-file-center-quadratic-eg",&pstConstruct[PieceType::KING].filecenter.quadratic.eg },
    { "pst-king-file-center-slope-mg", &pstConstruct[PieceType::KING].filecenter.slope.mg },
    { "pst-king-file-center-slope-eg",&pstConstruct[PieceType::KING].filecenter.slope.eg },
    { "pst-king-file-center-yintercept-mg", &pstConstruct[PieceType::KING].filecenter.yintercept.mg },
    { "pst-king-file-center-yintercept-eg",&pstConstruct[PieceType::KING].filecenter.yintercept.eg },

	{ "pst-king-rank-center-quadratic-mg", &pstConstruct[PieceType::KING].rankcenter.quadratic.mg },
    { "pst-king-rank-center-quadratic-eg",&pstConstruct[PieceType::KING].rankcenter.quadratic.eg },
    { "pst-king-rank-center-slope-mg", &pstConstruct[PieceType::KING].rankcenter.slope.mg },
    { "pst-king-rank-center-slope-eg", &pstConstruct[PieceType::KING].rankcenter.slope.eg },
    { "pst-king-rank-center-yintercept-mg", &pstConstruct[PieceType::KING].rankcenter.yintercept.mg },
    { "pst-king-rank-center-yintercept-eg",&pstConstruct[PieceType::KING].rankcenter.yintercept.eg },

	{ "pst-king-center-quadratic-mg", &pstConstruct[PieceType::KING].center.quadratic.mg },
    { "pst-king-center-quadratic-eg",&pstConstruct[PieceType::KING].center.quadratic.eg },
    { "pst-king-center-slope-mg", &pstConstruct[PieceType::KING].center.slope.mg },
    { "pst-king-center-slope-eg", &pstConstruct[PieceType::KING].center.slope.eg },
    { "pst-king-center-yintercept-mg", &pstConstruct[PieceType::KING].center.yintercept.mg },
    { "pst-king-center-yintercept-eg",&pstConstruct[PieceType::KING].center.yintercept.eg },

	{ "pawn-blocked-current-default-mg", &PawnCurrentColorBlockedDefault.mg },
	{ "pawn-blocked-current-default-eg", &PawnCurrentColorBlockedDefault.eg },
	{ "pawn-blocked-current-rank-mg", &pawnCurrentColorBlockedPstConstruct.rank.slope.mg },
	{ "pawn-blocked-current-rank-eg", &pawnCurrentColorBlockedPstConstruct.rank.slope.eg },
	{ "pawn-blocked-current-file-center-mg", &pawnCurrentColorBlockedPstConstruct.filecenter.slope.mg },
	{ "pawn-blocked-current-file-center-eg", &pawnCurrentColorBlockedPstConstruct.filecenter.slope.eg },

	{ "pawn-blocked-other-default-mg", &PawnOtherColorBlockedDefault.mg },
	{ "pawn-blocked-other-default-eg", &PawnOtherColorBlockedDefault.eg },
	{ "pawn-blocked-other-rank-mg", &pawnOtherColorBlockedPstConstruct.rank.slope.mg },
	{ "pawn-blocked-other-rank-eg", &pawnOtherColorBlockedPstConstruct.rank.slope.eg },
	{ "pawn-blocked-other-file-center-mg", &pawnOtherColorBlockedPstConstruct.filecenter.slope.mg },
	{ "pawn-blocked-other-file-center-eg", &pawnOtherColorBlockedPstConstruct.filecenter.slope.eg },

	{ "pawn-chain-back-default-mg", &PawnChainBackDefault.mg },
	{ "pawn-chain-back-default-eg", &PawnChainBackDefault.eg },

	{ "pawn-chain-back-rank-quadratic-mg", &pawnChainBackPstConstruct.rank.quadratic.mg },
	{ "pawn-chain-back-rank-quadratic-eg", &pawnChainBackPstConstruct.rank.quadratic.eg },
    { "pawn-chain-back-rank-slope-mg", &pawnChainBackPstConstruct.rank.slope.mg },
    { "pawn-chain-back-rank-slope-eg", &pawnChainBackPstConstruct.rank.slope.eg },

    { "pawn-chain-back-file-center-quadratic-mg", &pawnChainBackPstConstruct.filecenter.quadratic.mg },
	{ "pawn-chain-back-file-center-quadratic-eg", &pawnChainBackPstConstruct.filecenter.quadratic.eg },
    { "pawn-chain-back-file-center-slope-mg", &pawnChainBackPstConstruct.filecenter.slope.mg },
    { "pawn-chain-back-file-center-slope-eg", &pawnChainBackPstConstruct.filecenter.slope.eg },

	{ "pawn-chain-front-default-mg", &PawnChainFrontDefault.mg },
	{ "pawn-chain-front-default-eg", &PawnChainFrontDefault.eg },

    { "pawn-chain-front-rank-quadratic-mg", &pawnChainFrontPstConstruct.rank.quadratic.mg },
	{ "pawn-chain-front-rank-quadratic-eg", &pawnChainFrontPstConstruct.rank.quadratic.eg },
    { "pawn-chain-front-rank-slope-mg", &pawnChainFrontPstConstruct.rank.slope.mg },
    { "pawn-chain-front-rank-slope-eg", &pawnChainFrontPstConstruct.rank.slope.eg },

    { "pawn-chain-front-file-center-quadratic-mg", &pawnChainFrontPstConstruct.filecenter.quadratic.mg },
	{ "pawn-chain-front-file-center-quadratic-eg", &pawnChainFrontPstConstruct.filecenter.quadratic.eg },
    { "pawn-chain-front-file-center-slope-mg", &pawnChainFrontPstConstruct.filecenter.slope.mg },
    { "pawn-chain-front-file-center-slope-eg", &pawnChainFrontPstConstruct.filecenter.slope.eg },

	{ "pawn-doubled-default-mg", &PawnDoubledDefault.mg },
	{ "pawn-doubled-default-eg", &PawnDoubledDefault.eg },
    
    { "pawn-doubled-rank-quadratic-mg", &pawnDoubledPstConstruct.rank.quadratic.mg },
    { "pawn-doubled-rank-quadratic-eg", &pawnDoubledPstConstruct.rank.quadratic.eg },
    { "pawn-doubled-rank-slope-mg", &pawnDoubledPstConstruct.rank.slope.mg },
    { "pawn-doubled-rank-slope-eg", &pawnDoubledPstConstruct.rank.slope.eg },

    { "pawn-doubled-file-center-quadratic-mg", &pawnDoubledPstConstruct.filecenter.quadratic.mg },
	{ "pawn-doubled-file-center-quadratic-eg", &pawnDoubledPstConstruct.filecenter.quadratic.eg },
    { "pawn-doubled-file-center-mg", &pawnDoubledPstConstruct.filecenter.slope.mg },
    { "pawn-doubled-file-center-eg", &pawnDoubledPstConstruct.filecenter.slope.eg },

	{ "pawn-passed-default-mg", &PawnPassedDefault.mg },
	{ "pawn-passed-default-eg", &PawnPassedDefault.eg },

	{ "pawn-passed-rank-quadratic-mg", &pawnPassedPstConstruct.rank.quadratic.mg },
	{ "pawn-passed-rank-quadratic-eg",&pawnPassedPstConstruct.rank.quadratic.eg },
	{ "pawn-passed-rank-slope-mg", &pawnPassedPstConstruct.rank.slope.mg },
	{ "pawn-passed-rank-slope-eg",&pawnPassedPstConstruct.rank.slope.eg },

	{ "pawn-passed-file-center-quadratic-mg", &pawnPassedPstConstruct.filecenter.quadratic.mg },
	{ "pawn-passed-file-center-quadratic-eg",&pawnPassedPstConstruct.filecenter.quadratic.eg },
	{ "pawn-passed-file-center-slope-mg", &pawnPassedPstConstruct.filecenter.slope.mg },
	{ "pawn-passed-file-center-slope-eg",&pawnPassedPstConstruct.filecenter.slope.eg },

	{ "pawn-phalanx-default-mg", &PawnPhalanxDefault.mg },
	{ "pawn-phalanx-default-eg", &PawnPhalanxDefault.eg },

	{ "pawn-phalanx-rank-quadratic-mg", &pawnPhalanxPstConstruct.rank.quadratic.mg },
	{ "pawn-phalanx-rank-quadratic-eg", &pawnPhalanxPstConstruct.rank.quadratic.eg },
    { "pawn-phalanx-rank-slope-mg", &pawnPhalanxPstConstruct.rank.slope.mg },
    { "pawn-phalanx-rank-slope-eg", &pawnPhalanxPstConstruct.rank.slope.eg },

    { "pawn-phalanx-file-center-quadratic-mg", &pawnPhalanxPstConstruct.filecenter.quadratic.mg },
    { "pawn-phalanx-file-center-quadratic-eg", &pawnPhalanxPstConstruct.filecenter.quadratic.eg },
    { "pawn-phalanx-file-center-slope-mg", &pawnPhalanxPstConstruct.filecenter.slope.mg },
    { "pawn-phalanx-file-center-slope-eg", &pawnPhalanxPstConstruct.filecenter.slope.eg },

	{ "pawn-unstoppable-default-mg", &PawnUnstoppableDefault.mg },
	{ "pawn-unstoppable-default-eg", &PawnUnstoppableDefault.eg },
	{ "pawn-unstoppable-rank-mg", &pawnUnstoppablePstConstruct.rank.slope.mg },
	{ "pawn-unstoppable-rank-eg", &pawnUnstoppablePstConstruct.rank.slope.eg },
	{ "pawn-unstoppable-file-center-mg", &pawnUnstoppablePstConstruct.filecenter.slope.mg },
	{ "pawn-unstoppable-file-center-eg", &pawnUnstoppablePstConstruct.filecenter.slope.eg },

	{ "mobility-knight-quadratic-mg", &mobilityConstructor[PieceType::KNIGHT].quadratic.mg },
	{ "mobility-knight-quadratic-eg", &mobilityConstructor[PieceType::KNIGHT].quadratic.eg },
	{ "mobility-knight-slope-mg", &mobilityConstructor[PieceType::KNIGHT].slope.mg },
	{ "mobility-knight-slope-eg", &mobilityConstructor[PieceType::KNIGHT].slope.eg },
	{ "mobility-knight-yintercept-mg", &mobilityConstructor[PieceType::KNIGHT].yintercept.mg },
	{ "mobility-knight-yintercept-eg", &mobilityConstructor[PieceType::KNIGHT].yintercept.eg },

	{ "mobility-bishop-quadratic-mg", &mobilityConstructor[PieceType::BISHOP].quadratic.mg },
	{ "mobility-bishop-quadratic-eg", &mobilityConstructor[PieceType::BISHOP].quadratic.eg },
	{ "mobility-bishop-slope-mg", &mobilityConstructor[PieceType::BISHOP].slope.mg },
	{ "mobility-bishop-slope-eg", &mobilityConstructor[PieceType::BISHOP].slope.eg },
	{ "mobility-bishop-yintercept-mg", &mobilityConstructor[PieceType::BISHOP].yintercept.mg },
	{ "mobility-bishop-yintercept-eg", &mobilityConstructor[PieceType::BISHOP].yintercept.eg },

	{ "mobility-rook-quadratic-mg", &mobilityConstructor[PieceType::ROOK].quadratic.mg },
	{ "mobility-rook-quadratic-eg", &mobilityConstructor[PieceType::ROOK].quadratic.eg },
	{ "mobility-rook-slope-mg", &mobilityConstructor[PieceType::ROOK].slope.mg },
	{ "mobility-rook-slope-eg", &mobilityConstructor[PieceType::ROOK].slope.eg },
	{ "mobility-rook-yintercept-mg", &mobilityConstructor[PieceType::ROOK].yintercept.mg },
	{ "mobility-rook-yintercept-eg", &mobilityConstructor[PieceType::ROOK].yintercept.eg },

	{ "mobility-queen-quadratic-mg", &mobilityConstructor[PieceType::QUEEN].quadratic.mg },
	{ "mobility-queen-quadratic-eg", &mobilityConstructor[PieceType::QUEEN].quadratic.eg },
	{ "mobility-queen-slope-mg", &mobilityConstructor[PieceType::QUEEN].slope.mg },
	{ "mobility-queen-slope-eg", &mobilityConstructor[PieceType::QUEEN].slope.eg },
	{ "mobility-queen-yintercept-mg", &mobilityConstructor[PieceType::QUEEN].yintercept.mg },
	{ "mobility-queen-yintercept-eg", &mobilityConstructor[PieceType::QUEEN].yintercept.eg },

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

	//{ "search-aspiration-initial-value-mg", &AspirationWindowInitialValue.mg },
	//{ "search-aspiration-initial-value-eg", &AspirationWindowInitialValue.eg },
	//{ "search-aspiration-window-delta-mg", &AspirationWindowDelta.mg },
	//{ "search-aspiration-window-delta-eg", &AspirationWindowDelta.eg },

    { "search-futility-margin-quadratic-mg", &FutilityPruningMargin.quadratic.mg },
    { "search-futility-margin-quadratic-eg", &FutilityPruningMargin.quadratic.eg },
    { "search-futility-margin-slope-mg", &FutilityPruningMargin.slope.mg },
    { "search-futility-margin-slope-eg", &FutilityPruningMargin.slope.eg },
    { "search-futility-margin-yintercept-mg", &FutilityPruningMargin.yintercept.mg },
    { "search-futility-margin-yintercept-eg", &FutilityPruningMargin.yintercept.eg },

	{ "search-history-delta-depthleft-quadratic-mg", &HistoryDeltaByDepthLeft.quadratic.mg },
	{ "search-history-delta-depthleft-slope-mg", &HistoryDeltaByDepthLeft.slope.mg },
	{ "search-history-delta-depthleft-yintercept-mg", &HistoryDeltaByDepthLeft.yintercept.mg },

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

	{ "search-probcut-margin-quadratic-mg", &ProbCutPruningMargin.quadratic.mg },
	{ "search-probcut-margin-slope-mg", &ProbCutPruningMargin.slope.mg },
	{ "search-probcut-margin-yintercept-mg", &ProbCutPruningMargin.yintercept.mg },

	{ "search-quiescence-early-exit-margin-quadratic-mg", &QuiescenceEarlyExitMargin.quadratic.mg },
	{ "search-quiescence-early-exit-margin-slope-mg", &QuiescenceEarlyExitMargin.slope.mg },
	{ "search-quiescence-early-exit-margin-yintercept-mg", &QuiescenceEarlyExitMargin.yintercept.mg },

	{ "search-quiescence-early-see-quadratic-mg", &QuiescenceStaticExchangeEvaluationMargin.quadratic.mg },
	{ "search-quiescence-early-see-slope-mg", &QuiescenceStaticExchangeEvaluationMargin.slope.mg },
	{ "search-quiescence-early-see-yintercept-mg", &QuiescenceStaticExchangeEvaluationMargin.yintercept.mg },

	{ "search-razoring-margin-quadratic-mg", &RazoringMargin.quadratic.mg },
	{ "search-razoring-margin-quadratic-eg", &RazoringMargin.quadratic.eg },
	{ "search-razoring-margin-slope-mg", &RazoringMargin.slope.mg },
	{ "search-razoring-margin-slope-eg", &RazoringMargin.slope.eg },
	{ "search-razoring-margin-yintercept-mg", &RazoringMargin.yintercept.mg },
	{ "search-razoring-margin-yintercept-eg", &RazoringMargin.yintercept.eg },

    { "search-nullmove-verification-reductions-quadratic-mg", &NullMoveVerificationReductions.quadratic.mg },
    { "search-nullmove-verification-reductions-slope-mg", &NullMoveVerificationReductions.slope.mg },
    { "search-nullmove-verification-reductions-yintercept-mg", &NullMoveVerificationReductions.yintercept.mg },
    
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

	{ "tropism-knight-quadratic-mg", &tropismConstructor[PieceType::KNIGHT].quadratic.mg },
	{ "tropism-knight-quadratic-eg", &tropismConstructor[PieceType::KNIGHT].quadratic.eg },
	{ "tropism-knight-slope-mg", &tropismConstructor[PieceType::KNIGHT].slope.mg },
	{ "tropism-knight-slope-eg", &tropismConstructor[PieceType::KNIGHT].slope.eg },
	{ "tropism-knight-yintercept-mg", &tropismConstructor[PieceType::KNIGHT].yintercept.mg },
	{ "tropism-knight-yintercept-eg", &tropismConstructor[PieceType::KNIGHT].yintercept.eg },

	{ "tropism-bishop-quadratic-mg", &tropismConstructor[PieceType::BISHOP].quadratic.mg },
	{ "tropism-bishop-quadratic-eg", &tropismConstructor[PieceType::BISHOP].quadratic.eg },
	{ "tropism-bishop-slope-mg", &tropismConstructor[PieceType::BISHOP].slope.mg },
	{ "tropism-bishop-slope-eg", &tropismConstructor[PieceType::BISHOP].slope.eg },
	{ "tropism-bishop-yintercept-mg", &tropismConstructor[PieceType::BISHOP].yintercept.mg },
	{ "tropism-bishop-yintercept-eg", &tropismConstructor[PieceType::BISHOP].yintercept.eg },

	{ "tropism-rook-quadratic-mg", &tropismConstructor[PieceType::ROOK].quadratic.mg },
	{ "tropism-rook-quadratic-eg", &tropismConstructor[PieceType::ROOK].quadratic.eg },
	{ "tropism-rook-slope-mg", &tropismConstructor[PieceType::ROOK].slope.mg },
	{ "tropism-rook-slope-eg", &tropismConstructor[PieceType::ROOK].slope.eg },
	{ "tropism-rook-yintercept-mg", &tropismConstructor[PieceType::ROOK].yintercept.mg },
	{ "tropism-rook-yintercept-eg", &tropismConstructor[PieceType::ROOK].yintercept.eg },

	{ "tropism-queen-quadratic-mg", &tropismConstructor[PieceType::QUEEN].quadratic.mg },
	{ "tropism-queen-quadratic-eg", &tropismConstructor[PieceType::QUEEN].quadratic.eg },
	{ "tropism-queen-slope-mg", &tropismConstructor[PieceType::QUEEN].slope.mg },
	{ "tropism-queen-slope-eg", &tropismConstructor[PieceType::QUEEN].slope.eg },
	{ "tropism-queen-yintercept-mg", &tropismConstructor[PieceType::QUEEN].yintercept.mg },
	{ "tropism-queen-yintercept-eg", &tropismConstructor[PieceType::QUEEN].yintercept.eg },

	{ "doubled-rooks-mg", &DoubledRooks.mg },
	{ "doubled-rooks-eg", &DoubledRooks.eg },

	{ "empty-file-queen-mg", &EmptyFileQueen.mg },
	{ "empty-file-queen-eg", &EmptyFileQueen.eg },

	{ "empty-file-rook-mg", &EmptyFileRook.mg },
	{ "empty-file-rook-eg", &EmptyFileRook.eg },

	{ "queen-behind-passed-pawn-default-mg", &queenBehindPassedPawnDefault.mg },
	{ "queen-behind-passed-pawn-default-eg", &queenBehindPassedPawnDefault.eg },
	{ "queen-behind-passed-pawn-rank-mg", &queenBehindPassedPawnPstConstruct.rank.slope.mg },
	{ "queen-behind-passed-pawn-rank-eg", &queenBehindPassedPawnPstConstruct.rank.slope.eg },
	{ "queen-behind-passed-pawn-file-center-mg", &queenBehindPassedPawnPstConstruct.filecenter.slope.mg },
	{ "queen-behind-passed-pawn-file-center-eg", &queenBehindPassedPawnPstConstruct.filecenter.slope.eg },

	{ "rook-behind-passed-pawn-default-mg", &rookBehindPassedPawnDefault.mg },
	{ "rook-behind-passed-pawn-default-eg", &rookBehindPassedPawnDefault.eg },
	{ "rook-behind-passed-pawn-rank-mg", &rookBehindPassedPawnPstConstruct.rank.slope.mg },
	{ "rook-behind-passed-pawn-rank-eg", &rookBehindPassedPawnPstConstruct.rank.slope.eg },
	{ "rook-behind-passed-pawn-file-center-mg", &rookBehindPassedPawnPstConstruct.filecenter.slope.mg },
	{ "rook-behind-passed-pawn-file-center-eg", &rookBehindPassedPawnPstConstruct.filecenter.slope.eg },
};

void InitializeParameters()
{
	const ScoreConstructor scoreConstructor;

	for (PieceType pieceType = PieceType::PAWN; pieceType < PieceType::PIECETYPE_COUNT; pieceType++) {
		scoreConstructor.construct(pstConstruct[pieceType], PstParameters[pieceType]);
		scoreConstructor.addPst(PstParameters[pieceType], PersonalityPstParameters[pieceType]);

		scoreConstructor.construct(mobilityConstructor[pieceType], &(MobilityParameters[pieceType][0]), 32);

		scoreConstructor.construct(tropismConstructor[pieceType], &(TropismParameters[pieceType][0]), 16);
	}

	scoreConstructor.construct(queenBehindPassedPawnPstConstruct, QueenBehindPassedPawnPst, queenBehindPassedPawnDefault);
	scoreConstructor.construct(rookBehindPassedPawnPstConstruct, RookBehindPassedPawnPst, rookBehindPassedPawnDefault);

	scoreConstructor.construct(pawnCurrentColorBlockedPstConstruct, PawnBlockedPstParameters[Color::CURRENT_COLOR], PawnCurrentColorBlockedDefault);
	scoreConstructor.construct(pawnOtherColorBlockedPstConstruct, PawnBlockedPstParameters[Color::OTHER_COLOR], PawnOtherColorBlockedDefault);
	scoreConstructor.construct(pawnChainBackPstConstruct, PawnChainBackPstParameters, PawnChainBackDefault);
	scoreConstructor.construct(pawnChainFrontPstConstruct, PawnChainFrontPstParameters, PawnChainFrontDefault);
	scoreConstructor.construct(pawnDoubledPstConstruct, PawnDoubledPstParameters, PawnDoubledDefault);
	scoreConstructor.construct(pawnPassedPstConstruct, PawnPassedPstParameters, PawnPassedDefault);
	scoreConstructor.construct(pawnPhalanxPstConstruct, PawnPhalanxPstParameters, PawnPhalanxDefault);
	scoreConstructor.construct(pawnUnstoppablePstConstruct, PawnUnstoppablePstParameters, PawnUnstoppableDefault);

	for (Square src = Square::FIRST_SQUARE; src < Square::SQUARE_COUNT; src++) {
		const File file = GetFile(src);
		const Rank rank = GetRank(src);

		Distance[file][rank] = std::uint32_t(std::sqrt(file * file + rank * rank));
	}
}
