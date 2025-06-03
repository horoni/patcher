#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR

#ifdef __linux__
#  define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#  define NK_INCLUDE_FONT_BAKING
#  define NK_INCLUDE_DEFAULT_FONT
#  define NK_SDL_GL2_IMPLEMENTATION
#endif

#include <nuklear.h>

#ifdef __linux__
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_opengl.h>
#elif _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <nuklear_gdip.h>
#endif

class App {
private:
#ifdef __linux__
    SDL_Window *win;
    SDL_GLContext glContext;
#elif _WIN32
    GdipFont* font;
    WNDCLASSW wc;
    HWND wnd;
    int needs_refresh = 1;
#endif
    struct nk_context *ctx;
    struct nk_colorf bg;
    int win_width, win_height;
    int running = 1;
    const char *window_name;
public:
  App(const char *w_name, int w, int h);
  void menu();
  void loop();
  void cleanup();
};
