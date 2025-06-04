/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdint>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#include <gui.h>
#include <imgui/imgui.h>
#include <config.h>
#include <util.h>

typedef uint8_t u8;
typedef uint64_t u64;

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

bool config_found = true;
bool file_found = true;
std::string config_path = "config.json";
std::fstream file;
config_t conf;

int main(int argc, char *argv[])
{
    App app("Patcher", WINDOW_WIDTH, WINDOW_HEIGHT);
    
    if (file_exists(config_path))
        parse_config(config_path, conf);
    else config_found = false;
    
    if (file_exists(conf.file_path))
        file.open(conf.file_path);
    else file_found = false;

    app.loop();

    if(file.is_open()) file.close();
    return 0;
}

void App::menu() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(win_width, win_height));
    if (ImGui::Begin(window_name, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse )) {
        if (ImGui::Button("Exit", ImVec2(-1.0f, 0.0f))) running = 0;

        float avail_width = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;
        if (ImGui::Button("Reload config", ImVec2(avail_width*0.5f, 0.0f))) {
            conf.switches.clear();
            conf.stats.clear();
            if (file_exists(config_path)) {
                parse_config(config_path, conf);
                config_found = true;
            } else config_found = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload file", ImVec2(avail_width*0.5f, 0.0f))) {
            if (file.is_open()) file.close();
            if (file_exists(conf.file_path)) {
                file.open(conf.file_path);
                file_found = true;
            } else file_found = false;
        }

        if (!config_found) {
            ImGui::Text("Config \"%s\" not found.", config_path.c_str());
        } else if (!file_found) {
            ImGui::Text("File \"%s\" is not exists.", conf.file_path.c_str());
        } else {
            for ( auto& [key, val] : conf.switches) {
                ImGui::Checkbox(key.c_str(), &conf.stats[key].first);
                if (conf.stats[key].first != conf.stats[key].second) {
                    for (auto& p : val) {
                        file.seekp(p.addr);
                        file.write((const char *)(conf.stats[key].first == 1 ? &p.replace : &p.original), 1);
                    }
                    file.flush();
                    conf.stats[key].second = conf.stats[key].first;
                }
            }
        }
    }
    ImGui::End();
}
