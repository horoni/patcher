/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/
#include <cstdint>
#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::pair<int, int>> stats_t;
typedef std::map<std::string, std::vector<struct patch_t>> switches_t;

struct patch_t {
    uint64_t addr;
    uint8_t original, replace;
};

struct config_t {
  std::string file_path;
  stats_t stats;
  switches_t switches;
};

void parse_config(const std::string& path, struct config_t& conf);
