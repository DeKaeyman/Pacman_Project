#pragma once
#include "../states/StateManager.h"
#include "camera/Camera.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

namespace pacman::app {

class Game {
public:
    Game(unsigned width, unsigned height, const char* title);
    void run(); // Start the main loop of the game

private:
    void prepareStateManager(); // Help method for initializing the StateManager

private:
    sf::RenderWindow window_;                    // The SFML window that displays the game
    pacman::logic::Camera camera_;               // Logical camera bound to this window
    std::unique_ptr<StateManager> stateManager_; // Points to the active screen
};
} // namespace pacman::app