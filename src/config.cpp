/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with patcher. If not, see <https://www.gnu.org/licenses/>.
*/
#include <config.h>

#include <util.h>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void parse_config(const std::string& path, struct config_t& conf) {
    json config_j;
    json switches_j;
    std::ifstream f(path);
    config_j = json::parse(f);
    f.close();
    for (auto& [key, value] : config_j.items()) {
        if (key == "path") {
            conf.file_path = value.template get<std::string>();
        } else if (key == "switches") {
            switches_j = value.template get<json>();
        }
    }
    for (auto& [key, value] : switches_j.items()) {
        std::string title;
        std::vector<struct patch_t> patchs;
        for (auto& [key, value] : value.items()) {
            if (key == "title") {
                title = value.template get<std::string>();
            } else if (key == "patches") {
                for (auto& [key, value] : value.items() ) {
                    struct patch_t patch;
                    std::string patch_s = value.template get<std::string>();
                    patch_s.erase(std::remove(patch_s.begin(), patch_s.end(), ' '), patch_s.end());
                    auto patch_v = split(patch_s, ">>");
                    patch.addr = std::stoul(patch_v[0], nullptr, 16);
                    patch.original = std::stoi(patch_v[1], nullptr, 16);
                    patch.replace = std::stoi(patch_v[2], nullptr, 16);
                    patchs.push_back(patch);
                }
            }
        }
        conf.switches[title] = patchs;
        conf.stats[title] = {0, 0};
    }
}
