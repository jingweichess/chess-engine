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

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <cstdint>

#include "../types/score.h"

class Personality {
protected:
    std::map<std::string, Score> parameterList;
    std::int32_t ordinal = 0;
public:
    using iterator = typename std::map<std::string, Score>::iterator;

    Personality() = default;
    ~Personality() = default;

    iterator begin();
    iterator end();

    Score& operator[](const std::string index);
    bool operator>(const Personality& p) const;

    void clear();
    std::int32_t getOrdinal() const;
    void setOrdinal(std::int32_t ordinal);

    Score getParameter(std::string& name) const;
    void setParameter(const std::string& name, Score score);

    void loadPersonalityFile(const std::string& personalityFileName);
};

using PersonalityList = std::vector<Personality>;
