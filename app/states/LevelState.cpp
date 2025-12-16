// LevelState.cpp
#include "LevelState.h"
#include "StateManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "../logic/entities/Coin.h"
#include "../logic/entities/Fruit.h"

#include "../factory/ConcreteFactory.h"
#include "../logic/world/World.h"

namespace pacman::app {

LevelState::LevelState(pacman::app::StateManager& m) : State(m), tileMap_() { // Construct LevelState and init tilemap_
    factory_ = std::make_unique<ConcreteFactory>();                           // Create app side factory
    factory_->setScoreObserver(&score_);

    world_ = std::make_unique<pacman::logic::World>(*factory_); // Create world and inject abstract factory
    world_->loadLevel(tileMap_);                                // Build entities based on tile layout

    hudFont_.loadFromFile("assets/fonts/Crackman.otf");      // Load the font for the hud text
    hud_ = std::make_unique<Hud>(score_, *world_, hudFont_); // Create app side hud
}

void LevelState::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) { // Only react to key press events
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
        case Keyboard::Escape:
            push("paused");
            break;
        default:
            break;
        }
    }
}

void LevelState::update(double dt) {
    if (world_) { // Only update if world exists
        if (desiredDirection_ != pacman::logic::Direction::None) {
            world_->setPacManDirection(desiredDirection_);
        }

        if (world_ && world_->isGameOver()) {
            manager_.ctx.finalScore = score_.value();
            push("gameover");
            return;
        }

        if (world_ && score_.value() >= 1000) {
            manager_.ctx.finalScore = score_.value();
            push("victory");
            return;
        }

        world_->update(dt); // Advance game simulation by dt
    }
}

void LevelState::draw(sf::RenderWindow& w) {
    const auto size = w.getSize(); // Read current window size
    windowWidth_ = size.x;         // Cache width
    windowHeight_ = size.y;        // Cache height

    if (factory_) {
        factory_->setWindow(w);       // Provide render window to factory/views
        factory_->views().drawAll(w); // Draw all registered views
    }

    if (hud_) {
        hud_->draw(w);
    }
}
} // namespace pacman::app