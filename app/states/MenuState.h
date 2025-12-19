#pragma once

#include "State.h"

#include <SFML/Graphics/Font.hpp>

#include <string>
#include <vector>

namespace pacman::app {

    /**
     * @brief Main menu state that displays title, highscores, and a button to start the game.
     */
    class MenuState : public State {
    public:
        /**
         * @brief Constructs the menu state and loads required resources.
         * @param manager Reference to the central StateManager.
         */
        explicit MenuState(StateManager& manager);

        /**
         * @brief Handles keyboard and mouse input for starting the level.
         * @param event The SFML event to process.
         */
        void handleEvent(const sf::Event& event) override;

        /**
         * @brief Draws the menu UI elements.
         * @param window The render window to draw to.
         */
        void draw(sf::RenderWindow& window) override;

    private:
        sf::Font font_;
        unsigned int windowWidth_{800};
        unsigned int windowHeight_{600};

        std::vector<int> highscores_;
        const std::string highscorePath_{"assets/data/highscores.txt"};
    };

} // namespace pacman::app