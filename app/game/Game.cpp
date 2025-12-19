#include "Game.h"

#include "../states/GameOverState.h"
#include "../states/LevelState.h"
#include "../states/MenuState.h"
#include "../states/PausedState.h"
#include "../states/StateManager.h"
#include "../states/VictoryState.h"

#include "../views/View.h"

#include "utils/Stopwatch.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>

namespace pacman::app {

/**
 * @brief Constructs the game window, initializes the camera, sets up shared view state, and prepares the state manager.
 * @param width Window width in pixels.
 * @param height Window height in pixels.
 * @param title Window title string.
 */
Game::Game(unsigned width, unsigned height, const char* title)
    : window_(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close),
      camera_(static_cast<int>(width), static_cast<int>(height)) {
    window_.setFramerateLimit(60);

    View::setCamera(&camera_);

    prepareStateManager();

    pacman::logic::Stopwatch::getInstance().reset();
}

/**
 * @brief Creates the StateManager, registers all state factories, and pushes the initial state.
 */
void Game::prepareStateManager() {
    stateManager_ = std::make_unique<StateManager>();

    stateManager_->registerFactory("menu", [](StateManager& m) { return std::make_unique<MenuState>(m); });
    stateManager_->registerFactory("level", [](StateManager& m) { return std::make_unique<LevelState>(m); });
    stateManager_->registerFactory("paused", [](StateManager& m) { return std::make_unique<PausedState>(m); });
    stateManager_->registerFactory("victory", [](StateManager& m) { return std::make_unique<VictoryState>(m); });
    stateManager_->registerFactory("gameover", [](StateManager& m) { return std::make_unique<GameOverState>(m); });

    stateManager_->push("menu");
}

/**
 * @brief Runs the main game loop: event processing, fixed-step updates, and rendering.
 */
void Game::run() {
    auto& stopwatch = pacman::logic::Stopwatch::getInstance();
    stopwatch.reset();

    window_.setFramerateLimit(60);

    const double fixedDt = 1.0 / 60.0;
    const double maxFrameDt = 0.25;
    double accumulator = 0.0;

    while (window_.isOpen()) {
        sf::Event event{};
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
            stateManager_->handleEvent(event);
        }

        if (!window_.isOpen()) {
            break;
        }

        stopwatch.tick();

        double frameDt = stopwatch.deltaTime();
        if (frameDt > maxFrameDt) {
            frameDt = maxFrameDt;
        }

        accumulator += frameDt;

        while (accumulator >= fixedDt) {
            stateManager_->update(fixedDt);
            accumulator -= fixedDt;
        }

        window_.clear();
        stateManager_->draw(window_);
        window_.display();
    }
}

} // namespace pacman::app