#pragma once
#include <memory>
#include <vector>

#include "../entities/Direction.h"
#include "../entities/Entity.h"
#include "../factory/AbstractFactory.h"
#include "TileMap.h"

namespace pacman::logic {

struct GatePass {
    std::shared_ptr<Ghost> ghost;
    bool touchedGate{false};
};

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
    } // exports all collisions from last update moment²&

    // Level flow
    void resetLevel();   // reset to startstate
    void advanceLevel(); // go to next level
    int currentLevel() const { return currentLevel_; }
    int lives() const noexcept { return lives_; }

    void snapshotLevelTemplate(); // Startstate of level

    const std::vector<EntityPtr>& entities() const { return entities_; } // For iterating entities

    void loadLevel(const TileMap& map);                          // Load a new level layout into the world.
    const TileMap& tileMap() const noexcept { return tileMap_; } // Access the current tile map for read only queries

    void setPacManDirection(Direction dir);

    const Wall* ghostGate() const noexcept;
    bool canGhostPassGate(const Ghost* g) const noexcept;

private:
    bool checkPacmanDesiredDirection(PacMan& pac,
                                     double dt); // Check if Pac-Man can turn into his buffered desired direction

    void handlePacManTurning(double dt); // Evaluate buffered turning requests BEFORE movement each frame
    void updateEntities(double dt);      // Call update(dt) on all active entities
    void updateCollisions();             // Detect solid <-> solid collisions
    void resolveCollisions();            // Resolve solid collisions by physically correcting positions

    void updateOverlaps(float minOverlapRatio = 0.85f); // Detect “soft” overlaps
    void resolveOverlaps(); // Handle soft overlaps: collect coins, trigger fruit effects, etc.

    void startFearMode();            // Put all ghosts into fear mode, reset fear timer
    void stopFearMode();             // Restore ghosts to normal chase mode once timer expires
    void updateFearTimer(double dt); // Reduce fear timer. exit fear mode automatically at timeout

    void startGhostReleaseClocks();
    void updateGhostRelease();

    void respawnEatenGhost(Ghost& ghost);
    void resetActorsAfterPacmanHit(PacMan& pac);

private:
    AbstractFactory* factory_{nullptr};
    std::vector<EntityPtr> entities_;                           // All entities
    std::vector<std::pair<EntityId, EntityId>> lastCollisions_; // Collisions of last frame
    std::vector<std::pair<EntityId, EntityId>> lastOverlaps_;   // Overlaps of last frame (non-blocking)

    std::vector<EntityPtr> levelTemplate_; // Start state to later reset to
    int currentLevel_{1};
    EntityId nextId_{1}; // Counter of unique id's

    TileMap tileMap_{};

    int lives_{3};

    // Fear mode for ghosts
    bool fearActive_{false};
    double fearTimer_{0.0};
    double fearDuration_{10.0};

    // Ghost release system
    double levelStartTime_{0.0};
    std::vector<std::shared_ptr<Ghost>> ghostReleaseQueue_;
    std::vector<double> ghostReleaseDelays_{0.0, 0.0, 5.0, 10.0};
    std::size_t nextGhostToRelease_{0};

    std::weak_ptr<Wall> ghostGateWall_; // the wall that blocks the gate
    std::vector<GatePass> gatePass_;
};
} // namespace pacman::logic
