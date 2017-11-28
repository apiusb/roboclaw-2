cmake_minimum_required(VERSION 3.2.0)
cmake_policy(SET CMP0048 NEW)

include(cmake/HunterGate.cmake)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.161.tar.gz"
    SHA1 "9af482b6a54875bbbb221274f7290324dff29919"
)

project(roboclaw VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_UI "Include the QT Test UI in the build" ON)

hunter_add_package(Boost COMPONENTS program_options system log)

add_subdirectory(roboclaw)
if (BUILD_UI)
    add_subdirectory(ui/roboclaw_ui)
endif()