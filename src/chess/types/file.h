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

enum File {
    _A, _B, _C, _D, _E, _F, _G, _H, FILE_COUNT, NO_FILE, FIRST_FILE = _A
};

constexpr File& operator ++ (File& s, int)
{
    return s = File(int(s) + 1);
}

constexpr File operator + (File f1, File f2)
{
    return File(int(f1) + int(f2));
}

constexpr File operator - (File f1, File f2)
{
    return File(int(f1) - int(f2));
}

constexpr bool operator > (File r1, File r2)
{
    return int(r1) > int(r2);
}

constexpr bool operator < (File r1, File r2)
{
    return int(r1) < int(r2);
}

class FileIterator {
private:
    File currentState;

public:
    explicit constexpr FileIterator(File beginState = File::FIRST_FILE) : currentState(beginState) {}

    constexpr bool operator==(const FileIterator& other) const
    {
        return this->currentState == other.currentState;
    }

    constexpr bool operator!=(const FileIterator& other) const
    {
        return this->currentState != other.currentState;
    }

    constexpr File operator*() const
    {
        return this->currentState;
    }

    constexpr FileIterator& operator++()
    {
        this->currentState++;

        return *this;
    }

    constexpr FileIterator begin() const
    {
        return FileIterator{ File::FIRST_FILE };
    }

    constexpr FileIterator end() const
    {
        return FileIterator{ File::FILE_COUNT };
    }
};
