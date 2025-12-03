#pragma once
#include <memory>
#include <vector>

#include "../entities/Direction.h"
#include "../entities/Entity.h"
#include "../factory/AbstractFactory.h"
#include "TileMap.h"

namespace pacman::logic {

inline bool intersects(const Rect& a, const Rect& b,
                       float eps = 0) { // Small AABB overlap test in world coordinates
    const bool xOverlap = (a.x < b.x + b.w - eps) && (b.x < a.x + a.w - eps);
    const bool yOverlap = (a.y < b.y + b.h - eps) && (b.y < a.y + a.h - eps);
    return xOverlap && yOverlap;
}

// Compute overlap area between two rectangles (0 if no overlap)
inline float overlapArea(const Rect& a, const Rect& b) {
    const float x1 = std::max(a.x, b.x);
    const float y1 = std::max(a.y, b.y);
    const float x2 = std::min(a.x + a.w, b.x + b.w);
    const float y2 = std::min(a.y + a.h, b.y + b.h);

    const float w = x2 - x1;
    const float h = y2 - y1;

    if (w <= 0.f || h <= 0.f)
        return 0.f;
    return w * h;
}

// Ratio of overlap relative to the smaller rect's area
inline float overlapRatio(const Rect& a, const Rect& b) {
    const float areaOverlap = overlapArea(a, b);
    if (areaOverlap <= 0.f)
        return 0.f;

    const float areaA = a.w * a.h;
    const float areaB = b.w * b.h;
    if (areaA <= 0.f || areaB <= 0.f)
        return 0.f;

    const float minArea = std::min(areaA, areaB);
    return areaOverlap / minArea; // 0.4f means "40% of the smaller rect"
}

class World {
public:
    using EntityPtr = std::shared_ptr<Entity>; // World is owner of all entities
    using EntityId = Entity::Id;

    explicit World(AbstractFactory& factory) : factory_{&factory} {};

    EntityId addEntity(EntityPtr e); // Registers entity and returns id
    bool removeEntity(EntityId id);  // removes entity based on id

    // Small entity Utils
    Entity* get(EntityId id);
    const Entity* get(EntityId id) const;

    void update(double dt); // Ticks all entities and calculates possible collisions

    template <typename F>
    void forEachEntity(F&& f) {
        for (auto& p : entities_)
            if (p && p->active)
                f(*p);
    }

    const std::vector<std::pair<EntityId, EntityId>>& lastCollisions() const {
        return lastCollisions_;
    } // exports all collisions from last update moment

    // Level flow
    void resetLevel();   // reset to startstate
    void advanceLevel(); // go to next level
    int currentLevel() const { return currentLevel_; }

    void snapshotLevelTemplate(); // Startstate of level

    const std::vector<EntityPtr>& entities() const { return entities_; } // For iterating entities

    void loadLevel(const TileMap& map); // Load a new level layout into the world.
    const TileMap& tileMap() const noexcept { return tileMap_; } // Access the current tile map for read only queries

    void setPacManDirection(Direction dir);

private:
    bool checkPacmanDesiredDirection(PacMan& pac, double dt);

    void handlePacManTurning(double dt);
    void updateEntities(double dt);
    void updateCollisions();
    void resolveCollisions();

    void updateOverlaps(float minOverlapRatio = 0.85f);
    void resolveOverlaps();

private:
    AbstractFactory* factory_{nullptr};
    std::vector<EntityPtr> entities_;                           // All entities
    std::vector<std::pair<EntityId, EntityId>> lastCollisions_; // Collisions of last frame
    std::vector<std::pair<EntityId, EntityId>> lastOverlaps_;   // Overlaps of last frame (non-blocking)

    std::vector<EntityPtr> levelTemplate_; // Start state to later reset to
    int currentLevel_{1};
    EntityId nextId_{1}; // Counter of unique id's

    TileMap tileMap_{};
};
} // namespace pacman::logic
