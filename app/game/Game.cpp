#include "Game.h"
#include "../states/StateManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

namespace pacman::app {

    Game::Game(unsigned width, unsigned height, const char *title) : window_(sf::VideoMode(width, height), title) {
        prepareStateManager(); // request the prepare statement
    }

    void Game::prepareStateManager() {
        stateManager_ = std::make_unique<StateManager>(); // State manager manages which state is active
    }

    void Game::run() {
        while (window_.isOpen()) { // Main loop of the window
            sf::Event e{};
            while (window_.pollEvent(e)) { // Process all system events
                if (e.type == sf::Event::Closed) window_.close();
                stateManager_->handleEvent(e); // Proceed to give events to the state manager
            }

            stateManager_->update(0.0); // Update the logic in the active state

            window_.clear(); // Clear the background from the previous frame
            stateManager_->draw(window_); // Draw the active frame
            window_.display(); // Switch the buffers
        }
    }
}