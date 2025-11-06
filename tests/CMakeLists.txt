cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# Catch2 v3
FetchContent_Declare(catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.2)
FetchContent_MakeAvailable(catch2)

# ---------- Logic-only tests (no SFML) ----------
add_executable(pacman_tests_logic
        test_stopwatch.cpp
        test_observer.cpp
        test_world.cpp
        test_event_payloads.cpp
)

target_link_libraries(pacman_tests_logic PRIVATE logic Catch2::Catch2WithMain)
target_include_directories(pacman_tests_logic PRIVATE ${CMAKE_SOURCE_DIR})

# ---------- App StateManager tests (SFML headers, no rendering) ----------
# We directly compile the StateManager/State sources into the test exe.
# (This avoids linking the whole app and opening windows.)
add_executable(pacman_tests_app
        ${CMAKE_SOURCE_DIR}/app/states/State.cpp
        ${CMAKE_SOURCE_DIR}/app/states/StateManager.cpp
        test_statemanager.cpp
)

# Find SFML the same way your app does; adjust components if needed.
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)
target_link_libraries(pacman_tests_app PRIVATE Catch2::Catch2WithMain sfml-graphics)
target_include_directories(pacman_tests_app PRIVATE ${CMAKE_SOURCE_DIR})

# ---------- CTest discovery ----------
include(CTest)
include(Catch)
catch_discover_tests(pacman_tests_logic)
catch_discover_tests(pacman_tests_app)