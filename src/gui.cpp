/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <gui.h>

#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>

#if defined(__linux__)
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_opengl.h>
#  include <nuklear/nuklear_sdl_gl2.h>
#elif defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <string>
#  include <memory>
#  include <windows.h>
#  define NK_GDI_IMPLEMENTATION
#  include <nuklear/nuklear_gdi.h>
#endif

#include <fstream>
extern std::fstream file;

#if defined(_WIN32)
static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;
    return DefWindowProcW(wnd, msg, wparam, lparam);
}
std::wstring to_wstring(const char* str) {
    std::unique_ptr<wchar_t[]> tmp = nullptr;
    size_t sz, len;
    len = mbstowcs(nullptr, str, 0); // получить размер
    sz = len + 1;
    tmp.reset(new wchar_t[sz]);      // выделить память
    mbstowcs(tmp.get(), str, sz);    // перекодировать
    return std::wstring(tmp.get());
}
#endif

App::App(const char *w_name, int w, int h) {
  window_name = w_name;
#if defined(__linux__)
  /* SDL setup */
  SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  win = SDL_CreateWindow(window_name,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
  glContext = SDL_GL_CreateContext(win);
  SDL_GetWindowSize(win, &win_width, &win_height);

  /* GUI */
  ctx = nk_sdl_init(win);
  {
      struct nk_font_atlas *atlas;
      nk_sdl_font_stash_begin(&atlas);
      nk_sdl_font_stash_end();
  }
#elif defined(_WIN32)
  RECT rect = { 0, 0, w, h };
  DWORD style = WS_OVERLAPPEDWINDOW;
  DWORD exstyle = WS_EX_APPWINDOW;
  ATOM atom;

  /* Win32 */
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandleW(0);
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = L"NuklearWindowClass";
  atom = RegisterClassW(&wc);

  AdjustWindowRectEx(&rect, style, FALSE, exstyle);
  wnd = CreateWindowExW(exstyle, wc.lpszClassName, to_wstring(window_name).c_str(),
      style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
      rect.right - rect.left, rect.bottom - rect.top,
      NULL, NULL, wc.hInstance, NULL);

  dc = GetDC(wnd);
  font = nk_gdifont_create("Arial", 12);
  ctx = nk_gdi_init(font, dc, w, h);
#endif

  bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
}

void App::loop() {
  while (running)
  {
#if defined(__linux__)
      /* Input */
      SDL_Event evt;
      nk_input_begin(ctx);
      while (SDL_PollEvent(&evt)) {
          if (evt.type == SDL_QUIT) cleanup();
          nk_sdl_handle_event(&evt);
      }
      nk_sdl_handle_grab(); /* optional grabbing behavior */
      nk_input_end(ctx);
#elif defined(_WIN32)
      /* Input */
      MSG msg;
      nk_input_begin(ctx);
      if (needs_refresh == 0) {
          if (GetMessageW(&msg, NULL, 0, 0) <= 0)
              cleanup();
          else {
              TranslateMessage(&msg);
              DispatchMessageW(&msg);
          }
          needs_refresh = 1;
      } else needs_refresh = 0;
      while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
          if (msg.message == WM_QUIT)
              cleanup();
          TranslateMessage(&msg);
          DispatchMessageW(&msg);
          needs_refresh = 1;
      }
      nk_input_end(ctx);
#endif
      /* GUI */
      menu();
#if defined(__linux__)
      /* Draw */
      SDL_GetWindowSize(win, &win_width, &win_height);
      glViewport(0, 0, win_width, win_height);
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(bg.r, bg.g, bg.b, bg.a);
      nk_sdl_render(NK_ANTI_ALIASING_ON);
      SDL_GL_SwapWindow(win);
#elif defined(_WIN32)
      nk_gdi_render(nk_rgb(30,30,30));
#endif
  }
  cleanup();
}

void App::cleanup() {
#if defined(__linux__)
  nk_sdl_shutdown();
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(win);
  SDL_Quit();
#elif defined(_WIN32)
  nk_gdifont_del(font);
  ReleaseDC(wnd, dc);
  UnregisterClassW(wc.lpszClassName, wc.hInstance);
#endif
}
