#pragma once
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "observer/Observer.h"

namespace pacman::app {

    class View : public pacman::logic::Observer { // Base UI element that listens to model events and can draw itself
    public:
        ~View() override = default;

        virtual void draw(sf::RenderWindow& window) = 0; // Render this view into the given SFML window

        void onEvent(const pacman::logic::Event& /*e*/) override {}
    };
}
