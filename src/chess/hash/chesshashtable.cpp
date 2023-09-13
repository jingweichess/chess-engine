///*
//    Jing Wei, the rebirth of the chess engine I started in 2010
//    Copyright(C) 2019-2023 Chris Florin
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//*/
//
//#include <cassert>
//
//#include "chesshashtable.h"
//
//ChessHashtable::~ChessHashtable()
//{
//    if (this->hashBucketList != nullptr) {
//        delete[] this->hashBucketList;
//        this->hashBucketList = nullptr;
//    }
//}
//
//void ChessHashtable::incrementAge()
//{
//    this->currentAge++;
//}
//
//void ChessHashtable::initialize(std::uint64_t megaBytes)
//{
//    const std::uint64_t hashEntryCount = megaBytes * 1024 * 1024 / sizeof(HashtableEntry);
//    const std::uint64_t hashBucketCount = hashEntryCount / 4;
//
//    if (this->hashBucketList != nullptr) {
//        delete[] this->hashBucketList;
//        this->hashBucketList = nullptr;
//    }
//
//    HashtableBucket* newHashBucketList = new HashtableBucket[hashBucketCount];
//
//    this->hashBucketList = newHashBucketList;
//    this->hashBucketCount = hashBucketCount;
//
//    this->reset();
//}
//
//void ChessHashtable::insert(Hash hashValue, Score mg, Score eg)
//{
//    HashtableEntryInfo info;
//
//    info.pawn.mg = mg;
//    info.pawn.eg = eg;
//
//    this->insert(hashValue, info);
//}
//
//void ChessHashtable::insert(Hash hashValue, const ChessMove& move, Score score, Depth currentDepth, Depth depthLeft, HashtableEntryType hashtableEntryType)
//{
//    HashtableEntryInfo info;
//
//    info.search.score = ScoreToHash(score, currentDepth);
//
//    assert(ScoreFromHash(info.search.score, currentDepth) == score);
//
//    info.search.entryType = hashtableEntryType;
//    info.search.src = static_cast<std::uint8_t>(move.src);
//    info.search.dst = static_cast<std::uint8_t>(move.dst);
//    info.search.depthLeft = static_cast<std::uint8_t>(depthLeft);
//
//    this->insert(hashValue, info, depthLeft);
//}
//
//void ChessHashtable::insert(Hash hashValue, HashtableEntryInfo info, Depth depthLeft)
//{
//    std::uint64_t position = hashValue & (this->hashBucketCount - 1);
//    HashtableBucket* hashtableBucket = this->hashBucketList + position;
//
//    std::int32_t entry = -1;
//    std::uint64_t storedHash = hashValue & 0xffffffffffffff00;
//
//    HashtableAge age = this->currentAge;
//    std::uint64_t fullHash = storedHash | (static_cast<std::uint64_t>(age));
//
//    std::int8_t smallestDepth = hashtableBucket->entryList[0].hash.info.getDepthLeft();
//    std::uint32_t entryToOverwrite = fullHash & 0x3;
//
//    std::uint32_t priority = 100;
//
//    for (std::int32_t i = 0; i < 4; i++) {
//        HashtableEntry* hashtableEntry = &hashtableBucket->entryList[i];
//
//        if ((hashtableEntry->hash.hash & 0xffffffffffffff00) == storedHash) {
//            if (hashtableEntry->hash.info.getDepthLeft() <= depthLeft) {
//                entryToOverwrite = i;
//
//                break;
//            }
//        }
//        else if ((hashtableEntry->hash.hash & 0x00000000000000ff) != age) {
//            entryToOverwrite = i;
//            priority = 1;
//        }
//        else if (hashtableEntry->hash.info.getDepthLeft() < smallestDepth
//            && priority >= 2) {
//            entryToOverwrite = i;
//            smallestDepth = hashtableEntry->hash.info.getDepthLeft();
//            priority = 2;
//        }
//    }
//
//    assert(entryToOverwrite != 4);
//
//    HashtableEntry newHashtableEntry;
//
//    newHashtableEntry.hash.hash = fullHash;
//    newHashtableEntry.hash.info = info;
//
//    HashtableEntry* hashtableEntry = &hashtableBucket->entryList[entryToOverwrite];
//    *hashtableEntry = newHashtableEntry;
//}
//
//void ChessHashtable::prefetch(Hash hashValue) const
//{
//#ifndef _DEBUG
//    std::uint64_t position = hashValue & (this->hashBucketCount - 1);
//    HashtableBucket* hashtableBucket = this->hashBucketList + position;
//
//#ifdef _MSC_VER
//    _mm_prefetch((char *)hashtableBucket, _MM_HINT_T0);
//#else
//    __builtin_prefetch((void *)hashtableBucket);
//#endif
//#endif
//}
//
//void ChessHashtable::reset()
//{
//    HashtableBucket hashtableBucket;
//
//    hashtableBucket.entryList[0].perft.hash = EmptyHash;
//    hashtableBucket.entryList[0].perft.nodeCount = ZeroNodes;
//    hashtableBucket.entryList[1].perft.hash = EmptyHash;
//    hashtableBucket.entryList[1].perft.nodeCount = ZeroNodes;
//    hashtableBucket.entryList[2].perft.hash = EmptyHash;
//    hashtableBucket.entryList[2].perft.nodeCount = ZeroNodes;
//    hashtableBucket.entryList[3].perft.hash = EmptyHash;
//    hashtableBucket.entryList[3].perft.nodeCount = ZeroNodes;
//
//    for (std::uint32_t i = 0; i < this->hashBucketCount; i++) {
//        this->hashBucketList[i] = hashtableBucket;
//    }
//}
//
//bool ChessHashtable::search(HashtableEntryInfo& hashtableEntryInfo, Hash hashValue) const
//{
//    std::uint64_t position = hashValue & (this->hashBucketCount - 1);
//    HashtableBucket* hashtableBucket = this->hashBucketList + position;
//
//    std::uint64_t storedHash = hashValue & 0xffffffffffffff00;
//
//    for (std::int32_t i = 0; i < 4; i++) {
//        HashtableEntry* hashtableEntry = &hashtableBucket->entryList[i];
//
//        //Overwrite an entry with the same hash and a lower depth
//        if ((hashtableEntry->hash.hash & 0xffffffffffffff00) == storedHash) {
//            hashtableEntryInfo = hashtableEntry->hash.info;
//
//            return true;
//        }
//    }
//
//    return false;
//}
