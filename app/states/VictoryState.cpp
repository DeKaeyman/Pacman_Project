#include "VictoryState.h"

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
void VictoryState::init(const sf::RenderWindow& window) {
    if (initialized_)
        return;
    initialized_ = true;

    if (!font_.loadFromFile("../assets/fonts/Crackman.otf")) {
        throw std::runtime_error("Missing/failed to load font: assets/fonts/Crackman.otf");
    }

    title_.setFont(font_);
    title_.setString("LEVEL CLEARED!");
    title_.setCharacterSize(64);
    title_.setFillColor(sf::Color::Green);

    hint_.setFont(font_);
    hint_.setString("Press any key to continue");
    hint_.setCharacterSize(32);
    hint_.setFillColor(sf::Color(200, 200, 200));

    const float centerY = static_cast<float>(window.getSize().y) * 0.5f;
    centerText(title_, window, centerY - 40.f);
    centerText(hint_, window, centerY + 40.f);
}

/**
 * @brief Handles input for leaving the victory screen and returning to the menu.
 * @param event The SFML event to process.
 */
void VictoryState::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed)
        return;
    pop();
}

/**
 * @brief Draws the victory UI.
 * @param window The render window to draw to.
 */
void VictoryState::draw(sf::RenderWindow& window) {
    init(window);
    window.draw(title_);
    window.draw(hint_);
}

} // namespace pacman::app