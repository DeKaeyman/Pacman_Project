# app/CMakeLists.txt

find_package(SFML 2.6 CONFIG REQUIRED COMPONENTS graphics window system)

add_executable(pacman
        game/main.cpp
)

target_link_libraries(pacman
        PRIVATE
        logic
        sfml-graphics
        sfml-window
        sfml-system
)