cmake_minimum_required(VERSION 3.5)
project("mc_patcher")

file(GLOB SRC src/*.cpp src/*.c)

add_executable(
  patcher
  ${SRC}
)

target_include_directories(
  patcher
  PUBLIC
  include/
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
  GL
  m
)
elseif(WIN32)
set_target_properties(patcher PROPERTIES LINK_FLAGS "-static")
target_link_libraries(
  patcher
  PRIVATE
  user32
  gdi32
  msimg32
)
endif()
