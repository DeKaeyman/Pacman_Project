#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

    class StateManager {
    public:
        void handleEvent(const sf::Event&) {} // handle the events
        void update(double /*dt*/) {} // updates the logic of the states
        void draw(sf::RenderWindow& /*w*/) {} // draw the active state on the current screen
    };
}