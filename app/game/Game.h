#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include "../states/StateManager.h"

namespace pacman::app {

    class Game {
    public:
        Game(unsigned width, unsigned height, const char* title);
        void run(); // Start the main loop of the game

    private:
        void prepareStateManager(); // Help method for initializing the StateManager

    private:
        sf::RenderWindow window_; // The SFML window that displays the game
        std::unique_ptr<StateManager> stateManager_; // Points to the active screen
    };
}