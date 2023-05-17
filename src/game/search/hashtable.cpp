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

//#include <cassert>
//
//#include "hashtable.h"
//
//constexpr bool testHashtableSaves = true;
//
//Hashtable::Hashtable()
//{
//    this->currentAge = 0;
//    this->hashEntryCount = 0;
//    this->hashEntryList = nullptr;
//}
//
//Hashtable::~Hashtable()
//{
//    if (this->hashEntryList != nullptr) {
//        delete[] this->hashEntryList;
//    }
//}
//
//void Hashtable::incrementAge()
//{
//    this->currentAge++;
//}
//
//void Hashtable::initialize(std::uint32_t entryCount)
//{
//    if (this->hashEntryList != nullptr) {
//        delete[] this->hashEntryList;
//        this->hashEntryList = nullptr;
//    }
//
//    HashtableEntry* newHashEntryList = new HashtableEntry[entryCount];
//
//    this->hashEntryList = newHashEntryList;
//    this->hashEntryCount = entryCount;
//
//    this->reset();
//}
//
//void Hashtable::insert(Hash hashValue, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType, std::uint8_t custom)
//{
//    std::uint32_t position = hashValue & (this->hashEntryCount - 1);
//    HashtableEntry* oldHashtableEntry = this->hashEntryList + position;
//
//    //if (oldHashtableEntry->hashValue == hashValue) {
//    //    HashtableAge oldHashtableAge = oldHashtableEntry->age;
//    //    HashtableDepth oldHashtableDepth = oldHashtableEntry->depthLeft;
//
//    //    if (this->currentAge == oldHashtableAge
//    //        && depthLeft <= oldHashtableDepth) {
//    //        return;
//    //    }
//    //}
//
//    HashtableEntry hashtableEntry;
//
//    hashtableEntry.hash.hashValue = hashValue;
//    hashtableEntry.hash.score = this->scoreToHash(score, currentDepth);
//    hashtableEntry.hash.depthLeft = depthLeft;
//    hashtableEntry.hash.age = this->currentAge;
//    hashtableEntry.hash.hashtableEntryType = hashtableEntryType;
//    hashtableEntry.hash.custom = custom;
//
//#if defined(USE_M128I)
//    (*oldHashtableEntry).vector = hashtableEntry.vector;
//#else
//    *oldHashtableEntry = hashtableEntry;
//#endif
//
//    if (testHashtableSaves) {
//        Score testScore;
//        Depth testDepthLeft;
//        std::uint8_t testCustom;
//
//        HashtableEntryType testHashtableEntryType = this->search(hashValue, testScore, currentDepth, testDepthLeft, testCustom);
//
//        assert(testHashtableEntryType == hashtableEntryType);
//        assert(score == testScore);
//        assert(depthLeft == testDepthLeft);
//        assert(testCustom == custom);
//    }
//}
//
//void Hashtable::reset()
//{
//    HashtableEntry hashtableEntry;
//    hashtableEntry.hash.hashValue = EmptyHash;
//
//    for (std::uint32_t i = 0; i < this->hashEntryCount; i++) {
//        this->hashEntryList[i] = hashtableEntry;
//    }
//}
//
//Score Hashtable::scoreFromHash(Score score, Depth currentDepth) const
//{
//    if (score >= WIN_SCORE - Depth::MAX) {
//        return score - currentDepth;
//    }
//    else if (score <= -WIN_SCORE + Depth::MAX) {
//        return score + currentDepth;
//    }
//
//    return score;
//}
//
//Score Hashtable::scoreToHash(Score score, Depth currentDepth) const
//{
//    if (score >= WIN_SCORE - Depth::MAX) {
//        return score + currentDepth;
//    }
//    else if (score <= -WIN_SCORE + Depth::MAX) {
//        return score - currentDepth;
//    }
//
//    return score;
//}
//
//HashtableEntryType Hashtable::search(Hash hashValue, Score& score, Depth currentDepth, Depth& depthLeft, std::uint8_t& custom) const
//{
//    std::uint32_t position = hashValue & (this->hashEntryCount - 1);
//    HashtableEntry* hashtableEntry = this->hashEntryList + position;
//
//    if (hashValue != hashtableEntry->hash.hashValue) {
//        return HashtableEntryType::NONE;
//    }
//
//    custom = hashtableEntry->hash.custom;
//    depthLeft = (Depth)hashtableEntry->hash.depthLeft;
//    score = this->scoreFromHash(hashtableEntry->hash.score, currentDepth);
//
//    return hashtableEntry->hash.hashtableEntryType;
//}
//
//bool Hashtable::search(HashtableEntry& hashEntry, Hash hashValue) const
//{
//    std::uint32_t position = hashValue & (this->hashEntryCount - 1);
//    HashtableEntry* hashtableEntry = this->hashEntryList + position;
//
//    hashEntry = *hashtableEntry;
//
//    return hashValue == hashtableEntry->newHash.hashValue;
//}
