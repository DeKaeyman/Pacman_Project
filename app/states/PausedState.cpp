#include "PausedState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cstddef>

namespace pacman::app {

    namespace {
        constexpr float kButtonWidth = 260.0f;
        constexpr float kButtonHeight = 64.0f;
        constexpr float kButtonGap = 18.0f;

        /**
         * @brief Centers an SFML text object's origin using its local bounds.
         * @param text Text object to adjust.
         */
        void centerTextOrigin(sf::Text& text) {
            const auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        }
    } // namespace

    /**
     * @brief Performs one-time initialization of fonts, overlay, title, and button visuals.
     * @param window Render window used for initial layout.
     */
    void PausedState::init(const sf::RenderWindow& window) {
        if (initialized_) {
            return;
        }
        initialized_ = true;

        font_.loadFromFile("assets/fonts/Crackman.otf");

        title_.setFont(font_);
        title_.setString("PAUSED");
        title_.setCharacterSize(72);
        title_.setFillColor(sf::Color::Yellow);
        title_.setOutlineThickness(3.0f);
        title_.setOutlineColor(sf::Color::Black);

        overlay_.setFillColor(sf::Color(0, 0, 0, 170));

        buttons_.resize(3);

        const char* labels[3] = {"RESUME", "RESTART", "MENU"};
        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            buttons_[i].rect.setSize({kButtonWidth, kButtonHeight});
            buttons_[i].rect.setFillColor(sf::Color(30, 30, 30, 220));
            buttons_[i].rect.setOutlineThickness(3.0f);
            buttons_[i].rect.setOutlineColor(sf::Color::White);

            buttons_[i].label.setFont(font_);
            buttons_[i].label.setString(labels[i]);
            buttons_[i].label.setCharacterSize(32);
            buttons_[i].label.setFillColor(sf::Color::White);
        }

        layout(window);
    }

    /**
     * @brief Updates overlay and UI element positions based on the current window size.
     * @param window Render window used to compute layout.
     */
    void PausedState::layout(const sf::RenderWindow& window) {
        const auto ws = window.getSize();
        overlay_.setSize(sf::Vector2f{static_cast<float>(ws.x), static_cast<float>(ws.y)});

        centerTextOrigin(title_);
        title_.setPosition(static_cast<float>(ws.x) * 0.5f, static_cast<float>(ws.y) * 0.25f);

        const float centerX = static_cast<float>(ws.x) * 0.5f;
        const float startY = static_cast<float>(ws.y) * 0.45f;

        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            auto& button = buttons_[i];

            button.rect.setOrigin(kButtonWidth * 0.5f, kButtonHeight * 0.5f);
            button.rect.setPosition(centerX, startY + static_cast<float>(i) * (kButtonHeight + kButtonGap));

            centerTextOrigin(button.label);
            button.label.setPosition(button.rect.getPosition().x, button.rect.getPosition().y - 2.0f);
        }
    }

    /**
     * @brief Checks whether a given mouse position lies inside the button rectangle.
     * @param index Index of the button in the button list.
     * @param mx Mouse x position in window coordinates.
     * @param my Mouse y position in window coordinates.
     * @return True if the mouse point lies inside the button bounds.
     */
    bool PausedState::hitButton(std::size_t index, float mx, float my) const {
        const auto& rect = buttons_[index].rect;

        const auto pos = rect.getPosition();
        const auto origin = rect.getOrigin();
        const auto size = rect.getSize();

        const float left = pos.x - origin.x;
        const float top = pos.y - origin.y;
        const float right = left + size.x;
        const float bottom = top + size.y;

        return (mx >= left && mx <= right && my >= top && my <= bottom);
    }

    /**
     * @brief Handles pause menu input: resume, restart, or return to menu.
     * @param event The SFML event to process.
     */
    void PausedState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            pop();
            return;
        }

        if (event.type == sf::Event::Resized) {
            return;
        }

        if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
            return;
        }

        const float mx = static_cast<float>(event.mouseButton.x);
        const float my = static_cast<float>(event.mouseButton.y);

        if (hitButton(0, mx, my)) {
            pop();
            return;
        }

        if (hitButton(1, mx, my)) {
            pop();
            pop();
            push("level");
            return;
        }

        if (hitButton(2, mx, my)) {
            pop();
            pop();
            push("menu");
            return;
        }
    }

    /**
     * @brief Draws the pause overlay, title, and buttons (with hover feedback).
     * @param window The render window to draw to.
     */
    void PausedState::draw(sf::RenderWindow& window) {
        init(window);
        layout(window);

        window.draw(overlay_);
        window.draw(title_);

        const auto mousePos = sf::Mouse::getPosition(window);
        const float mx = static_cast<float>(mousePos.x);
        const float my = static_cast<float>(mousePos.y);

        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            auto& button = buttons_[i];

            if (hitButton(i, mx, my)) {
                button.rect.setFillColor(sf::Color(70, 70, 70, 230));
            } else {
                button.rect.setFillColor(sf::Color(30, 30, 30, 220));
            }

            window.draw(button.rect);
            window.draw(button.label);
        }
    }

} // namespace pacman::app