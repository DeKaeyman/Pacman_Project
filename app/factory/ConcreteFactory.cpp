#include "ConcreteFactory.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "../logic/entities/Coin.h"
#include "../logic/entities/Fruit.h"

#include "../views/CoinView.h"
#include "../views/FruitView.h"

#include <cassert>

namespace pacman::app {

    template <typename T>
    static std::shared_ptr<T> notImplementedModel() { // Placeholder for unimplemented model creation
        return nullptr;
    }

    std::shared_ptr<logic::PacMan> ConcreteFactory::createPacMan() { // Creates PacMan and attaches view
        auto m = /* std::make_shared<logic::PacMan>(...) */ notImplementedModel<logic::PacMan>(); // Stub creation
        attachViewToModel(m); // Connect model to view
        return m; // Return PacMan model
    }

    std::shared_ptr<logic::Ghost> ConcreteFactory::createGhost(logic::GhostKind kind) {
        auto m = /* std::make_shared<logic::Ghost>(kind, ...) */ notImplementedModel<logic::Ghost>(); // Stub creation
        attachViewToModel(m); // Connect model to view
        return m; // Return Ghost model
    }

    std::shared_ptr<logic::Coin> ConcreteFactory::createCoin() {
        logic::Rect initial{};
        auto m = std::make_shared<logic::Coin>(initial);
        attachViewToModel(m); // Connect model to view
        return m; // Return Coin model
    }

    std::shared_ptr<logic::Fruit> ConcreteFactory::createFruit() {
        logic::Rect initial{};
        auto m = std::make_shared<logic::Fruit>(initial);
        attachViewToModel(m); // Connect model to view
        return m; // Return Fruit model
    }

    std::shared_ptr<logic::Wall> ConcreteFactory::createWall() {
        auto m = /* std::make_shared<logic::Wall>(...) */ notImplementedModel<logic::Wall>(); // Stub creation
        attachViewToModel(m); // Connect model to view
        return m; // Return Wall model
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::PacMan> &) {} // Empty stub; would connect PacMan view to render window

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Ghost> &) {} // Empty stub; would connect Ghost view to render window

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Coin>& coin) {
        if (!window_ || !coin) return;
        auto view = std::make_unique<CoinView>(coin);
        coin->attach(view.get());
        views_.add(std::move(view));
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Fruit>& fruit) {
        if (!window_ || !fruit) return;
        auto view = std::make_unique<FruitView>(fruit);
        fruit->attach(view.get());
        views_.add(std::move(view));
    }

    void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Wall> &) {} // Empty stub; would connect Wall view to render window
}