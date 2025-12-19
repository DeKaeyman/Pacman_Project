#pragma once

#include "State.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cstddef>
#include <vector>

namespace pacman::app {

    /**
     * @brief Pause menu state shown on top of gameplay, offering resume/restart/menu actions.
     */
    class PausedState : public State {
    public:
        using State::State;

        /**
         * @brief Handles pause state input (escape and mouse clicks).
         * @param event The SFML event to process.
         */
        void handleEvent(const sf::Event& event) override;

        /**
         * @brief Draws the pause overlay and menu UI.
         * @param window The render window to draw to.
         */
        void draw(sf::RenderWindow& window) override;

    private:
        /**
         * @brief Performs one-time initialization of resources and base UI styling.
         * @param window Render window used for initial layout.
         */
        void init(const sf::RenderWindow& window);

        /**
         * @brief Recomputes UI layout for the current window dimensions.
         * @param window Render window used to compute layout.
         */
        void layout(const sf::RenderWindow& window);

        /**
         * @brief Tests if a mouse coordinate hits the button at a given index.
         * @param index Index in the buttons array.
         * @param mx Mouse x position in window coordinates.
         * @param my Mouse y position in window coordinates.
         * @return True if the point is inside the button bounds.
         */
        bool hitButton(std::size_t index, float mx, float my) const;

    private:
        sf::Font font_;
        sf::Text title_;
        bool initialized_ = false;

        sf::RectangleShape overlay_;

        struct Button {
            sf::RectangleShape rect;
            sf::Text label;
        };

        std::vector<Button> buttons_;
    };

} // namespace pacman::app