#pragma once

#include <cstdint>

namespace pacman::logic {

/**
 * @brief Axis-aligned bounding box (AABB) used for collision and rendering.
 */
struct Rect {
    float x{}; ///< World-space X coordinate (top-left)
    float y{}; ///< World-space Y coordinate (top-left)
    float w{}; ///< Width in world units
    float h{}; ///< Height in world units
};

/**
 * @brief Abstract base class for all logical game entities.
 *
 * An Entity represents any object that exists in the game world, such as
 * Pac-Man, ghosts, walls, coins, or fruits. Each entity:
 * - has a unique identifier assigned by the world,
 * - exposes an axis-aligned bounding box,
 * - updates its internal state every simulation step.
 */
class Entity {
public:
    /**
     * @brief Type used to uniquely identify entities.
     */
    using Id = std::uint32_t;

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~Entity() = default;

    /**
     * @brief Returns the unique identifier of this entity.
     * @return Entity identifier.
     */
    Id id() const { return id_; }

    /**
     * @brief Assigns a unique identifier to this entity.
     * @param id Identifier provided by the world.
     */
    void setId(Id id) { id_ = id; }

    /**
     * @brief Returns the world-space axis-aligned bounding box of the entity.
     * @return Bounding rectangle.
     */
    virtual Rect bounds() const = 0;

    /**
     * @brief Updates the entity's internal logic.
     * @param dt Time step in seconds.
     */
    virtual void update(double dt) = 0;

    /**
     * @brief Whether the entity participates in logic updates and collisions.
     */
    bool active{true};

    /**
     * @brief Whether the entity blocks movement.
     *
     * If false, other entities may pass through this entity.
     */
    bool solid{true};

    /**
     * @brief Whether the entity should be rendered.
     */
    bool visible{true};

private:
    Id id_{0}; ///< Unique entity identifier (0 means uninitialized)
};

} // namespace pacman::logic