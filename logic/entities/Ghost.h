#pragma once

#include "Direction.h"
#include "Entity.h"
#include "factory/AbstractFactory.h"
#include "observer/Subject.h"

#include <vector>

namespace pacman::logic {

class World;

/**
 * @brief High-level behavioral modes for ghosts.
 */
enum class GhostMode { Chase, Fear };

/**
 * @brief Logic model for a ghost.
 *
 * A Ghost is an entity that:
 * - emits Tick and Moved events during updates,
 * - emits StateChanged events when direction changes and when fear mode toggles,
 * - queries the World for collision checks and AI targeting,
 * - supports chase and fear behavior depending on its mode and kind.
 */
class Ghost : public Entity, public Subject {
public:
    /**
     * @brief Constructs a ghost at the given bounds, with a kind and movement speed.
     * @param startBounds Initial world-space bounds.
     * @param kind Ghost kind (used to select chase behavior).
     * @param speed Movement speed in world units per second.
     */
    Ghost(const Rect& startBounds, GhostKind kind, double speed = 0.3);

    /**
     * @brief Sets the world context used by the ghost AI.
     * @param world Pointer to the world instance.
     */
    void setWorld(World* world) noexcept { world_ = world; }

    /**
     * @brief Returns the current world-space bounds of the ghost.
     * @return Bounding rectangle.
     */
    Rect bounds() const override { return bounds_; }

    /**
     * @brief Updates AI and applies continuous movement.
     * @param dt Time step in seconds.
     */
    void update(double dt) override;

    /**
     * @brief Forces the current movement direction and notifies observers.
     *
     * Emits a StateChanged event encoding direction as an integer code:
     * Right=0, Left=1, Up=2, Down=3.
     *
     * @param dir New movement direction.
     */
    void setDirection(Direction dir) noexcept;

    /**
     * @brief Returns the current movement direction.
     * @return Current direction.
     */
    Direction direction() const noexcept { return direction_; }

    /**
     * @brief Sets movement speed in world units per second.
     * @param speed New speed.
     */
    void setSpeed(double speed) noexcept { speed_ = speed; }

    /**
     * @brief Returns movement speed in world units per second.
     * @return Current speed.
     */
    double speed() const noexcept { return speed_; }

    /**
     * @brief Returns the ghost kind.
     * @return Ghost kind.
     */
    GhostKind kind() const noexcept { return kind_; }

    /**
     * @brief Directly sets world-space bounds.
     * @param bounds New bounds.
     */
    void setBounds(const Rect& bounds) noexcept { bounds_ = bounds; }

    /**
     * @brief Returns the original spawn bounds.
     * @return Spawn bounds.
     */
    const Rect& spawnBounds() const noexcept { return spawnBounds_; }

    /**
     * @brief Resets the ghost to its spawn position and clears movement.
     */
    void resetToSpawn() noexcept;

    /**
     * @brief Returns the current ghost mode.
     * @return Current mode.
     */
    GhostMode mode() const noexcept { return mode_; }

    /**
     * @brief Indicates whether the ghost is currently in fear mode.
     * @return True if mode is Fear.
     */
    bool isInFearMode() const noexcept { return mode_ == GhostMode::Fear; }

    /**
     * @brief Sets the current mode and notifies observers of fear toggles.
     * @param mode New mode.
     */
    void setMode(GhostMode mode) noexcept;

    /**
     * @brief Sets the spawn bounds used by resetToSpawn().
     * @param bounds New spawn bounds.
     */
    void setStartBounds(const Rect& bounds) noexcept { spawnBounds_ = bounds; }

    /**
     * @brief Emits a Collected event with the ghost score value.
     */
    void collectScore();

    /**
     * @brief Returns the base speed stored at construction.
     * @return Base speed.
     */
    double baseSpeed() const noexcept { return baseSpeed_; }

private:
    /**
     * @brief Dispatches AI behavior based on gate logic and current mode.
     * @param dt Time step in seconds.
     */
    void applyStrategy(double dt);

    /**
     * @brief Fear behavior: attempts to maximize distance from Pac-Man.
     * @param dt Time step in seconds.
     */
    void applyFearStrategy(double dt);

    /**
     * @brief Chase behavior: selects a strategy based on ghost kind.
     * @param dt Time step in seconds.
     */
    void applyChaseStrategy(double dt);

    /**
     * @brief Chase behavior for GhostKind::A (random-ish patrol).
     * @param dt Time step in seconds.
     */
    void applyChaseStrategyA(double dt);

    /**
     * @brief Chooses a direction that tends towards a target point.
     * @param dt Time step in seconds.
     * @param tx Target X (world-space).
     * @param ty Target Y (world-space).
     * @return Chosen direction or Direction::None if no viable move exists.
     */
    Direction chooseDirectionTowards(double dt, float tx, float ty) const;

    /**
     * @brief Checks whether moving in the given direction for dt would collide with walls.
     * @param direction Candidate direction.
     * @param dt Time step in seconds.
     * @return True if movement is allowed.
     */
    bool isMoveViable(Direction direction, double dt) const;

    /**
     * @brief Computes Manhattan distance to a target after a hypothetical move.
     * @param direction Candidate direction.
     * @param dt Time step in seconds.
     * @param tx Target X (world-space).
     * @param ty Target Y (world-space).
     * @return Manhattan distance from ghost center after move to target.
     */
    float manhattanDistanceAfterMove(Direction direction, double dt, float tx, float ty) const;

    /**
     * @brief Collects all viable movement directions for the current state.
     * @param dt Time step in seconds.
     * @return Vector of viable directions.
     */
    std::vector<Direction> collectViableDirections(double dt) const;

    /**
     * @brief Checks whether the viable set forms an intersection or corner.
     * @param viable Directions that are currently viable.
     * @return True if there is a choice (corner or intersection).
     */
    bool isIntersectionOrCorner(const std::vector<Direction>& viable) const;

    /**
     * @brief Randomly selects a direction from a set.
     * @param dirs Candidate directions.
     * @return Selected direction or Direction::None if empty.
     */
    Direction randomDirectionFrom(const std::vector<Direction>& dirs) const;

private:
    Rect bounds_{};
    Rect spawnBounds_{};

    Direction direction_{Direction::None};

    double speed_{0.0};
    double baseSpeed_{0.0};

    GhostKind kind_{GhostKind::A};
    GhostMode mode_{GhostMode::Chase};

    int value_{200};

    World* world_{nullptr};
};

} // namespace pacman::logic