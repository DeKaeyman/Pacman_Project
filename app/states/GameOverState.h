#pragma once

#include "State.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace pacman::app {

/**
 * @brief State shown when the player loses; displays final score and returns to menu on key press.
 */
class GameOverState : public State {
public:
    using State::State;

    /**
     * @brief Handles input for leaving the state.
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Draws the game over UI.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window) override;

private:
    /**
     * @brief Performs one-time initialization of fonts and text objects.
     * @param window Render window used to compute layout.
     */
    void init(const sf::RenderWindow& window);

private:
    sf::Font font_;
    sf::Text title_;
    sf::Text scoreText_;
    sf::Text hint_;
    bool initialized_ = false;
};

} // namespace pacman::app