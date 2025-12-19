#pragma once

#include "Entity.h"

namespace pacman::logic {

    /**
     * @brief Static wall entity used as a solid collision blocker.
     *
     * Walls do not move or update over time and always remain active.
     */
    class Wall final : public Entity {
    public:
        /**
         * @brief Constructs a wall with the given world-space bounds.
         * @param bounds Axis-aligned bounding box of the wall.
         */
        explicit Wall(const Rect& bounds) : bounds_(bounds) {
            solid = true;
            active = true;
        }

        /**
         * @brief Returns the wall's axis-aligned bounding box.
         * @return World-space bounding rectangle.
         */
        Rect bounds() const override { return bounds_; }

        /**
         * @brief Updates the wall entity.
         *
         * Walls are static and do not update.
         *
         * @param dt Unused time step.
         */
        void update(double /*dt*/) override {}

        /**
         * @brief Updates the wall's bounding box.
         * @param bounds New world-space bounds.
         */
        void setBounds(const Rect& bounds) noexcept { bounds_ = bounds; }

    private:
        Rect bounds_{}; ///< Wall bounding box in world coordinates
    };

} // namespace pacman::logic