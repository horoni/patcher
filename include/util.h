/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with patcher. If not, see <https://www.gnu.org/licenses/>.
*/
#include <string>
#include <vector>

bool file_exists(const std::string& filename);
std::string format(const char *fmt, ...);
std::vector<std::string> split(std::string& s, const std::string& delimiter);
