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

#include <array>

#include "../../game/types/score.h"

#include "../types/nodetype.h"

#include "../eval/constructor.h"

extern QuadraticConstruct NullMoveMargins;
extern QuadraticConstruct NullMoveReductions;
extern QuadraticConstruct NullMoveVerificationReductions;

constexpr Depth NullMoveMargin(Depth depthLeft, std::uint32_t phase)
{
    return static_cast<Depth>(NullMoveMargins(depthLeft)(phase));
}

constexpr Depth NullMoveReduction(Depth depthLeft, std::uint32_t phase)
{
    return static_cast<Depth>(NullMoveReductions(depthLeft)(phase) / 256);
}

constexpr Depth NullMoveVerificationReduction(Depth depthLeft, std::uint32_t phase)
{
    return static_cast<Depth>(NullMoveVerificationReductions(depthLeft)(phase) / 256);
}
