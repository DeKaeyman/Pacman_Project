#pragma once

#include "Direction.h"
#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

    /**
     * @brief Logic-only model for Pac-Man.
     *
     * Pac-Man is an entity that:
     * - moves continuously in its current direction using a speed (world units per second),
     * - emits Tick events each update step,
     * - emits Moved events when its bounds change,
     * - emits StateChanged events when its movement direction changes,
     * - can be reset back to a spawn position,
     * - can emit a Died event with a score payload (as implemented).
     */
    class PacMan : public Entity, public Subject {
    public:
        /**
         * @brief Constructs Pac-Man with initial bounds and movement speed.
         * @param startBounds Initial world-space bounds.
         * @param speed Movement speed in world units per second.
         */
        PacMan(const Rect& startBounds, double speed = 0.4);

        /**
         * @brief Returns the current world-space bounds of Pac-Man.
         * @return Bounding rectangle.
         */
        Rect bounds() const override { return bounds_; }

        /**
         * @brief Advances Pac-Man movement and emits Tick/Moved events.
         * @param dt Time step in seconds.
         */
        void update(double dt) override;

        /**
         * @brief Sets the current applied movement direction.
         *
         * If the direction changes, a StateChanged event is emitted with
         * an integer code describing the new direction (as implemented).
         *
         * @param dir New movement direction.
         */
        void setDirection(Direction dir) noexcept;

        /**
         * @brief Returns the currently applied movement direction.
         * @return Current direction.
         */
        Direction direction() const noexcept { return direction_; }

        /**
         * @brief Sets the desired (requested) direction.
         * @param dir Requested movement direction.
         */
        void setDesiredDirection(Direction dir) noexcept;

        /**
         * @brief Returns the desired (requested) direction.
         * @return Desired direction.
         */
        Direction desiredDirection() const noexcept { return desiredDirection_; }

        /**
         * @brief Sets the movement speed in world units per second.
         * @param speed New speed.
         */
        void setSpeed(double speed) noexcept { speed_ = speed; }

        /**
         * @brief Returns the current movement speed.
         * @return Speed in world units per second.
         */
        double speed() const noexcept { return speed_; }

        /**
         * @brief Returns the base speed stored at construction.
         * @return Base speed.
         */
        double baseSpeed() const noexcept { return baseSpeed_; }

        /**
         * @brief Directly sets Pac-Man bounds in world space.
         * @param bounds New bounding rectangle.
         */
        void setBounds(const Rect& bounds) noexcept { bounds_ = bounds; }

        /**
         * @brief Sets the spawn bounds used by resetToSpawn().
         * @param bounds New spawn bounding rectangle.
         */
        void setStartBounds(const Rect& bounds) noexcept { spawnBounds_ = bounds; }

        /**
         * @brief Resets Pac-Man to its spawn bounds and clears movement state.
         */
        void resetToSpawn() noexcept;

        /**
         * @brief Emits a Died event with the configured death score value.
         */
        void dieScore();

    private:
        Rect bounds_{};
        Direction direction_{Direction::None};
        Direction desiredDirection_{Direction::Right};

        double speed_{0.0};
        double baseSpeed_{0.0};

        Rect spawnBounds_{};

        int deathValue_{-500};
    };

} // namespace pacman::logic