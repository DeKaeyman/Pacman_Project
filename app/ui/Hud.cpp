#include "Hud.h"

#include "score/Score.h"
#include "world/World.h"

#include <string>

namespace pacman::app {

    /**
     * @brief Constructs the HUD and initializes static text properties.
     * @param score Reference to the score tracker.
     * @param world Reference to the game world.
     * @param font Font used to render all HUD text.
     */
    Hud::Hud(logic::Score& score, logic::World& world, const sf::Font& font)
            : score_(score), world_(world) {
        const unsigned int fontSize = 22;

        scoreText_.setFont(font);
        scoreText_.setCharacterSize(fontSize);
        scoreText_.setFillColor(sf::Color::White);
        scoreText_.setPosition(10.0f, 5.0f);

        livesText_.setFont(font);
        livesText_.setCharacterSize(fontSize);
        livesText_.setFillColor(sf::Color::White);
        livesText_.setPosition(10.0f, 30.0f);

        levelText_.setFont(font);
        levelText_.setCharacterSize(fontSize);
        levelText_.setFillColor(sf::Color::White);
        levelText_.setPosition(10.0f, 55.0f);
    }

    /**
     * @brief Updates HUD values and renders them to the window.
     * @param window The render window to draw to.
     */
    void Hud::draw(sf::RenderWindow& window) {
        scoreText_.setString("Score: " + std::to_string(score_.value()));
        livesText_.setString("Lives: " + std::to_string(world_.lives()));
        levelText_.setString("Level: " + std::to_string(world_.currentLevel()));

        window.draw(scoreText_);
        window.draw(livesText_);
        window.draw(levelText_);
    }

} // namespace pacman::app