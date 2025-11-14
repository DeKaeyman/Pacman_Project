#include "Game.h"
#include "../states/StateManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include "../states/MenuState.h"
#include "../states/LevelState.h"
#include "../states/PausedState.h"
#include "../states/VictoryState.h"
#include "../states/GameOverState.h"
#include "utils/Stopwatch.h"
#include "../views/View.h"

namespace pacman::app {

    Game::Game(unsigned width, unsigned height, const char *title) : window_(sf::VideoMode(width, height), title), camera_(static_cast<int>(width), static_cast<int>(height)) {

        View::setCamera(&camera_); // Give the camera to all views

        prepareStateManager(); // request the prepare statement
        pacman::logic::Stopwatch::getInstance().reset();
    }

    void Game::prepareStateManager() {
        stateManager_ = std::make_unique<StateManager>(); // State manager manages which state is active

        stateManager_->registerFactory("menu", [](StateManager& m){ return std::make_unique<MenuState>(m); });
        stateManager_->registerFactory("level", [](StateManager& m){ return std::make_unique<LevelState>(m); });
        stateManager_->registerFactory("paused", [](StateManager& m){ return std::make_unique<PausedState>(m); });
        stateManager_->registerFactory("victory", [](StateManager& m){ return std::make_unique<VictoryState>(m); });
        stateManager_->registerFactory("gameover", [](StateManager& m){ return std::make_unique<GameOverState>(m); });

        stateManager_->push("menu");
    }

    void Game::run() {
        auto& clock = pacman::logic::Stopwatch::getInstance();

        while (window_.isOpen()) { // Main loop of the window

            clock.tick();
            double dt = clock.deltaTime();

            if (dt > 0.066) dt = 0.066;

            sf::Event e{};
            while (window_.pollEvent(e)) { // Process all system events
                if (e.type == sf::Event::Closed) {
                    window_.close();
                } else if (e.type == sf::Event::Resized) {
                    camera_.setViewport(static_cast<int>(e.size.width), static_cast<int>(e.size.height)); // Keep the camera in sync with the window size
                }

                stateManager_->handleEvent(e); // Proceed to give events to the state manager
            }

            stateManager_->update(0.0); // Update the logic in the active state

            window_.clear(); // Clear the background from the previous frame
            stateManager_->draw(window_); // Draw the active frame
            window_.display(); // Switch the buffers
        }
    }
}
