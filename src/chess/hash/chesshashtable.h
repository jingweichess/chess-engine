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

//#pragma once
//
//#include <cstdint>
//
//#include "../types/move.h"
//
//#include "../types/score.h"
//#include "../types/square.h"
//
//#include "../../game/types/hash.h"
//#include "../../game/types/nodecount.h"
//
//using HashtableAge = std::uint8_t;
//
//enum class HashtableEntryType : std::uint8_t {
//    NONE,
//    EXACT_VALUE,
//    LOWER_BOUND,
//    UPPER_BOUND
//};
//
//constexpr Score ScoreFromHash(Score score, Depth currentDepth)
//{
//    if (score >= WinInMaxDepth()) {
//        return score - currentDepth;
//    }
//    else if (score <= LostInMaxDepth()) {
//        return score + currentDepth;
//    }
//
//    return score;
//}
//
//constexpr Score ScoreToHash(Score score, Depth currentDepth)
//{
//    if (score >= WinInMaxDepth()) {
//        return score + currentDepth;
//    }
//    else if (score <= LostInMaxDepth()) {
//        return score - currentDepth;
//    }
//
//    return score;
//}
//
//struct HashtableEntryInfo
//{
//    union {
//        std::uint64_t info;
//
//        struct {
//            Score score;
//
//            HashtableEntryType entryType;
//            std::uint8_t src;
//            std::uint8_t dst;
//            std::int8_t depthLeft;
//        } search;
//
//        struct {
//            Score mg;
//            Score eg;
//        } pawn;
//    };
//
//    constexpr HashtableEntryInfo() {}
//
//    constexpr Depth getDepthLeft()
//    {
//        return static_cast<Depth>(this->search.depthLeft);
//    }
//
//    constexpr Square getDst()
//    {
//        return static_cast<Square>(this->search.dst);
//    }
//
//    constexpr Score getEg()
//    {
//        return this->pawn.eg;
//    }
//
//    constexpr HashtableEntryType getHashtableEntryType()
//    {
//        return this->search.entryType;
//    }
//
//    constexpr Score getMg()
//    {
//        return this->pawn.mg;
//    }
//
//    constexpr Score getScore(Depth currentDepth)
//    {
//        return ScoreFromHash(this->search.score, currentDepth);
//    }
//
//    constexpr Square getSrc()
//    {
//        return static_cast<Square>(this->search.src);
//    }
//};
//
//struct HashtableEntry
//{
//    union {
//        struct {
//            Hash hash;
//            HashtableEntryInfo info;
//        } hash;
//
//        struct {
//            Hash hash;
//            Score mg;
//            Score eg;
//        } pawn;
//
//        struct {
//            Hash hash;
//            Score mg;
//            Score eg;
//        } eval;
//
//        struct {
//            Hash hash;
//            NodeCount nodeCount;
//        } perft;
//
//#if defined(USE_M128I)
//        __m128i vector;
//#endif
//    };
//
//    HashtableEntry() {}
//};
//
//static_assert(sizeof(HashtableEntry) == 16);
//
//struct HashtableBucket
//{
//    HashtableEntry entryList[4];
//
//    HashtableBucket() {}
//};
//
//static_assert(sizeof(HashtableBucket) == 64);
//
//class ChessHashtable
//{
//protected:
//    HashtableBucket* hashBucketList = nullptr;
//
//    std::uint64_t hashBucketCount = 0;
//    HashtableAge currentAge = 0;
//public:
//    constexpr ChessHashtable() {};
//    ~ChessHashtable();
//
//    void incrementAge();
//
//    void initialize(std::uint64_t megaBytes);
//
//    void insert(Hash hashValue, Score mg, Score eg);
//    void insert(Hash hashValue, const ChessMove& move, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType);
//
//    void insert(Hash hashValue, HashtableEntryInfo info, Depth depthLeft = Depth::ZERO);
//
//    void prefetch(Hash hashValue) const;
//
//    void reset();
//
//    bool search(HashtableEntryInfo& hashtableEntryInfo, Hash hashValue) const;
//};
