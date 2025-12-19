#include "ConcreteFactory.h"

#include "../logic/entities/Coin.h"
#include "../logic/entities/Fruit.h"
#include "../logic/entities/Ghost.h"
#include "../logic/entities/PacMan.h"
#include "../logic/entities/Wall.h"

#include "../views/CoinView.h"
#include "../views/FruitView.h"
#include "../views/GhostView.h"
#include "../views/PacManView.h"
#include "../views/WallView.h"

namespace pacman::app {

/**
 * @brief Creates a PacMan model and attaches its corresponding view and observers.
 * @return Shared pointer to the created PacMan model.
 */
std::shared_ptr<logic::PacMan> ConcreteFactory::createPacMan() {
    const logic::Rect initial{};
    auto model = std::make_shared<logic::PacMan>(initial);
    attachViewToModel(model);
    return model;
}

/**
 * @brief Creates a Ghost model of the given kind and attaches its corresponding view and observers.
 * @param kind The ghost kind to create.
 * @return Shared pointer to the created Ghost model.
 */
std::shared_ptr<logic::Ghost> ConcreteFactory::createGhost(logic::GhostKind kind) {
    const logic::Rect initial{};
    auto model = std::make_shared<logic::Ghost>(initial, kind);
    attachViewToModel(model);
    return model;
}

/**
 * @brief Creates a Coin model and attaches its corresponding view and observers.
 * @return Shared pointer to the created Coin model.
 */
std::shared_ptr<logic::Coin> ConcreteFactory::createCoin() {
    const logic::Rect initial{};
    auto model = std::make_shared<logic::Coin>(initial);
    attachViewToModel(model);
    return model;
}

/**
 * @brief Creates a Fruit model and attaches its corresponding view and observers.
 * @return Shared pointer to the created Fruit model.
 */
std::shared_ptr<logic::Fruit> ConcreteFactory::createFruit() {
    const logic::Rect initial{};
    auto model = std::make_shared<logic::Fruit>(initial);
    attachViewToModel(model);
    return model;
}

/**
 * @brief Creates a Wall model and attaches its corresponding view.
 * @return Shared pointer to the created Wall model.
 */
std::shared_ptr<logic::Wall> ConcreteFactory::createWall() {
    auto model = std::make_shared<logic::Wall>(logic::Rect{});
    attachViewToModel(model);
    return model;
}

/**
 * @brief Attaches a PacManView to the given PacMan model and registers observers if present.
 * @param pacman The PacMan model to attach the view to.
 */
void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::PacMan>& pacman) {
    if (!pacman) {
        return;
    }

    auto view = std::make_unique<PacManView>(pacman);
    pacman->attach(view.get());

    if (scoreObserver_) {
        pacman->attach(scoreObserver_);
    }

    views_.add(std::move(view));
}

/**
 * @brief Attaches a GhostView to the given Ghost model and registers observers if present.
 * @param ghost The Ghost model to attach the view to.
 */
void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Ghost>& ghost) {
    if (!ghost) {
        return;
    }

    auto view = std::make_unique<GhostView>(ghost);
    ghost->attach(view.get());

    if (scoreObserver_) {
        ghost->attach(scoreObserver_);
    }

    views_.add(std::move(view));
}

/**
 * @brief Attaches a CoinView to the given Coin model and registers observers if present.
 * @param coin The Coin model to attach the view to.
 */
void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Coin>& coin) {
    if (!coin) {
        return;
    }

    auto view = std::make_unique<CoinView>(coin);
    coin->attach(view.get());

    if (scoreObserver_) {
        coin->attach(scoreObserver_);
    }

    views_.add(std::move(view));
}

/**
 * @brief Attaches a FruitView to the given Fruit model and registers observers if present.
 * @param fruit The Fruit model to attach the view to.
 */
void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Fruit>& fruit) {
    if (!fruit) {
        return;
    }

    auto view = std::make_unique<FruitView>(fruit);
    fruit->attach(view.get());

    if (scoreObserver_) {
        fruit->attach(scoreObserver_);
    }

    views_.add(std::move(view));
}

/**
 * @brief Attaches a WallView to the given Wall model.
 * @param wall The Wall model to attach the view to.
 */
void ConcreteFactory::attachViewToModel(const std::shared_ptr<logic::Wall>& wall) {
    if (!wall) {
        return;
    }

    auto view = std::make_unique<WallView>(wall);
    views_.add(std::move(view));
}

} // namespace pacman::app