#include "ConcreteFactory.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "../logic/entities/Coin.h"
#include "../logic/entities/Fruit.h"
#include "../logic/entities/PacMan.h"
#include "../logic/entities/Ghost.h"
#include "../logic/entities/Wall.h"

#include "../views/CoinView.h"
#include "../views/FruitView.h"
#include "../views/PacManView.h"
#include "../views/GhostView.h"
#include "../views/WallView.h"

#include <cassert>

namespace pacman::app {

    template <typename T>
    static std::shared_ptr<T> notImplementedModel() { // Placeholder for unimplemented model creation
        return nullptr;
    }

    std::shared_ptr<logic::PacMan> ConcreteFactory::createPacMan() {
        logic::Rect initial{}; // Default rectangle for Pacman until world positions it
        auto m = std::make_shared<logic::PacMan>(initial); // Create model instance
        attachViewToModel(m); // Connect model to view
        return m; // Return PacMan model
    }

    std::shared_ptr<logic::Ghost> ConcreteFactory::createGhost(logic::GhostKind kind) {
        logic::Rect initial{}; // Default rectangle for Ghost until world positions it
        auto m = std::make_shared<logic::Ghost>(initial, kind); // Create model instance
        attachViewToModel(m); // Connect model to view
        return m; // Return Ghost model
    }

    std::shared_ptr<logic::Coin> ConcreteFactory::createCoin() {
        logic::Rect initial{}; // Default rectangle for Coin until world positions it
        auto m = std::make_shared<logic::Coin>(initial); // Create model instance
        attachViewToModel(m); // Connect model to view
        return m; // Return Coin model
    }

    std::shared_ptr<logic::Fruit> ConcreteFactory::createFruit() {
        logic::Rect initial{}; // Default rectangle for Fruit until world positions it
        auto m = std::make_shared<logic::Fruit>(initial); // Create model instance
        attachViewToModel(m); // Connect model to view
        return m; // Return Fruit model
    }

    std::shared_ptr<logic::Wall> ConcreteFactory::createWall() {
        auto m = std::make_shared<logic::Wall>(logic::Rect{}); // Create simple wall model with placeholder bounds
        attachViewToModel(m); // Connect model to view
        return m; // Return Wall model
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::PacMan> &pacman) {
        if (!pacman) return; // Nothing to attach if creation failed
        auto view = std::make_unique<PacManView>(pacman); // Create view tied to this model
        pacman->attach(view.get()); // Pacman observers events
        views_.add(std::move(view)); // Register view so LevelState can draw it
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Ghost> &ghost) {
        if (!ghost) return; // Nothing to attach if creation failed
        auto view = std::make_unique<GhostView>(ghost); // Create view tied to this model
        ghost->attach(view.get()); // Ghost observers events
        views_.add(std::move(view)); // Register view so LevelState can draw it
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Coin>& coin) {
        if (!coin) return; // Nothing to attach if creation failed
        auto view = std::make_unique<CoinView>(coin); // Create view tied to this model
        coin->attach(view.get()); // Coin observers events
        views_.add(std::move(view)); // Register view so LevelState can draw it
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Fruit>& fruit) {
        if (!fruit) return; // Fruit to attach if creation failed
        auto view = std::make_unique<FruitView>(fruit); // Create view tied to this model
        fruit->attach(view.get()); // Pacman observers events
        views_.add(std::move(view)); // Register view so LevelState can draw it
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Wall>& wall) {
        if (!wall) return; // Nothing to attach if creation failed

        auto view = std::make_unique<WallView>(wall); // Create view tied to this model
        views_.add(std::move(view)); // Register view so LevelState can draw it
    }
}