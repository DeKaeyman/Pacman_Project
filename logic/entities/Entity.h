#pragma once
#include <cstdint>

namespace pacman::logic {

struct Rect { // Axis aligned rectangle (AABB) for collision
    float x{}, y{}, w{}, h{};
};

class Entity { // Entity represents the game objects (Pacman, Ghost, Wall, ...)
public:
    using Id = std::uint32_t; // Unique identifier for entity recognition
    virtual ~Entity() = default;

    Id id() const { return id_; }   // Read identity of entity
    void setId(Id id) { id_ = id; } // Set identity of entity

    virtual Rect bounds() const = 0; // Axis aligned bounding box of this entity

    virtual void update(double dt) = 0;

    bool active{true}; // false => Entity will be ignored
    bool solid{true};  // false => can be phased trough
    bool visible{true}; // false => Entity is invisible

private:
    Id id_{0}; // 0 as invalid, changes when created in world
};
} // namespace pacman::logic