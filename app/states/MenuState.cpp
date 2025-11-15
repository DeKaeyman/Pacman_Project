// MenuState.cpp
#include "MenuState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace pacman::app {

    namespace {
        constexpr float BUTTON_WIDTH = 200.0f; // Fixed width for the Play button
        constexpr float BUTTON_HEIGHT = 60.0f; // Fixed height for the Play button
    }

    MenuState::MenuState(pacman::app::StateManager &m) : State(m) { // Constructor, forwards StateManager to base class
        font_.loadFromFile("assets/fonts/arial.ttf"); // Load a font for displaying text
    }

    void MenuState::handleEvent(const sf::Event& e) {
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter) { // When enter is pressed -> Go to level state
            push("level"); // push new level state on the state stack
            return;
        }

        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) { // When left mouse button is pressed -> check if clicked on Play button

            // Compute center of the window
            float centerX = static_cast<float>(windowWidth_)  * 0.5f;
            float centerY = static_cast<float>(windowHeight_) * 0.75f;

            // Compute the rectangle bounds of the Play button
            float left = centerX - BUTTON_WIDTH * 0.5f;
            float top = centerY - BUTTON_HEIGHT * 0.5f;
            float right = left + BUTTON_WIDTH;
            float bottom = top + BUTTON_HEIGHT;

            // Get mouse coordinates
            float mx = static_cast<float>(e.mouseButton.x);
            float my = static_cast<float>(e.mouseButton.y);

            // If mouse coordinates lies inside the button bounds -> start the level
            if (mx >= left && mx <= right && my >= top && my <= bottom) {
                push("level"); // Push the level state on the state stack
            }
        }
    }

    void MenuState::draw(sf::RenderWindow& w) {
        // Get current window size for positioning UI elements
        auto size = w.getSize();
        windowWidth_ = size.x;
        windowHeight_ = size.y;

        // Title text
        sf::Text title; // Text object for game title
        title.setFont(font_); // Assing loaded font
        title.setString("Pac-Man"); // Displayed string
        title.setCharacterSize(64); // Big title size
        title.setFillColor(sf::Color::Yellow); // Pac-Mac color

        // Center title horizontally and vertically at 25% height
        auto titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.left + titleBounds.width * 0.5f, titleBounds.top + titleBounds.height * 0.5f);
        title.setPosition(windowWidth_ * 0.5f, windowHeight_ * 0.25f);

        w.draw(title); // Draw the title on the screen

        const char* lines[5] = { // Placeholder top 5 score lines
                "1. 00000",
                "2. 00000",
                "3. 00000",
                "4. 00000",
                "5. 00000"
        };

        sf::Text scoreText;
        scoreText.setFont(font_);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);

        float startY = windowHeight_ * 0.35f; // Start position for first score line
        float stepY  = 30.0f; // Spacing between lines

        // Draw all 5 placeholder scores centered horizontally
        for (int i = 0; i < 5; ++i) {
            scoreText.setString(lines[i]); // Set line text
            auto b = scoreText.getLocalBounds();
            scoreText.setOrigin(b.left + b.width * 0.5f, b.top  + b.height * 0.5f); // Center text
            scoreText.setPosition(windowWidth_ * 0.5f, startY + i * stepY); // Vertical placement
            w.draw(scoreText);
        }

        float centerX = static_cast<float>(windowWidth_)  * 0.5f; // Center x
        float centerY = static_cast<float>(windowHeight_) * 0.75f; // 75% down the screen

        sf::RectangleShape button({BUTTON_WIDTH, BUTTON_HEIGHT}); // Create button shape with fixed size
        button.setOrigin(BUTTON_WIDTH * 0.5f, BUTTON_HEIGHT * 0.5f); // Center origin
        button.setPosition(centerX, centerY); // Place in center
        button.setFillColor(sf::Color(50, 50, 150)); // Dark blue fill
        button.setOutlineColor(sf::Color::White); // White border
        button.setOutlineThickness(2.0f); // Slight border thickness

        w.draw(button); // Draw button rectangle

        sf::Text playText;
        playText.setFont(font_);
        playText.setString("Play");
        playText.setCharacterSize(28);
        playText.setFillColor(sf::Color::White);

        // Center the label inside the button
        auto pb = playText.getLocalBounds();
        playText.setOrigin(pb.left + pb.width * 0.5f, pb.top  + pb.height * 0.5f);
        playText.setPosition(centerX, centerY); // Center label on button

        w.draw(playText); // Draw text label
    }
}