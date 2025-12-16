#pragma once
#include "State.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace pacman::app {

class PausedState : public State {
public:
    using State::State;

    void handleEvent(const sf::Event& e) override;
    void draw(sf::RenderWindow& w) override;

private:
    sf::Font font_;
    sf::Text title_;
    sf::Text hint_;
    bool initialized_ = false;

    void init(const sf::RenderWindow& w);
};
} // namespace pacman::app