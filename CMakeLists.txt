cmake_minimum_required(VERSION 3.25)

project(
  SchedSim
  LANGUAGES CXX
  VERSION 0.6.9)

if(NOT DEFINED LOG_LEVEL_SCHEDSIM)
  set(LOG_LEVEL_SCHEDSIM
      0
      CACHE STRING "Log level for Scheduler" FORCE)
endif()
message(STATUS "LOG_LEVEL_SCHEDSIM = ${LOG_LEVEL_SCHEDSIM}")
add_compile_definitions(LOG_LEVEL_SCHEDSIM=${LOG_LEVEL_SCHEDSIM})

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/raylib/")
include_directories(raylib/include include/)
link_directories(raylib/)

file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(raylib REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
