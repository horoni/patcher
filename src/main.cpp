#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#include <gui.h>
#include <json.hpp>
using json = nlohmann::json;

typedef uint8_t u8;
typedef uint64_t u64;

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

struct patch_t {
    u64 addr;
    u8 original, replace;
};

bool config_found = true;
bool file_found = true;
std::string config_path = "config.json";
std::string file_path = "";
std::fstream file;
std::map<std::string, std::pair<nk_bool, nk_bool>> stats;
std::map<std::string, std::vector<struct patch_t>> switches;

std::string format(const char *fmt, ...);
void parse_config(const std::string& path);
bool file_exists(const std::string& filename);
void print_map(std::map<std::string, std::vector<struct patch_t>> &map);
std::vector<std::string> split(std::string& s, const std::string& delimiter);

int main(int argc, char *argv[])
{
    App app("Patcher", WINDOW_WIDTH, WINDOW_HEIGHT);
    
    if (file_exists(config_path))
        parse_config(config_path);
    else config_found = false;
    
    if (file_exists(file_path))
        file.open(file_path);
    else file_found = false;

    // validate_patches();
    // print_map(switches);
    
    app.loop();

    if(file.is_open()) file.close();
    return 0;
}

// TODO(): Reopen file
void App::menu() {
  if (nk_begin(ctx, window_name, nk_rect(0, 0, 400, 400),
      NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE))
  {
    nk_layout_row_dynamic(ctx, 25, 1);
    if (nk_button_label(ctx, "Exit")) running = 0;

    nk_layout_row_dynamic(ctx, 25, 2);
    if (nk_button_label(ctx, "Reload config")) {
        switches.clear();
        stats.clear();
        if (file_exists(config_path)) {
            parse_config(config_path);
            config_found = true;
        } else config_found = false;
    }
    if (nk_button_label(ctx, "Reload file")) {
        if (file.is_open()) file.close();
        if (file_exists(file_path)) {
            file.open(file_path);
            file_found = true;
        } else file_found = false;        
    }
    
    if (!config_found) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_wrap(ctx, format("Config \"%s\" is not found", config_path.c_str()).c_str());
    } else if (!file_found) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label_wrap(ctx, format("File \"%s\" is not exists", file_path.c_str()).c_str());
    } else {
        nk_layout_row_dynamic(ctx, 25, 2);
        for ( auto& [key, val] : switches) {
            nk_checkbox_label(ctx, key.c_str(), &stats[key].first);
            if (stats[key].first != stats[key].second) {
                std::cout << format("status of %s changed to %d", key.c_str(), stats[key].first) << std::endl;
                for (auto& p : val) {
                    file.seekp(p.addr);
                    file.write((const char *)(stats[key].first == 1 ? &p.replace : &p.original), 1);
                }
                file.flush();
                stats[key].second = stats[key].first;
            }
        }
    }
  }
  nk_end(ctx);
}

void parse_config(const std::string& path) {
    json config_j;
    json switches_j;
    std::ifstream f(path);
    config_j = json::parse(f);
    f.close();
    for (auto& [key, value] : config_j.items()) {
        if (key == "path") {
            file_path = value.template get<std::string>();
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
        switches[title] = patchs;
        stats[title] = {0, 0};
    }
}

void print_map(std::map<std::string, std::vector<struct patch_t>> &map) {
    for ( const auto& [key, val] : map) {
        std::cout << key << " ->" << std::endl;
        for (const auto& el : val) {
            std::cout << format("%lx %x %x", el.addr, el.original, el.replace) << std::endl;
        }
        std::cout << "<-" << std::endl;
    }
}

std::vector<std::string> split(std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

bool file_exists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::vector<char> v(1024);
    while (true)
    {
        va_list args2;
        va_copy(args2, args);
        int res = vsnprintf(v.data(), v.size(), fmt, args2);
        if ((res >= 0) && (res < static_cast<int>(v.size())))
        {
            va_end(args);
            va_end(args2);
            return std::string(v.data());
        }
        size_t size;
        if (res < 0)
            size = v.size() * 2;
        else
            size = static_cast<size_t>(res) + 1;
        v.clear();
        v.resize(size);
        va_end(args2);
    }
}

