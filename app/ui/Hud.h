#pragma once

#include "score/Score.h"
#include "world/World.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

namespace pacman::app {

/**
 * @brief Heads-up display showing score, lives, and current level.
 */
class Hud {
public:
    /**
     * @brief Constructs the HUD using shared game data and a common font.
     * @param score Reference to the global score tracker.
     * @param world Reference to the game world.
     * @param font Font used for rendering text.
     */
    Hud(logic::Score& score, logic::World& world, const sf::Font& font);

    /**
     * @brief Draws the HUD text elements.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window);

private:
    logic::Score& score_;
    logic::World& world_;

    sf::Text scoreText_;
    sf::Text livesText_;
    sf::Text levelText_;
};

} // namespace pacman::app