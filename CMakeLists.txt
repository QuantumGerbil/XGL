cmake_minimum_required(VERSION 3.28)

project(XGL)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
# Force the final executable to appear in the project root:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
add_compile_definitions(-D_LIBCPP_REMOVE_TRANSITIVE_INCLUDES)
add_compile_options(-fdiagnostics-color -pedantic-errors -Wall -Wextra -Wdeprecated -fexceptions)
add_link_options(-fuse-ld=lld -Wl,--color-diagnostics -fdiagnostics-color)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)

file(GLOB MODULE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cppm")
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

#add_library(XGL)
#target_sources(XGL
#	PUBLIC
#	  FILE_SET CXX_MODULES
#	  FILES
#		${MODULE_FILES}
#)

add_executable(main)
#target_include_directories(main ${SDL2_INCLUDE_DIR})
target_sources(main
  PRIVATE src/main.cpp)
target_link_libraries(main PRIVATE SDL2::SDL2 OpenGL::GL
#  PRIVATE XGL
)
