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

#include <type_traits>

#include <cstdint>

#include "depth.h"

using Score = std::int32_t;

constexpr Score ZERO_SCORE = 0;
constexpr Score DRAW_SCORE = ZERO_SCORE;
constexpr Score NO_SCORE = 32002;
constexpr Score UNIT_SCORE = 256;
constexpr Score WIN_SCORE = 32000;
constexpr Score INFINITE_SCORE = WIN_SCORE + 1;
constexpr Score INVALID_SCORE = -32768;

constexpr Score BASICALLY_WINNING_SCORE = UNIT_SCORE * 50;

constexpr Score LostInDepth(Depth depth)
{
    return -WIN_SCORE + depth;
}

constexpr Score LostInMaxDepth()
{
    return LostInDepth(Depth::MAX);
}

constexpr Score WinInDepth(Depth depth)
{
    return WIN_SCORE - depth;
}

constexpr Score WinInMaxDepth()
{
    return WinInDepth(Depth::MAX);
}

constexpr bool IsLossScore(Score score)
{
    return score < (-WIN_SCORE + Depth::MAX);
}

constexpr bool IsMateScore(Score score)
{
    return (score < (-WIN_SCORE + Depth::MAX)) ||
        (score > (WIN_SCORE - Depth::MAX));
}

constexpr bool IsDrawScore(Score score)
{
    return score == DRAW_SCORE;
}

constexpr Depth DistanceToWin(Score score)
{
    if (score > (WIN_SCORE - Depth::MAX)) {
        return Depth::ONE * (WIN_SCORE - score);
    }
    else if (score < (-WIN_SCORE + Depth::MAX)) {
        return Depth::ONE * (score + WIN_SCORE);
    }
    else {
        return Depth::ZERO;
    }
}

#if defined(USE_M128I)
#include <immintrin.h>

static __m128i zerovector = _mm_setr_epi32(0, 0, 0, 0);
#endif

union Evaluation {
    struct {
        Score mg, eg, pad0, pad1;
    };

#if defined(USE_M128I)
    __m128i vector;
#endif

    constexpr Evaluation() : mg(0), eg(0), pad0(0), pad1(0) {}
    constexpr Evaluation(Score mg, Score eg) : mg(mg), eg(eg), pad0(0), pad1(0) {}
    constexpr ~Evaluation() {}

    constexpr Score operator()(std::int32_t phase) const
    {
        const Score result = ((this->mg * phase) + (this->eg * (32 - phase))) / 32;

        return result;
    }

    constexpr Evaluation& operator += (const Evaluation e2)
    {
        if (std::is_constant_evaluated()) {
            this->mg = this->mg + e2.mg;
            this->eg = this->eg + e2.eg;
        }
        else {
#if defined(USE_M128I)
            this->vector = _mm_add_epi32(this->vector, e2.vector);
#else
            this->mg = this->mg + e2.mg;
            this->eg = this->eg + e2.eg;
#endif
        }
        return *this;
    }

    constexpr friend Evaluation operator + (const Evaluation e1, const Evaluation e2)
    {
        Evaluation result;

        if (std::is_constant_evaluated()) {
            result.mg = e1.mg + e2.mg;
            result.eg = e1.eg + e2.eg;
        }
        else {
#if defined(USE_M128I)
            result.vector = _mm_add_epi32(e1.vector, e2.vector);
#else
            result.mg = e1.mg + e2.mg;
            result.eg = e1.eg + e2.eg;
#endif
        }

        return result;
    }

    constexpr Evaluation& operator -= (const Evaluation e2)
    {

        if (std::is_constant_evaluated()) {
            this->mg = this->mg - e2.mg;
            this->eg = this->eg - e2.eg;
        }
        else {
#if defined(USE_M128I)
            this->vector = _mm_sub_epi32(this->vector, e2.vector);
#else
            this->mg = this->mg - e2.mg;
            this->eg = this->eg - e2.eg;
#endif
        }

        return *this;
    }

