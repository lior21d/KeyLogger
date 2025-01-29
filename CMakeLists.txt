cmake_minimum_required(VERSION 3.10)

project(KeyLogger)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(logger.exe WIN32 KeyLogger.cpp)

target_link_libraries(logger.exe user32 gdi32)
