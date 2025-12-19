#include "LevelState.h"

#include "StateManager.h"

#include "../factory/ConcreteFactory.h"
#include "../logic/world/World.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace pacman::app {

/**
 * @brief Constructs the level gameplay state, sets up the world, factory, HUD, and initial delay.
 * @param manager Reference to the central StateManager.
 */
LevelState::LevelState(StateManager& manager) : State(manager), tileMap_() {
    factory_ = std::make_unique<ConcreteFactory>();
    factory_->setScoreObserver(&score_);

    world_ = std::make_unique<pacman::logic::World>(*factory_);
    world_->loadLevel(tileMap_);

    hudFont_.loadFromFile("assets/fonts/Crackman.otf");
    hud_ = std::make_unique<Hud>(score_, *world_, hudFont_);

    startDelayTimer_ = startDelay_;
}

/**
 * @brief Handles user input and translates key presses into gameplay actions.
 * @param event The SFML event to process.
 */
void LevelState::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed) {
        return;
    }

    using sf::Keyboard;

    switch (event.key.code) {
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

/**
 * @brief Updates the gameplay simulation using a fixed timestep from the game loop.
 * @param dt Fixed timestep in seconds.
 */
void LevelState::update(double dt) {
    if (!world_) {
        return;
    }

    if (startDelayTimer_ > 0.0) {
        startDelayTimer_ -= dt;
        if (startDelayTimer_ < 0.0) {
            startDelayTimer_ = 0.0;
        }

        world_->tickAnimationsOnly();
        return;
    }

    if (desiredDirection_ != pacman::logic::Direction::None) {
        world_->setPacManDirection(desiredDirection_);
    }

    if (world_->isGameOver()) {
        const int finalScore = score_.value();
        manager_.ctx.finalScore = finalScore;

        auto highs = pacman::logic::Score::loadHighscores("assets/data/highscores.txt");
        highs = pacman::logic::Score::updateHighscores(highs, finalScore);
        pacman::logic::Score::saveHighscores("assets/data/highscores.txt", highs);

        push("gameover");
        return;
    }

    world_->update(dt);

    if (world_->isLevelCleared()) {
        factory_->views().clear();
        world_->advanceLevel();
        score_.add(1000);
        desiredDirection_ = pacman::logic::Direction::None;
        startDelayTimer_ = startDelay_;
        push("victory");
    }
}

/**
 * @brief Draws the level views and HUD for the current frame.
 * @param window The render window to draw to.
 */
void LevelState::draw(sf::RenderWindow& window) {
    const auto size = window.getSize();
    windowWidth_ = size.x;
    windowHeight_ = size.y;

    if (factory_) {
        factory_->setWindow(window);
        factory_->views().drawAll(window);
    }

    if (hud_) {
        hud_->draw(window);
    }
}

} // namespace pacman::app