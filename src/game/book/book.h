#pragma once

#include <map>
#include <string>

#include "../types/book.h"
#include "../types/hash.h"
#include "../types/movelist.h"
#include "../types/nodecount.h"

template <class Board, class BookPosition = DefaultBookPosition>
class Book {
private:
	using MoveListIterator = typename std::vector<typename Board::MoveType>::iterator;
protected:
	using BookPositionMap = std::map<Hash, BookPosition>;
	using BookPositionMapIterator = typename std::map<Hash, BookPosition>::iterator;

	BookPositionMap positionMap;
public:
	using BoardType = Board;
	using MoveType = typename BoardType::MoveType;

	using size_type = typename BookPositionMap::size_type;

	Book()
	{

	}

	~Book()
	{

	}

	void addPosition(Hash hashValue)
	{
		BookPosition& bookPosition = this->positionMap[hashValue];
		bookPosition.hashValue = hashValue;
	}

	NodeCount getBookMoves(BoardType& board, MoveList<MoveType>& moveList)
	{
		MoveListIterator it = moveList.begin();
		while (it != moveList.end()) {
			BoardType nextBoard = board;
			MoveType& move = *it;

			nextBoard.doMove();

			if (!this->hasPosition(nextBoard.hashValue)) {
				it = moveList.erase(it);
			}
			else {
				++it;
			}
		}

		return moveList.size();
	}

	BookPosition& getPosition(Hash hashValue)
	{
		return this->positionMap.find(hashValue)->second;
	}

	bool hasPosition(Hash hashValue)
	{
		return this->positionMap.count(hashValue) > 0;
	}

	bool loadFromFile(std::string& fileName)
	{
		std::fstream bookFile;

		bookFile.open(fileName, std::fstream::ios_base::in | std::fstream::ios_base::binary);

		if (!bookFile.is_open()) {
			return false;
		}

		while (!bookFile.eof()) {
			BookPosition learningBookPosition;

			bookFile.read((char*)&learningBookPosition, sizeof(learningBookPosition));

			Hash hashValue = learningBookPosition.hashValue;
			this->addPosition(hashValue);

			BookPosition& currentBookPosition = this->positionMap.find(hashValue)->second;
			currentBookPosition += learningBookPosition;
		}

		bookFile.close();

		return true;
	}

	bool saveToFile(std::string& fileName)
	{
		std::fstream bookFile;

		bookFile.open(fileName, std::fstream::ios_base::out | std::fstream::ios_base::binary | std::fstream::ios_base::trunc);

		if (!bookFile.is_open()) {
			return false;
		}

		for (BookPositionMapIterator it = this->positionMap.begin(); it != this->positionMap.end(); ++it) {
			Hash positionHash = it->first;
			BookPosition& bookPosition = it->second;

			if (positionHash != EmptyHash
				&& bookPosition.hashValue == positionHash) {
				bookFile.write((const char*)&(bookPosition), sizeof(BookPosition));
			}
		}

		bookFile.close();

		return true;
	}

	size_type size()
	{
		return this->positionMap.size();
	}
};
