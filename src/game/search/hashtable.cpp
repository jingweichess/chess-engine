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

#include <cassert>

#include "hashtable.h"

constexpr bool testHashtableSaves = false;

Hashtable::Hashtable()
{
    this->currentAge = 0;
    this->hashEntryCount = 0;
    this->hashEntryList = nullptr;
}

Hashtable::~Hashtable()
{
    if (this->hashEntryList != nullptr) {
        delete[] this->hashEntryList;
    }
}

void Hashtable::incrementAge()
{
    this->currentAge++;
}

void Hashtable::initialize(std::uint32_t entryCount)
{
    if (this->hashEntryList != nullptr) {
        delete[] this->hashEntryList;
        this->hashEntryList = nullptr;
    }

    HashtableEntry* newHashEntryList = new HashtableEntry[entryCount];

    this->hashEntryList = newHashEntryList;
    this->hashEntryCount = entryCount;

    this->reset();
}

void Hashtable::insert(Hash hashValue, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType, std::uint8_t custom)
{
    const std::uint32_t position = hashValue & (this->hashEntryCount - 1);
    HashtableEntry* oldHashtableEntry = this->hashEntryList + position;

    //if (oldHashtableEntry->search.hashValue == hashValue) {
    //    const HashtableAge oldHashtableAge = oldHashtableEntry->search.age;
    //    const HashtableDepth oldHashtableDepth = oldHashtableEntry->search.depthLeft;

    //    if (depthLeft <= oldHashtableDepth) {
    //        if (this->currentAge != oldHashtableAge) {
    //            oldHashtableEntry->search.age = this->currentAge;
    //        }

    //        return;
    //    }
    //}

    HashtableEntry hashtableEntry;

    hashtableEntry.search.hashValue = hashValue;
    hashtableEntry.search.score = ScoreToHash(score, currentDepth);
    hashtableEntry.search.depthLeft = depthLeft;
    hashtableEntry.search.age = this->currentAge;
    hashtableEntry.search.hashtableEntryType = hashtableEntryType;
    hashtableEntry.search.custom = custom;

#if defined(USE_M128I)
    (*oldHashtableEntry).vector = hashtableEntry.vector;
#else
    *oldHashtableEntry = hashtableEntry;
#endif

    if (testHashtableSaves) {
        Score testScore;
        Depth testDepthLeft;
        std::uint8_t testCustom;

        const HashtableEntryType testHashtableEntryType = this->search(hashValue, testScore, currentDepth, testDepthLeft, testCustom);

        assert(testHashtableEntryType == hashtableEntryType);
        assert(score == testScore);
        assert(depthLeft == testDepthLeft);
        assert(testCustom == custom);
    }
}

void Hashtable::insert(Hash hashValue, Score mateScore)
{
    const std::uint32_t position = hashValue & (this->hashEntryCount - 1);
    HashtableEntry* oldHashtableEntry = this->hashEntryList + position;

    HashtableEntry hashtableEntry;

    hashtableEntry.mate.hashValue = hashValue;
    hashtableEntry.mate.score = mateScore;

#if defined(USE_M128I)
    (*oldHashtableEntry).vector = hashtableEntry.vector;
#else
    *oldHashtableEntry = hashtableEntry;
#endif
}

void Hashtable::reset()
{
    HashtableEntry hashtableEntry;
    hashtableEntry.search.hashValue = EmptyHash;

    for (std::uint32_t i = 0; i < this->hashEntryCount; i++) {
        this->hashEntryList[i] = hashtableEntry;
    }
}

HashtableEntryType Hashtable::search(Hash hashValue, Score& score, Depth currentDepth, Depth& depthLeft, std::uint8_t& custom) const
{
    std::uint32_t position = hashValue & (this->hashEntryCount - 1);
    HashtableEntry* hashtableEntry = this->hashEntryList + position;

    if (hashValue != hashtableEntry->search.hashValue) {
        return HashtableEntryType::NONE;
    }

    custom = hashtableEntry->search.custom;
    depthLeft = (Depth)hashtableEntry->search.depthLeft;
    score = ScoreFromHash(hashtableEntry->search.score, currentDepth);

    return hashtableEntry->search.hashtableEntryType;
}

bool Hashtable::search(HashtableEntry& hashEntry, Hash hashValue) const
{
    std::uint32_t position = hashValue & (this->hashEntryCount - 1);
    HashtableEntry* hashtableEntry = this->hashEntryList + position;

    hashEntry = *hashtableEntry;

    return hashValue == hashtableEntry->newHash.hashValue;
}
