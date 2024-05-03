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

#include <cstdint>

#if defined(_MSC_VER)

#include <intrin.h>

#pragma intrinsic(_byteswap_uint64)

static std::uint64_t SwapBytes(std::uint64_t b)
{
    return _byteswap_uint64(b);
}

#elif defined(__GNUC__)

static std::uint64_t SwapBytes(std::uint64_t b)
{
    return __builtin_bswap64(b);
}

#else

static std::uint64_t SwapBytes(std::uint64_t b)
{
    std::uint64_t save = b;
    unsigned char* arr = (unsigned char*)&save;

    unsigned char temp;
    
    temp = arr[7]; arr[7] = arr[0]; arr[0] = temp;
    temp = arr[6]; arr[6] = arr[1]; arr[1] = temp;
    temp = arr[5]; arr[5] = arr[2]; arr[2] = temp;
    temp = arr[4]; arr[4] = arr[3]; arr[3] = temp;

    return save;
}

#endif
