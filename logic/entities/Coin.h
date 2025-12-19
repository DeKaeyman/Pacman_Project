#pragma once

#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

/**
 * @brief Collectable coin entity that awards score when picked up.
 *
 * A Coin is a non-solid entity that emits a Collected event
 * when collected. After collection, it becomes inactive.
 */
class Coin : public Entity, public Subject {
public:
    /**
     * @brief Constructs a coin with a given bounding area and score value.
     * @param area World-space bounding box of the coin.
     * @param value Score value awarded upon collection.
     */
    explicit Coin(const Rect& area, int value = 10);

    /**
     * @brief Returns the world-space bounding box of the coin.
     * @return Coin bounding rectangle.
     */
    Rect bounds() const override { return area_; }

    /**
     * @brief Updates the coin entity.
     *
     * Coins have no per-frame logic.
     *
     * @param dt Unused time step.
     */
    void update(double /*dt*/) override {}

    /**
     * @brief Returns the score value of this coin.
     * @return Score value.
     */
    int value() const noexcept { return value_; }

    /**
     * @brief Updates the coin's bounding box.
     * @param bounds New world-space bounds.
     */
    void setBounds(const Rect& bounds) { area_ = bounds; }

    /**
     * @brief Indicates whether the coin has been collected.
     * @return True if the coin is already collected.
     */
    bool isCollected() const noexcept { return !active; }

    /**
     * @brief Collects the coin and emits a Collected event.
     */
    void collect();

private:
    Rect area_{};   ///< World-space bounding box of the coin
    int value_{10}; ///< Score value awarded on collection
};

} // namespace pacman::logic