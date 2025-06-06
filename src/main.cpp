/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with patcher. If not, see <https://www.gnu.org/licenses/>.
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
#include <config.h>
#include <util.h>

typedef uint8_t u8;
typedef uint64_t u64;

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

bool config_found = true;
bool file_found = true;
std::string config_path = "config.json";
config_t conf;
std::fstream file;

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
  if (nk_begin(ctx, window_name, nk_rect(0, 0, 400, 400), NK_WINDOW_BORDER))
  {
    nk_layout_row_dynamic(ctx, 25, 1);
    if (nk_button_label(ctx, "Exit")) running = 0;

    nk_layout_row_dynamic(ctx, 25, 2);
    if (nk_button_label(ctx, "Reload config")) {
        conf.switches.clear();
        conf.stats.clear();
        if (file_exists(config_path)) {
            parse_config(config_path, conf);
            config_found = true;
        } else config_found = false;
    }
    if (nk_button_label(ctx, "Reload file")) {
        if (file.is_open()) file.close();
        if (file_exists(conf.file_path)) {
            file.open(conf.file_path);
            file_found = true;
        } else file_found = false;        
    }
    
    if (!config_found) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_wrap(ctx, format("Config \"%s\" is not found", config_path.c_str()).c_str());
    } else if (!file_found) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_wrap(ctx, format("File \"%s\" is not exists", conf.file_path.c_str()).c_str());
    } else {
        nk_layout_row_dynamic(ctx, 25, 2);
        for ( auto& [key, val] : conf.switches) {
            nk_checkbox_label(ctx, key.c_str(), &conf.stats[key].first);
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
  nk_end(ctx);
}
