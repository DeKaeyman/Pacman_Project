#include "VictoryState.h"
#include "StateManager.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

void VictoryState::init(const sf::RenderWindow& w) {
    if (initialized_)
        return;
    initialized_ = true;

    font_.loadFromFile("assets/fonts/arial.ttf");

    title_.setFont(font_);
    title_.setString("VICTORY!");
    title_.setCharacterSize(48);
    title_.setFillColor(sf::Color::Green);

    scoreText_.setFont(font_);
    scoreText_.setCharacterSize(24);
    scoreText_.setFillColor(sf::Color::White);
    scoreText_.setString("Final score: " + std::to_string(manager_.ctx.finalScore));

    hint_.setFont(font_);
    hint_.setCharacterSize(18);
    hint_.setFillColor(sf::Color(200, 200, 200));
    hint_.setString("Press any key to return to menu");

    auto center = [&](sf::Text& t, float y) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.width / 2.f, b.height / 2.f);
        t.setPosition(w.getSize().x / 2.f, y);
    };

    center(title_, w.getSize().y / 2.f - 60.f);
    center(scoreText_, w.getSize().y / 2.f);
    center(hint_, w.getSize().y / 2.f + 60.f);
}

void VictoryState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        pop(); // Victory
        pop(); // Level
        push("menu");
    }
}

void VictoryState::draw(sf::RenderWindow& w) {
    init(w);
    w.draw(title_);
    w.draw(scoreText_);
    w.draw(hint_);
}
} // namespace pacman::app