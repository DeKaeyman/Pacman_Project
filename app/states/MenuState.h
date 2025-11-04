// MenuState.h
#pragma once
#include "State.h"

namespace pacman::app {

    class MenuState : public State {
    public:
        using State::State;

        void handleEvent(const sf::Event& e) override;
        void draw(sf::RenderWindow& w) override;
    };
}