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

//#pragma once
//
//#include <cstdint>
//#include <vector>
//
//#include "../types/depth.h"
//#include "../types/hash.h"
//#include "../types/nodecount.h"
//#include "../types/score.h"
//#include "../../chess/types/square.h"
//
//using HashtableAge = std::uint8_t;
//using HashtableDepth = std::int8_t;
//using HashtableInfo = std::uint64_t;
//
//enum class HashtableEntryType : std::uint8_t {
//    NONE,
//    EXACT_VALUE,
//    LOWER_BOUND,
//    UPPER_BOUND
//};
//
//struct HashtableEntry {
//    union {
//        struct {
//            Hash hashValue;
//            Score score;
//            HashtableDepth depthLeft;
//            HashtableAge age;
//            HashtableEntryType hashtableEntryType;
//            std::uint8_t custom;
//        } hash;
//
//        struct {
//            Hash hashValue;
//            HashtableInfo info;
//        } newHash;
//
//        struct {
//            Hash hashValue;
//            NodeCount nodeCount;
//        } perft;
//
//#if defined(USE_M128I)
//        __m128i vector;
//#endif
//    };
//};
//
//static_assert(sizeof(HashtableEntry) == 16);
//
//class Hashtable
//{
//protected:
//    HashtableEntry* hashEntryList = nullptr;
//
//    std::uint32_t hashEntryCount;
//    HashtableAge currentAge;
//public:
//    Hashtable();
//    ~Hashtable();
//
//    void incrementAge();
//
//    void initialize(std::uint32_t entryCount);
//
//    void insert(Hash hashValue, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType, std::uint8_t custom);
//
//    void reset();
//
//    Score scoreFromHash(Score score, Depth currentDepth) const;
//    Score scoreToHash(Score score, Depth currentDepth) const;
//
//    HashtableEntryType search(Hash hashValue, Score& score, Depth currentDepth, Depth& depthLeft, std::uint8_t& custom) const;
//
//    bool search(HashtableEntry& hashEntry, Hash hashValue) const;
//};
