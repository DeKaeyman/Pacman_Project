# app/CMakeLists.txt

find_package(SFML 2.6 CONFIG REQUIRED COMPONENTS graphics window system)

add_executable(pacman
        game/main.cpp
        game/Game.cpp
        game/Game.h
        states/StateManager.cpp
        states/StateManager.h
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
        factory/ConcreteFactory.h
        factory/ConcreteFactory.cpp
        views/View.h
        views/ViewRegistry.h
        views/CoinView.cpp
        views/CoinView.h
        views/FruitView.cpp
        views/FruitView.h
        views/PacManView.cpp
        views/PacManView.h
        views/WallView.cpp
        views/WallView.h
)

target_link_libraries(pacman PRIVATE logic sfml-graphics sfml-window sfml-system
)