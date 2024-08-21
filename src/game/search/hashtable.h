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

#pragma once

#include <cstdint>
#include <vector>

#include "../types/depth.h"
#include "../types/hash.h"
#include "../types/nodecount.h"
#include "../types/score.h"

#include "../../chess/types/move.h"
#include "../../chess/types/piecetype.h"
#include "../../chess/types/square.h"

using HashtableAge = std::uint8_t;
using HashtableDepth = std::int8_t;
using HashtableInfo = std::uint64_t;

enum class HashtableEntryType : std::uint8_t {
    NONE,
    EXACT_VALUE,
    LOWER_BOUND,
    UPPER_BOUND
};

constexpr Score ScoreFromHash(Score score, Depth currentDepth)
{
    if (score >= WinInMaxDepth()) {
        return score - currentDepth;
    }
    else if (score <= LostInMaxDepth()) {
        return score + currentDepth;
    }

    return score;
}

constexpr Score ScoreToHash(Score score, Depth currentDepth)
{
    if (score >= WinInMaxDepth()) {
        return score + currentDepth;
    }
    else if (score <= LostInMaxDepth()) {
        return score - currentDepth;
    }

    return score;
}

using HashScore = std::int16_t;

struct HashtableEntry {
    union {
        struct {
            Hash hashValue;
            HashScore score;
            HashtableDepth depthLeft;
            HashtableAge age;
            HashtableEntryType hashtableEntryType;
            Square src, dst;
            PieceType promotionPiece;
        } search;

        struct {
            Hash hashValue;
            HashtableInfo info;
        } newHash;

        struct {
            Hash hashValue;
            Score mg;
            Score eg;
        } eval;

        struct {
            Hash hashValue;
            Score mg;
            Score eg;
        } pawn;

        struct {
            Hash hashValue;
            NodeCount nodeCount;
        } perft;

        struct {
            Hash hashValue;
            Score score;
        } mate;

#if defined(USE_M128I)
        __m128i vector;
#endif
    };

    constexpr Depth getDepthLeft() const
    {
        return static_cast<Depth>(this->search.depthLeft);
    }

    constexpr Square getDst() const
    {
        return this->search.dst;
    }

    constexpr Score getMateScore() const
    {
        return this->mate.score;
    }

    constexpr PieceType getPromotionPiece() const
    {
        return this->search.promotionPiece;
    }

    constexpr Score getScore(Depth currentDepth) const
    {
        const HashScore hashScore = this->search.score;

        return ScoreFromHash(hashScore, currentDepth);
    }

    constexpr Square getSrc() const
    {
        return this->search.src;
    }

    constexpr HashtableEntryType getType() const
    {
        return this->search.hashtableEntryType;
    }
};

static_assert(sizeof(HashtableEntry) == 16);

class Hashtable
{
protected:
    HashtableEntry* hashEntryList = nullptr;

    std::uint32_t hashEntryCount;
    HashtableAge currentAge;
public:
    Hashtable();
    ~Hashtable();

    void incrementAge();

    void initialize(std::uint32_t entryCount);

    void insert(Hash hashValue, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType, const ChessMove& move);
    void insert(Hash hashValue, Score mateScore);

    void prefetch(Hash hashValue) const
    {
#ifndef _DEBUG
        std::uint32_t position = hashValue & (this->hashEntryCount - 1);
        HashtableEntry* hashtableEntry = this->hashEntryList + position;

#ifdef _MSC_VER
        _mm_prefetch((char *)hashtableEntry, _MM_HINT_T0);
#else
        __builtin_prefetch((void *)hashtableEntry);
#endif
#endif
    }

    void reset();

    HashtableEntryType search(Hash hashValue, Score& score, Depth currentDepth, Depth& depthLeft, ChessMove& move) const;

    bool search(HashtableEntry& hashEntry, Hash hashValue) const;
};
