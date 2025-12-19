#pragma once

#include "State.h"

#include "../factory/ConcreteFactory.h"
#include "../logic/entities/Direction.h"
#include "../logic/score/Score.h"
#include "../logic/world/TileMap.h"
#include "../logic/world/World.h"
#include "../ui/Hud.h"

#include <SFML/Graphics/Font.hpp>

#include <memory>

namespace pacman::app {

/**
 * @brief Gameplay state responsible for running a level: input, world updates, and rendering.
 */
class LevelState : public State {
public:
    /**
     * @brief Constructs the level state and initializes world, factory, HUD, and timers.
     * @param manager Reference to the central StateManager.
     */
    explicit LevelState(StateManager& manager);

    /**
     * @brief Translates SFML events into player intentions (movement, pause).
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Advances world simulation and handles transitions such as victory/game over and level clears.
     * @param dt Fixed timestep in seconds.
     */
    void update(double dt) override;

    /**
     * @brief Draws all views created for the level and the HUD.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window) override;

private:
    std::unique_ptr<logic::World> world_;
    logic::TileMap tileMap_;
    std::unique_ptr<ConcreteFactory> factory_;
    logic::Direction desiredDirection_{logic::Direction::None};

    double startDelay_ = 1.0;
    double startDelayTimer_ = 0.0;

    logic::Score score_;
    sf::Font hudFont_;
    std::unique_ptr<Hud> hud_;

    unsigned int windowWidth_{800};
    unsigned int windowHeight_{600};
};

} // namespace pacman::app