/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/

#if defined(__linux__)
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_opengl.h>
#elif defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <imgui/imgui.h>

inline int win_width, win_height;

class App {
private:
#if   defined(__linux__)
    SDL_Window *window;
    SDL_GLContext gl_context;
#endif
    int running = 1;
    const char *window_name;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
public:
  App(const char *w_name, int w, int h);
  void menu();
  void loop();
  void cleanup();
};
