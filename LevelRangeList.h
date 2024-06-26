/**
 *  LevelAPI - Geometry Dash level cacher with search functionality and more.
    Copyright (C) 2023  Sergei Baigerov

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
#include <cstddef>
#include <optional>

#include "LevelRange.h"

namespace LevelAPI {
    class LevelRangeList {
    private:
        // array of ranges
        std::vector<LevelRange> _ranges;
    public:
        LevelRangeList(std::vector<LevelRange> ranges);

        // get game version by level id
        const std::string operator[](std::size_t idx);
    };
}
