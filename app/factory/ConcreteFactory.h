#pragma once
#include <memory>
#include "factory/AbstractFactory.h"

namespace sf { class RenderWindow; } // Forward declare SFML window to avoid heavy include

namespace pacman::app {

    class ConcreteFactory final : public logic::AbstractFactory {
    public:
        explicit ConcreteFactory(sf::RenderWindow& window) noexcept : window_(&window) {} // Stores reference to render window for view creation
        ~ConcreteFactory() override = default;

        std::shared_ptr<logic::PacMan> createPacMan() override; // Creates PacMan model and attaches its view
        std::shared_ptr<logic::Ghost> createGhost(logic::GhostKind kind) override; // Creates Ghost model and attaches its view
        std::shared_ptr<logic::Coin> createCoin() override; // Creates Coin model and attaches its view
        std::shared_ptr<logic::Fruit> createFruit() override; // Creates Fruit model and attaches its view
        std::shared_ptr<logic::Wall> createWall() override; // Creates Wall model and attaches its view

    private:
        sf::RenderWindow* window_; // Pointer to render window

        void attachViewToModel(const std::shared_ptr<logic::PacMan>& /*model*/); // Links PacMan model to its visual representation
        void attachViewToModel(const std::shared_ptr<logic::Ghost>& /*model*/); // Links Ghost model to its visual representation
        void attachViewToModel(const std::shared_ptr<logic::Coin>& /*model*/); // Links Coin model to its visual representation
        void attachViewToModel(const std::shared_ptr<logic::Fruit>& /*model*/); // Links Fruit model to its visual representation
        void attachViewToModel(const std::shared_ptr<logic::Wall>& /*model*/); // Links Wall model to its visual representation
    };
}