cmake_minimum_required(VERSION 3.16)
project(PanzerWar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib QUIET)
if(NOT raylib_FOUND)
  find_library(RAYLIB_LIBRARY raylib REQUIRED)
  find_path(RAYLIB_INCLUDE_DIR raylib.h REQUIRED)
endif()

set(SOURCES
  src/main.cpp
  src/panzer.cpp
  src/settings.cpp
  src/ui.cpp
  src/button.cpp
)

add_executable(panzer_war ${SOURCES})

target_include_directories(panzer_war PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  /usr/local/include
)

if(raylib_FOUND)
  target_link_libraries(panzer_war PRIVATE raylib)
else()
  target_include_directories(panzer_war PRIVATE ${RAYLIB_INCLUDE_DIR})
  target_link_libraries(panzer_war PRIVATE ${RAYLIB_LIBRARY})
endif()

# Linux / X11 extras commonly needed with system raylib
if(UNIX AND NOT APPLE)
  target_link_libraries(panzer_war PRIVATE GL m pthread dl rt X11)
endif()

set_target_properties(panzer_war PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Convenience: run from bin so ../resources resolves
add_custom_target(run
  COMMAND ./panzer_war
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  DEPENDS panzer_war
  USES_TERMINAL
)
