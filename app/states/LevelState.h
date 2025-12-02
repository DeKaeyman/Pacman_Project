// LevelState.h
#pragma once
#include "State.h"

#include <memory>
#include <SFML/Graphics/Font.hpp>

#include "../logic/entities/Direction.h"
#include "../logic/world/TileMap.h"
#include "../logic/world/World.h"
#include "../factory/ConcreteFactory.h"

#include "../logic/score/Score.h"
#include "../ui/Hud.h"

namespace pacman::app {

    class LevelState : public State {
    public:
        explicit LevelState(StateManager& m); // Construct state and hook into StateManager

        void handleEvent(const sf::Event& e) override; // Translate SFML events into desired direction
        void update(double dt) override; // Advance world simulation
        void draw(sf::RenderWindow& w) override; // Draw all views for the current frame

    private:
        std::unique_ptr<logic::World> world_; //Owns logic world instance for this level
        logic::TileMap tileMap_; // Tile layout used to spawn entities
        std::unique_ptr<ConcreteFactory> factory_; // Creates model + attaches views for this level
        logic::Direction desiredDirection_{logic::Direction::None}; // Last request direction from player input

        logic::Score score_;
        sf::Font hudFont_;
        std::unique_ptr<Hud> hud_;

        unsigned int windowWidth_{800}; // Cached window width used for layout
        unsigned int windowHeight_{600}; // Cached window height used for layout
    };
}