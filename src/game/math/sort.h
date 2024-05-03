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

#include <algorithm>
#include <cstdint>

template<typename T>
constexpr bool greater(const T& a, const T& b)
{
    return a > b;
}

template <typename T>
constexpr void CombSort(T* a, std::uint32_t n)
{
	std::uint32_t gap = n;
	bool swapped = true;

	while ((gap > 1) || swapped) {
		if (gap > 1) {
			gap = (gap * 4) / 5;
		}

		swapped = false;

		for (std::uint32_t i = 0; i + gap < n; i++) {
			if (greater<T>(a[i + gap], a[i])) {
				std::swap(a[i], a[i + gap]);

				swapped = true;
			}
		}
	}
}
