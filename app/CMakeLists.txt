# app/CMakeLists.txt

find_package(SFML 2.6 CONFIG REQUIRED COMPONENTS graphics window system)

add_executable(pacman
        game/main.cpp
        states/StateManager.cpp
        states/StateManager.h
        game/Game.cpp
        game/Game.h
        states/State.cpp
        states/State.h
        states/MenuState.cpp
        states/MenuState.h
        states/LevelState.cpp
        states/LevelState.h
        states/PausedState.cpp
        states/PausedState.h
        states/VictoryState.cpp
        states/VictoryState.h
        states/GameOverState.cpp
        states/GameOverState.h
)

target_link_libraries(pacman PRIVATE logic sfml-graphics sfml-window sfml-system
)