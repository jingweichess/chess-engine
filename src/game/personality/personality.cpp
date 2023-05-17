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

#include "personality.h"

Personality::iterator Personality::begin()
{
    return this->parameterList.begin();
}

Personality::iterator Personality::end()
{
    return this->parameterList.end();
}

Score& Personality::operator[](const std::string index)
{
    return this->parameterList[index];
}

bool Personality::operator> (const Personality& p) const
{
    return this->ordinal > p.ordinal;
}

void Personality::clear()
{
    this->parameterList.clear();
}

std::int32_t Personality::getOrdinal() const
{
    return this->ordinal;
}

Score Personality::getParameter(std::string& name) const
{
    return this->parameterList.find(name)->second;
}

void Personality::loadPersonalityFile(const std::string& personalityFileName)
{
    std::fstream personalityFile;

    personalityFile.open(personalityFileName, std::fstream::ios_base::in);

    if (!personalityFile.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(personalityFile, line)) {
        std::stringstream ss(line);

        std::string parameterName;
        Score parameterScore;

        ss >> parameterName >> parameterScore;

        if (parameterName == "") {
            continue;
        }

        this->setParameter(parameterName, parameterScore);
    }

    personalityFile.close();
}

void Personality::setOrdinal(std::int32_t ordinal)
{
    this->ordinal = ordinal;
}

void Personality::setParameter(const std::string& name, Score score)
{
    //First, make sure the parameter exists
    this->parameterList[name];

    //Use a reference and add the incoming parameter to the existing one
    Score& oldScore = this->parameterList.find(name)->second;
    oldScore += score;
}
