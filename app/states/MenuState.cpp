#include "MenuState.h"

#include "score/Score.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <iomanip>
#include <sstream>
#include <utility>

namespace pacman::app {

    namespace {
        constexpr float kButtonWidth = 200.0f;
        constexpr float kButtonHeight = 60.0f;

        /**
         * @brief Computes the axis-aligned bounds of the "Play" button in window coordinates.
         * @param windowWidth Current window width in pixels.
         * @param windowHeight Current window height in pixels.
         * @return A FloatRect representing the clickable bounds.
         */
        sf::FloatRect computePlayButtonBounds(unsigned windowWidth, unsigned windowHeight) {
            const float centerX = static_cast<float>(windowWidth) * 0.5f;
            const float centerY = static_cast<float>(windowHeight) * 0.75f;

            const float left = centerX - kButtonWidth * 0.5f;
            const float top = centerY - kButtonHeight * 0.5f;

            return sf::FloatRect(left, top, kButtonWidth, kButtonHeight);
        }

        /**
         * @brief Updates cached window dimensions based on the render window.
         * @param window Render window to read size from.
         * @param outWidth Output width in pixels.
         * @param outHeight Output height in pixels.
         */
        void updateWindowSize(const sf::RenderWindow& window, unsigned& outWidth, unsigned& outHeight) {
            const auto size = window.getSize();
            outWidth = size.x;
            outHeight = size.y;
        }

        /**
         * @brief Builds five formatted highscore lines with zero-padding.
         * @param highscores Highscore values loaded from storage.
         * @return Vector containing exactly five formatted lines.
         */
        std::vector<std::string> buildHighscoreLines(const std::vector<int>& highscores) {
            std::vector<std::string> lines;
            lines.reserve(5);

            for (int i = 0; i < 5; ++i) {
                const int value = (i < static_cast<int>(highscores.size())) ? highscores[i] : 0;

                std::ostringstream oss;
                oss << (i + 1) << ". " << std::setw(5) << std::setfill('0') << value;

                lines.push_back(oss.str());
            }

            return lines;
        }

        /**
         * @brief Configures text origin so it is centered based on its local bounds.
         * @param text Text object to center.
         */
        void centerTextOrigin(sf::Text& text) {
            const auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        }
    } // namespace

    /**
     * @brief Constructs the menu state, loads resources, and reads highscores.
     * @param manager Reference to the central StateManager.
     */
    MenuState::MenuState(StateManager& manager) : State(manager) {
        font_.loadFromFile("assets/fonts/Crackman.otf");
        highscores_ = pacman::logic::Score::loadHighscores(highscorePath_);
    }

    /**
     * @brief Handles keyboard and mouse interactions for starting the game.
     * @param event The SFML event to process.
     */
    void MenuState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            push("level");
            return;
        }

        if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
            return;
        }

        const auto bounds = computePlayButtonBounds(windowWidth_, windowHeight_);
        const float mx = static_cast<float>(event.mouseButton.x);
        const float my = static_cast<float>(event.mouseButton.y);

        if (bounds.contains(mx, my)) {
            push("level");
        }
    }

    /**
     * @brief Draws the menu: title, highscores, and the play button.
     * @param window The render window to draw to.
     */
    void MenuState::draw(sf::RenderWindow& window) {
        updateWindowSize(window, windowWidth_, windowHeight_);

        sf::Text title;
        title.setFont(font_);
        title.setString("Pac-Man");
        title.setCharacterSize(64);
        title.setFillColor(sf::Color::Yellow);
        centerTextOrigin(title);
        title.setPosition(static_cast<float>(windowWidth_) * 0.5f, static_cast<float>(windowHeight_) * 0.25f);
        window.draw(title);

        sf::Text scoreText;
        scoreText.setFont(font_);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);

        const auto lines = buildHighscoreLines(highscores_);
        const float startY = static_cast<float>(windowHeight_) * 0.35f;
        const float stepY = 30.0f;

        for (int i = 0; i < 5; ++i) {
            scoreText.setString(lines[i]);
            centerTextOrigin(scoreText);
            scoreText.setPosition(static_cast<float>(windowWidth_) * 0.5f, startY + static_cast<float>(i) * stepY);
            window.draw(scoreText);
        }

        const float centerX = static_cast<float>(windowWidth_) * 0.5f;
        const float centerY = static_cast<float>(windowHeight_) * 0.75f;

        sf::RectangleShape button({kButtonWidth, kButtonHeight});
        button.setOrigin(kButtonWidth * 0.5f, kButtonHeight * 0.5f);
        button.setPosition(centerX, centerY);
        button.setFillColor(sf::Color(50, 50, 150));
        button.setOutlineColor(sf::Color::White);
        button.setOutlineThickness(2.0f);
        window.draw(button);

        sf::Text playText;
        playText.setFont(font_);
        playText.setString("Play");
        playText.setCharacterSize(28);
        playText.setFillColor(sf::Color::White);
        centerTextOrigin(playText);
        playText.setPosition(centerX, centerY);
        window.draw(playText);
    }

} // namespace pacman::app