    constexpr friend Evaluation operator - (const Evaluation e1, const Evaluation e2)
    {
        Evaluation result;

        if (std::is_constant_evaluated()) {
            result.mg = e1.mg - e2.mg;
            result.eg = e1.eg - e2.eg;
        }
        else {
#if defined(USE_M128I)
            result.vector = _mm_sub_epi32(e1.vector, e2.vector);
#else
            result.mg = e1.mg - e2.mg;
            result.eg = e1.eg - e2.eg;
#endif
        }

        return result;
    }

    constexpr Evaluation operator - ()
    {
        if (std::is_constant_evaluated()) {
            return Evaluation{ -this->mg, -this->eg };
        }
        else {
#if defined(USE_M128I)
            Evaluation result;
            result.vector = _mm_sub_epi32(zerovector, this->vector);

            return result;
#else
            return Evaluation{ -this->mg, -this->eg };
#endif
        }
    }

    constexpr Evaluation operator * (const std::int32_t i)
    {
        Evaluation result;

        if (std::is_constant_evaluated()) {
            result.mg = this->mg * i;
            result.eg = this->eg * i;
        }
        else {
#if defined(USE_M128I)
            __m128i ivector = _mm_setr_epi32(i, i, i, i);

            result.vector = _mm_mullo_epi32(this->vector, ivector);
#else
            result.mg = this->mg * i;
            result.eg = this->eg * i;
#endif
        }

        return result;
    }

    constexpr Evaluation operator / (const std::int32_t i)
    {
        Evaluation result;

        if (std::is_constant_evaluated()) {
            result.mg = this->mg / i;
            result.eg = this->eg / i;
        }
        else {
//#if defined(USE_M128I) and defined(_MSC_VER)
//        __m128i ivector = _mm_setr_epi32(i, i, i, i);

//        result.vector = _mm_div_epi32(this->vector, ivector);
//#else
            result.mg = this->mg / i;
            result.eg = this->eg / i;
//#endif
        }

        return result;
    }

    constexpr bool operator == (Evaluation e2)
    {
        if (std::is_constant_evaluated()) {
            return (this->mg == e2.mg) && (this->eg == e2.eg);
        }
        else {
#if defined(USE_M128I)
            __m128i result = _mm_cmpeq_epi32(this->vector, e2.vector);
            std::uint64_t spread[2];

            _mm_store_si128((__m128i*)spread, result);

            return spread != 0x0;
#else
            return (this->mg == e2.mg) && (this->eg == e2.eg);
#endif
        }
    }

    constexpr bool operator != (Evaluation e2)
    {
        if (std::is_constant_evaluated()) {
            return (this->mg != e2.mg) || (this->eg != e2.eg);
        }
        else {
//#if defined(USE_M128I)
//        __m128i result = _mm_cmpeq_epi32(this->vector, e2.vector);
//        std::uint64_t spread[2];
//
//        _mm_store_si128((__m128i*)spread, result);
//
//        return spread != 0x0;
//#else
            return (this->mg != e2.mg) || (this->eg != e2.eg);
//#endif
        }
//#if defined(USE_M128I)
//        __m128i result = _mm_cmpeq_epi32(this->vector, e2.vector);
//        std::uint64_t spread[2];
//
//        _mm_store_si128((__m128i*)spread, result);
//
//        return spread != 0x0;
//#else
        return (this->mg != e2.mg) || (this->eg != e2.eg);
//#endif
    }
};

constexpr Evaluation operator * (std::int32_t i, const Evaluation e)
{
    Evaluation result;

    if (std::is_constant_evaluated()) {
        result.mg = e.mg * i;
        result.eg = e.eg * i;
    }
    else {
#if defined(USE_M128I)
        __m128i ivector = _mm_setr_epi32(i, i, i, i);

        result.vector = _mm_mullo_epi32(e.vector, ivector);
#else
        result.mg = e.mg * i;
        result.eg = e.eg * i;
#endif
    }

    return result;
}
