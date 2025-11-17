// LevelState.h
#pragma once
#include "State.h"

#include <memory>
#include <SFML/Graphics/Font.hpp>

#include "../logic/entities/Direction.h"
#include "../logic/world/TileMap.h"
#include "../logic/world/World.h"
#include "../factory/ConcreteFactory.h"

namespace pacman::app {

    class LevelState : public State {
    public:
        explicit LevelState(StateManager& m);

        void handleEvent(const sf::Event& e) override;
        void update(double dt) override;
        void draw(sf::RenderWindow& w) override;

    private:
        std::unique_ptr<logic::World> world_;
        logic::TileMap tileMap_;

        std::unique_ptr<ConcreteFactory> factory_;
        
        logic::Direction desiredDirection_{logic::Direction::None};

        unsigned int windowWidth_{800};
        unsigned int windowHeight_{600};
    };
}