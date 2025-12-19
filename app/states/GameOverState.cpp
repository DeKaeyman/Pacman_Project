#include "GameOverState.h"

#include "StateManager.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

namespace pacman::app {

    namespace {
        /**
         * @brief Centers an SFML text object horizontally on the window and places it at a given y-position.
         * @param text The text to center.
         * @param window The window providing the target dimensions.
         * @param y The y-position (in pixels) to place the centered text at.
         */
        void centerText(sf::Text& text, const sf::RenderWindow& window, float y) {
            const auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
            text.setPosition(static_cast<float>(window.getSize().x) * 0.5f, y);
        }
    } // namespace

    /**
     * @brief Initializes UI resources and text elements (lazy initialization).
     * @param window Render window used to compute layout.
     */
    void GameOverState::init(const sf::RenderWindow& window) {
        if (initialized_) {
            return;
        }
        initialized_ = true;

        font_.loadFromFile("assets/fonts/Crackman.otf");

        title_.setFont(font_);
        title_.setString("GAME OVER");
        title_.setCharacterSize(64);
        title_.setFillColor(sf::Color::Red);

        scoreText_.setFont(font_);
        scoreText_.setCharacterSize(32);
        scoreText_.setFillColor(sf::Color::White);
        scoreText_.setString("Final score: " + std::to_string(manager_.ctx.finalScore));

        hint_.setFont(font_);
        hint_.setCharacterSize(32);
        hint_.setFillColor(sf::Color(200, 200, 200));
        hint_.setString("Press any key to return to menu");

        const float centerY = static_cast<float>(window.getSize().y) * 0.5f;
        centerText(title_, window, centerY - 60.0f);
        centerText(scoreText_, window, centerY);
        centerText(hint_, window, centerY + 60.0f);
    }

    /**
     * @brief Handles input for leaving the game over screen and returning to the menu.
     * @param event The SFML event to process.
     */
    void GameOverState::handleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed) {
            return;
        }

        pop();
        pop();
        push("menu");
    }

    /**
     * @brief Draws the game over UI.
     * @param window The render window to draw to.
     */
    void GameOverState::draw(sf::RenderWindow& window) {
        init(window);
        window.draw(title_);
        window.draw(scoreText_);
        window.draw(hint_);
    }

} // namespace pacman::app