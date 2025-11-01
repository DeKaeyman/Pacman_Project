cmake_minimum_required(VERSION 3.28)
project(Pacman LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subprojects
add_subdirectory(logic)
add_subdirectory(app)