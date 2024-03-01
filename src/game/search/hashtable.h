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

#include <cstdint>
#include <vector>

#include "../types/depth.h"
#include "../types/hash.h"
#include "../types/nodecount.h"
#include "../types/score.h"
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

struct HashtableEntry {
    union {
        struct {
            Hash hashValue;
            Score score;
            HashtableDepth depthLeft;
            HashtableAge age;
            HashtableEntryType hashtableEntryType;
            std::uint8_t custom;
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

#if defined(USE_M128I)
        __m128i vector;
#endif
    };

    constexpr std::uint8_t getCustom() const
    {
        return this->search.custom;
    }

    constexpr Depth getDepthLeft() const
    {
        return static_cast<Depth>(this->search.depthLeft);
    }

    constexpr Score getScore(Depth currentDepth) const
    {
        return ScoreFromHash(this->search.score, currentDepth);
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

    void insert(Hash hashValue, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType, std::uint8_t custom = 0);

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

    HashtableEntryType search(Hash hashValue, Score& score, Depth currentDepth, Depth& depthLeft, std::uint8_t& custom) const;

    bool search(HashtableEntry& hashEntry, Hash hashValue) const;
};
