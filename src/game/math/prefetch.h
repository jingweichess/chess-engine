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

#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
#include <xmmintrin.h>
#else

#endif

void Prefetch(void* addr)
{
#if defined(__INTEL_COMPILER)
    __asm__("");
#endif

#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
    _mm_prefetch((char*)addr, _MM_HINT_T0);
#else
    __builtin_prefetch(addr, 0, 3);
#endif
}
