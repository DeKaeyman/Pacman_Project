#pragma once

#include "../states/StateManager.h"
#include "camera/Camera.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

namespace pacman::app {

    class Game {
    public:
        /**
         * @brief Constructs the Game instance and prepares required subsystems (window, camera, and state manager).
         * @param width Window width in pixels.
         * @param height Window height in pixels.
         * @param title Window title string.
         */
        Game(unsigned width, unsigned height, const char* title);

        /**
         * @brief Starts the main loop of the game.
         */
        void run();

    private:
        /**
         * @brief Initializes the StateManager, registers state factories, and pushes the initial state.
         */
        void prepareStateManager();

    private:
        sf::RenderWindow window_;
        pacman::logic::Camera camera_;
        std::unique_ptr<StateManager> stateManager_;
    };

} // namespace pacman::app