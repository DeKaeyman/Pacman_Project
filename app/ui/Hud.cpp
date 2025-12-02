// app/ui/Hud.cpp
#include "Hud.h"

#include "score/Score.h"
#include "world/World.h"

namespace pacman::app {

    Hud::Hud(logic::Score &score, logic::World &world, const sf::Font &font) : score_(score), world_(world) {
        const unsigned int fontSize = 22; // Consistent HUD text size

        scoreText_.setFont(font);
        scoreText_.setCharacterSize(fontSize);
        scoreText_.setFillColor(sf::Color::White);
        scoreText_.setPosition(10.f, 5.f);

        livesText_.setFont(font);
        livesText_.setCharacterSize(fontSize);
        livesText_.setFillColor(sf::Color::White);
        livesText_.setPosition(10.f, 30.f);

        levelText_.setFont(font);
        levelText_.setCharacterSize(fontSize);
        levelText_.setFillColor(sf::Color::White);
        levelText_.setPosition(10.f, 55.f);
    }

    void Hud::draw(sf::RenderWindow& window) {
        // Build dynamic text for current score
        scoreText_.setString("Score: " + std::to_string(score_.value()));
        livesText_.setString("Lives: " + std::to_string(lives_));
        levelText_.setString("Level: " + std::to_string(world_.currentLevel()));

        window.draw(scoreText_);
        window.draw(livesText_);
        window.draw(levelText_);
    }
}