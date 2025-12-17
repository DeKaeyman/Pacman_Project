#include "PausedState.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace pacman::app {

void PausedState::init(const sf::RenderWindow& w) {
    if (initialized_)
        return;
    initialized_ = true;

    font_.loadFromFile("assets/fonts/Crackman.otf");

    title_.setFont(font_);
    title_.setString("PAUSED");
    title_.setCharacterSize(64);
    title_.setFillColor(sf::Color::Yellow);
    title_.setOutlineThickness(2.f);
    title_.setOutlineColor(sf::Color::Black);

    hint_.setFont(font_);
    hint_.setString("Press ESC to resume");
    hint_.setCharacterSize(32);
    hint_.setFillColor(sf::Color(220, 220, 220));

    auto center = [&](sf::Text& t, float y) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.width / 2.f, b.height / 2.f);
        t.setPosition(w.getSize().x / 2.f, y);
    };

    center(title_, w.getSize().y / 2.f - 40.f);
    center(hint_, w.getSize().y / 2.f + 30.f);
}

void PausedState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
        pop();
    }
}

void PausedState::draw(sf::RenderWindow& w) {
    init(w);

    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(w.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    w.draw(overlay);
    w.draw(title_);
    w.draw(hint_);
}
} // namespace pacman::app