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

#include <vector>

#include <cstdint>

template <typename T>
class Statistics
{
public:
    using value_type = T;

    using size_type = std::vector<value_type>::size_type;
protected:
    std::vector<value_type> data;
    value_type sum = 0;
public:
    constexpr Statistics() = default;
    constexpr ~Statistics() = default;

    constexpr value_type average() const
    {
        if (this->data.size() == 0) {
            return 0;
        }

        return this->sum / static_cast<T>(this->data.size());
    }

    constexpr void clear()
    {
        this->data.clear();
    }

    constexpr const value_type& operator[](const size_type _Pos) const
    {
        return this->data[_Pos];
    }

    constexpr void push_back(const value_type& sample)
    {
        this->data.push_back(sample);

        this->sum += sample;
    }

    constexpr float stddev() const
    {
        if (this->data.size() == 0) {
            return 0.0f;
        }

        const value_type mean = this->average();

        float standardDeviation = 0.0;
        for (const value_type sample : this->data) {
            const value_type difference = sample - mean;
            standardDeviation += static_cast<float>(difference * difference);
        }

        return std::sqrt(standardDeviation / this->data.size());
    }
};
