// MenuState.h
#pragma once
#include "State.h"
#include <SFML/Graphics/Font.hpp>

namespace pacman::app {

    class MenuState : public State {
    public:
        explicit MenuState(StateManager& m); // Constructor receives reference to the StateManager

        void handleEvent(const sf::Event& e) override; // Handles keyboard and mouse input
        void draw(sf::RenderWindow& w) override; // Draws the menu elements

    private:
        sf::Font font_; // Font used for all texts
        unsigned int windowWidth_{800}; // Stores last known window width
        unsigned int windowHeight_{600}; // Stores last known window height
    };
}