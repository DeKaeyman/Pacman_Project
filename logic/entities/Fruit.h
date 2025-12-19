#pragma once

#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

    /**
     * @brief Collectable fruit entity that awards bonus score when picked up.
     *
     * Fruits behave similarly to coins but typically grant a higher
     * score value and appear less frequently.
     */
    class Fruit : public Entity, public Subject {
    public:
        /**
         * @brief Constructs a fruit with a given bounding area and score value.
         * @param area World-space bounding box of the fruit.
         * @param value Score value awarded upon collection.
         */
        explicit Fruit(const Rect& area, int value = 50);

        /**
         * @brief Returns the world-space bounding box of the fruit.
         * @return Fruit bounding rectangle.
         */
        Rect bounds() const override { return area_; }

        /**
         * @brief Updates the fruit entity.
         *
         * Fruits have no per-frame logic.
         *
         * @param dt Unused time step.
         */
        void update(double /*dt*/) override {}

        /**
         * @brief Returns the score value awarded by this fruit.
         * @return Score value.
         */
        int value() const noexcept { return value_; }

        /**
         * @brief Updates the fruit's bounding box.
         * @param bounds New world-space bounds.
         */
        void setBounds(const Rect& bounds) { area_ = bounds; }

        /**
         * @brief Indicates whether the fruit has been collected.
         * @return True if the fruit is already collected.
         */
        bool isCollected() const noexcept { return !active; }

        /**
         * @brief Collects the fruit and emits a Collected event.
         */
        void collect();

    private:
        Rect area_{};   ///< World-space bounding box of the fruit
        int value_{50}; ///< Score value awarded on collection
    };

} // namespace pacman::logic