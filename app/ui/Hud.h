// app/ui/Hud.h
#pragma once

#include "score/Score.h"
#include "world/World.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

namespace pacman::app {

class Hud {
public:
    Hud(logic::Score& score, logic::World& world,
        const sf::Font& font); // Display score, lives and level using a shared game font

    void draw(sf::RenderWindow& window); // Render all HUD text elements to the window

private:
    logic::Score& score_; // Reference to global score tracker
    logic::World& world_; // Reference to world for current level number

    sf::Text scoreText_;
    sf::Text livesText_;
    sf::Text levelText_;
};
} // namespace pacman::app