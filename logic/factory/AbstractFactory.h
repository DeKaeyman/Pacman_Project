#pragma once

#include <cstdint>
#include <memory>

namespace pacman::logic {

    class PacMan;
    class Ghost;
    class Coin;
    class Fruit;
    class Wall;

    /**
     * @brief Enum defining the available ghost variants.
     */
    enum class GhostKind : std::uint8_t { A = 0, B, C, D };

    /**
     * @brief Interface for factories that create logic-layer game entities.
     *
     * Concrete implementations may also attach observers/views on the app side,
     * but the in/out contract of each create method must remain stable.
     */
    class AbstractFactory {
    public:
        /**
         * @brief Virtual destructor for safe polymorphic destruction.
         */
        virtual ~AbstractFactory() = default;

        /**
         * @brief Creates a PacMan model instance.
         * @return Shared pointer to the created PacMan model.
         */
        virtual std::shared_ptr<PacMan> createPacMan() = 0;

        /**
         * @brief Creates a Ghost model instance of the given kind.
         * @param kind The ghost kind to create.
         * @return Shared pointer to the created Ghost model.
         */
        virtual std::shared_ptr<Ghost> createGhost(GhostKind kind) = 0;

        /**
         * @brief Creates a Coin model instance.
         * @return Shared pointer to the created Coin model.
         */
        virtual std::shared_ptr<Coin> createCoin() = 0;

        /**
         * @brief Creates a Fruit model instance.
         * @return Shared pointer to the created Fruit model.
         */
        virtual std::shared_ptr<Fruit> createFruit() = 0;

        /**
         * @brief Creates a Wall model instance.
         * @return Shared pointer to the created Wall model.
         */
        virtual std::shared_ptr<Wall> createWall() = 0;
    };

} // namespace pacman::logic