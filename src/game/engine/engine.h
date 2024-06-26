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

#include <iostream>
#include <string>
#include <vector>

#include "../comm/comm.h"

#include "../player/player.h"

template <class Communicator>
class Engine
{
protected:
    Communicator communicator;
public:
    constexpr Engine() = default;
    constexpr ~Engine() = default;

    constexpr void start(int argc, char** argv)
    {
        const std::vector<std::string> args(argv + 1, argv + argc);

        for (const std::string& arg : args) {
            this->communicator.processCommand(arg);
        }

        while (!this->communicator.isFinished()) {
            std::string cmd;

            if (!std::getline(std::cin, cmd)) {
                cmd = "quit";
            }

            this->communicator.processCommand(cmd);
        }
    }
};
