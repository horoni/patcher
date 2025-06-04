/*  Copyright (C) 2025  horoni
  This file is part of patcher.
  patcher is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  patcher is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.
*/

#include <gui.h>
#include <cstdlib>
#include <GL/gl.h>
#include <imgui/imgui.h>

#if defined(__linux__)
#  include <imgui/backends/imgui_impl_opengl3.h>
#  include <imgui/backends/imgui_impl_sdl2.h>
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_opengl.h>
#elif defined(_WIN32)
#  include <memory>
#  include <iostream>
#  include <imgui/backends/imgui_impl_opengl3.h>
#  include <imgui/backends/imgui_impl_win32.h>
#endif

#if defined(_WIN32)
struct WGL_WindowData { HDC hDC; };

bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
void ResetDeviceWGL();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::wstring to_wstring(const char* str);

static WGL_WindowData g_MainWindow;
WNDCLASSEXW wc;
HGLRC g_hRC;
HWND hwnd;
#endif

App::App(const char *w_name, int w, int h) {
  window_name = w_name;
  win_width = w;
  win_height = h;
#if defined(__linux__)
  /* SDL setup */
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  const char* glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char* glsl_version = "#version 300 es";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, window_flags);
  gl_context = SDL_GL_CreateContext(window);
  
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync


#elif defined(_WIN32)
  DWORD style = WS_OVERLAPPEDWINDOW;
  wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui", nullptr };
  ::RegisterClassExW(&wc);
  hwnd = ::CreateWindowW(wc.lpszClassName, to_wstring(window_name).c_str(), style, 100, 100, win_width, win_height, nullptr, nullptr, wc.hInstance, nullptr);

  // Initialize OpenGL
  if (!CreateDeviceWGL(hwnd, &g_MainWindow))
  {
      CleanupDeviceWGL(hwnd, &g_MainWindow);
      ::DestroyWindow(hwnd);
      ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
      std::cerr << "[!] Failed to init OpenGL." << std::endl;
      exit(1);
  }
  wglMakeCurrent(g_MainWindow.hDC, g_hRC);

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);
#endif

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  #if defined(__linux__)
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);
  #elif defined(_WIN32)
  ImGui_ImplWin32_InitForOpenGL(hwnd);
  ImGui_ImplOpenGL3_Init();
  #endif
}

void App::loop() {
  while (running)
  {
#if defined(__linux__)
      SDL_GetWindowSize(window, &win_width, &win_height);
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
          ImGui_ImplSDL2_ProcessEvent(&event);
          if (event.type == SDL_QUIT)
              running = false;
          if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
              running = false;
      }
      if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
      {
          SDL_Delay(10);
          continue;
      }
#elif defined(_WIN32)
      MSG msg;
      while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
      {
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
          if (msg.message == WM_QUIT)
              running = false;
      }
      if (!running)
          break;
      if (::IsIconic(hwnd))
      {
          ::Sleep(10);
          continue;
      }
#endif
      ImGui_ImplOpenGL3_NewFrame();
      #if defined(__linux__)
      ImGui_ImplSDL2_NewFrame();
      #elif defined(_WIN32)
      ImGui_ImplWin32_NewFrame();
      #endif
      ImGui::NewFrame();
      /* GUI */
      menu();
      
      ImGui::Render();
      /* Draw */
      ImGuiIO& io = ImGui::GetIO();
      glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
      #if defined(__linux__)
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
      #elif defined(_WIN32)
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      #endif
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      #if defined(__linux__)
      SDL_GL_SwapWindow(window);
      #elif defined(_WIN32)
      ::SwapBuffers(g_MainWindow.hDC);
      #endif
  }
  cleanup();
}

void App::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
#if defined(__linux__)
  ImGui_ImplSDL2_Shutdown();
#elif defined(_WIN32)
  ImGui_ImplWin32_Shutdown();
#endif
  ImGui::DestroyContext();

#if defined (__linux__)
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
#elif defined(_WIN32)
  CleanupDeviceWGL(hwnd, &g_MainWindow);
  wglDeleteContext(g_hRC);
  ::DestroyWindow(hwnd);
  ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
#endif
}

#if defined(_WIN32)
std::wstring to_wstring(const char* str) {
    std::unique_ptr<wchar_t[]> tmp = nullptr;
    size_t sz, len;
    len = mbstowcs(nullptr, str, 0); // получить размер
    sz = len + 1;
    tmp.reset(new wchar_t[sz]);      // выделить память
    mbstowcs(tmp.get(), str, sz);    // перекодировать
    return std::wstring(tmp.get());
}
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            win_width = LOWORD(lParam);
            win_height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
#endif
