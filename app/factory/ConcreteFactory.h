#pragma once

#include "factory/AbstractFactory.h"

#include "../logic/score/Score.h"
#include "../views/ViewRegistry.h"

#include <memory>

namespace sf {
    class RenderWindow;
}

namespace pacman::app {

    class ConcreteFactory final : public logic::AbstractFactory {
    public:
        /**
         * @brief Constructs a factory without a render window set.
         */
        ConcreteFactory() noexcept : window_(nullptr) {}

        /**
         * @brief Constructs a factory using the given render window.
         * @param window The render window to store for potential view-related usage.
         */
        explicit ConcreteFactory(sf::RenderWindow& window) noexcept : window_(&window) {}

        /**
         * @brief Default destructor.
         */
        ~ConcreteFactory() override = default;

        /**
         * @brief Sets/updates the render window used by this factory.
         * @param window The render window to store.
         */
        void setWindow(sf::RenderWindow& window) noexcept { window_ = &window; }

        /**
         * @brief Creates a PacMan model and attaches its view/observers.
         * @return Shared pointer to the created PacMan model.
         */
        std::shared_ptr<logic::PacMan> createPacMan() override;

        /**
         * @brief Creates a Ghost model and attaches its view/observers.
         * @param kind The ghost kind to create.
         * @return Shared pointer to the created Ghost model.
         */
        std::shared_ptr<logic::Ghost> createGhost(logic::GhostKind kind) override;

        /**
         * @brief Creates a Coin model and attaches its view/observers.
         * @return Shared pointer to the created Coin model.
         */
        std::shared_ptr<logic::Coin> createCoin() override;

        /**
         * @brief Creates a Fruit model and attaches its view/observers.
         * @return Shared pointer to the created Fruit model.
         */
        std::shared_ptr<logic::Fruit> createFruit() override;

        /**
         * @brief Creates a Wall model and attaches its view.
         * @return Shared pointer to the created Wall model.
         */
        std::shared_ptr<logic::Wall> createWall() override;

        /**
         * @brief Sets the score observer that will be attached to newly created models where applicable.
         * @param score Pointer to the score observer (not owned).
         */
        void setScoreObserver(pacman::logic::Score* score) noexcept { scoreObserver_ = score; }

        /**
         * @brief Provides access to the registry of views created by this factory.
         * @return Reference to the internal ViewRegistry.
         */
        ViewRegistry& views() noexcept { return views_; }

    private:
        sf::RenderWindow* window_;
        pacman::logic::Score* scoreObserver_{nullptr};
        ViewRegistry views_;

        /**
         * @brief Attaches the PacMan view and observers to the given model.
         * @param pacman The PacMan model instance.
         */
        void attachViewToModel(const std::shared_ptr<logic::PacMan>& pacman);

        /**
         * @brief Attaches the Ghost view and observers to the given model.
         * @param ghost The Ghost model instance.
         */
        void attachViewToModel(const std::shared_ptr<logic::Ghost>& ghost);

        /**
         * @brief Attaches the Coin view and observers to the given model.
         * @param coin The Coin model instance.
         */
        void attachViewToModel(const std::shared_ptr<logic::Coin>& coin);

        /**
         * @brief Attaches the Fruit view and observers to the given model.
         * @param fruit The Fruit model instance.
         */
        void attachViewToModel(const std::shared_ptr<logic::Fruit>& fruit);

        /**
         * @brief Attaches the Wall view to the given model.
         * @param wall The Wall model instance.
         */
        void attachViewToModel(const std::shared_ptr<logic::Wall>& wall);
    };

} // namespace pacman::app