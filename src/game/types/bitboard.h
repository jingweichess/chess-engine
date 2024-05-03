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

#include <array>
#include <bit>
#include <iostream>
#include <type_traits>

#ifdef __BMI2__
#include <immintrin.h>
#endif

#include <cassert>
#include <cstdint>

#include "../../game/math/byteswap.h"
#include "../../game/math/shift.h"

using Bitboard = std::uint64_t;
using BitboardPair = std::array<Bitboard, 2>;

constexpr Bitboard EmptyBitboard = 0x0;
constexpr Bitboard FullBitboard = 0xffffffffffffffff;

constexpr Bitboard FlipBitboardOnHorizontal(Bitboard bitboard)
{
    constexpr Bitboard k1 = 0x5555555555555555;
    constexpr Bitboard k2 = 0x3333333333333333;
    constexpr Bitboard k4 = 0x0f0f0f0f0f0f0f0f;

    bitboard = ((bitboard >> 1) & k1) | ((bitboard & k1) << 1);
    bitboard = ((bitboard >> 2) & k2) | ((bitboard & k2) << 2);
    bitboard = ((bitboard >> 4) & k4) | ((bitboard & k4) << 4);

    return bitboard;
}

static Bitboard FlipBitboardOnVertical(Bitboard bitboard)
{
    return SwapBytes(bitboard);
}

template <typename ValueType>
class BitboardBitScanForward {
public:
    static ValueType NextValue(Bitboard state) {
        return static_cast<ValueType>(std::countr_zero(state));
    }

    static Bitboard AdvanceState(Bitboard state) {
        return state & (state - 1);
    }
};

template <typename ValueType, class StateMath = BitboardBitScanForward<ValueType>>
class BitboardIterator {
private:
    const Bitboard initialState;
    Bitboard currentState;

public:
    explicit constexpr BitboardIterator(Bitboard state) : initialState(state), currentState(state) {}

    constexpr bool operator==(const BitboardIterator& other) const
    {
        return this->currentState == other.currentState;
    }

    constexpr bool operator!=(const BitboardIterator& other) const
    {
        return this->currentState != other.currentState;
    }

    constexpr ValueType operator*() const
    {
        return StateMath::NextValue(this->currentState);
    }

    constexpr BitboardIterator& operator++()
    {
        this->currentState = StateMath::AdvanceState(this->currentState);

        return *this;
    }

    constexpr BitboardIterator begin() const
    {
        return BitboardIterator{ this->initialState };
    }

    constexpr BitboardIterator end() const
    {
        return BitboardIterator{ EmptyBitboard };
    }
};

template <typename ValueType>
using ForwardBitScanBitboardIterator = BitboardIterator<ValueType>;

constexpr Bitboard Pdep(Bitboard bitboard, std::uint32_t n)
{
    if (std::is_constant_evaluated()) {
        for (std::uint32_t i = 0; i < n - 1; i++) {
            bitboard &= bitboard - 1;
        }

        return bitboard & ~(bitboard - 1);
    }
    else {
#ifdef __BMI2__
        return _pdep_u64(OneShiftedBy(n), bitboard);
#else
        for (std::uint32_t i = 0; i < n - 1; i++) {
            bitboard &= bitboard - 1;
        }

        return bitboard & ~(bitboard - 1);
#endif
    }
}

constexpr Bitboard Pext(Bitboard bitboard, Bitboard occupied)
{
    if (std::is_constant_evaluated()) {
        Bitboard result = EmptyBitboard;
        Bitboard pextBitboard = 0x1;

        do {
            const Bitboard leastSignificantBit = occupied & (0 - occupied);
            occupied &= ~leastSignificantBit;

            const bool isset = (bitboard & leastSignificantBit) != EmptyBitboard;
            result |= isset ? pextBitboard : EmptyBitboard;

            pextBitboard <<= 1;
        } while (occupied != EmptyBitboard);

        return result;
    }
    else {
#ifdef __BMI2__
        return _pext_u64(bitboard, occupied);
#else
        Bitboard result = EmptyBitboard;
        Bitboard pextBitboard = 0x1;

        do {
            const Bitboard leastSignificantBit = occupied & (0 - occupied);
            occupied &= ~leastSignificantBit;

            const bool isset = (bitboard & leastSignificantBit) != EmptyBitboard;
            result |= isset ? pextBitboard : EmptyBitboard;

            pextBitboard <<= 1;
        } while (occupied != EmptyBitboard);

        return result;
#endif
    }
}

static void PrintBitboard(Bitboard bitboard)
{
    for (std::uint32_t i = 0; i < 64; i++) {
        const bool isBitSet = (OneShiftedBy(i) & bitboard) != EmptyBitboard;

        std::cout << isBitSet ? " 1" : " 0";

        if (i % 8 == 7) {
            std::cout << std::endl;
        }
    }
}
