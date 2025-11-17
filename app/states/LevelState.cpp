// LevelState.cpp
#include "LevelState.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../factory/ConcreteFactory.h"
#include "../logic/world/World.h"

namespace pacman::app {

    LevelState::LevelState(pacman::app::StateManager &m) : State(m), tileMap_() {
        factory_ = std::make_unique<ConcreteFactory>();
        world_ = std::make_unique<pacman::logic::World>(*factory_);
        world_->loadLevel(tileMap_);
    }

    void LevelState::handleEvent(const sf::Event& e) {
        if (e.type == sf::Event::KeyPressed) {
            using sf::Keyboard;
            switch (e.key.code) {
                case Keyboard::Up:
                    desiredDirection_ = pacman::logic::Direction::Up;
                    break;
                case Keyboard::Down:
                    desiredDirection_ = pacman::logic::Direction::Down;
                    break;
                case Keyboard::Left:
                    desiredDirection_ = pacman::logic::Direction::Left;
                    break;
                case Keyboard::Right:
                    desiredDirection_ = pacman::logic::Direction::Right;
                    break;
                default:
                    break;
            }
        }
    }

    void LevelState::update(double dt) {
        if (world_) {
            world_->update(dt);
        }
    }

    void LevelState::draw(sf::RenderWindow& w) {
        const auto size = w.getSize();
        windowWidth_  = size.x;
        windowHeight_ = size.y;

        if (factory_) {
            factory_->setWindow(w);
            factory_->views().drawAll(w);
        }
    }
}