cmake_minimum_required(VERSION 3.30)
project("mc_patcher")

find_package(OpenGL REQUIRED)

file(GLOB SRC src/*.cpp src/*.c)
file(GLOB IMGUI_SRC third_party/imgui/*.cpp)

if (WIN32)
  set(IMGUI_BACKEND_SRC
    third_party/imgui/backends/imgui_impl_win32.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
  )
else()
  set(IMGUI_BACKEND_SRC
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
  )
endif()

add_executable(
  patcher
  ${IMGUI_SRC}
  ${IMGUI_BACKEND_SRC}
  ${SRC}
)

target_include_directories(
  patcher
  PUBLIC
  include/
  third_party/
)

if (MSVC)
  target_compile_options(patcher PRIVATE "/std:c++17")
else()
  target_compile_options(patcher PRIVATE "-std=c++17")
endif()

if(UNIX AND NOT APPLE) 
target_link_libraries(
  patcher
  PRIVATE
  SDL2
  m
  OpenGL::GL
)
elseif(WIN32)
# TODO(horoni): FIX(Doesn't work with msvc)
set_target_properties(patcher PROPERTIES LINK_FLAGS "-static")
target_link_libraries(
  patcher
  PRIVATE
  dwmapi
  OpenGL::GL
)
endif()
