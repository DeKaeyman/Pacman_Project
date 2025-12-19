#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "../entities/Direction.h"
#include "../entities/Entity.h"
#include "../factory/AbstractFactory.h"
#include "TileMap.h"

namespace pacman::logic {

class Ghost;
class PacMan;
class Wall;

/**
 * @brief Tracks a ghost that is temporarily allowed to pass the ghost gate.
 */
struct GatePass {
    std::shared_ptr<Ghost> ghost;
    bool touchedGate{false};
};

/**
 * @brief Small AABB overlap test in world coordinates.
 * @param a First rectangle.
 * @param b Second rectangle.
 * @param eps Optional epsilon to shrink comparisons (reduces jitter).
 * @return True if rectangles overlap.
 */
inline bool intersects(const Rect& a, const Rect& b, float eps = 0.0f) {
    const bool xOverlap = (a.x < b.x + b.w - eps) && (b.x < a.x + a.w - eps);
    const bool yOverlap = (a.y < b.y + b.h - eps) && (b.y < a.y + a.h - eps);
    return xOverlap && yOverlap;
}

/**
 * @brief Computes overlap area between two rectangles.
 * @param a First rectangle.
 * @param b Second rectangle.
 * @return Overlap area, or 0 if no overlap.
 */
inline float overlapArea(const Rect& a, const Rect& b) {
    const float x1 = std::max(a.x, b.x);
    const float y1 = std::max(a.y, b.y);
    const float x2 = std::min(a.x + a.w, b.x + b.w);
    const float y2 = std::min(a.y + a.h, b.y + b.h);

    const float w = x2 - x1;
    const float h = y2 - y1;

    if (w <= 0.0f || h <= 0.0f) {
        return 0.0f;
    }
    return w * h;
}

/**
 * @brief Computes overlap ratio relative to the smaller rectangle's area.
 * @param a First rectangle.
 * @param b Second rectangle.
 * @return Ratio in [0, 1], where 0.4 means "40% of the smaller rect overlaps".
 */
inline float overlapRatio(const Rect& a, const Rect& b) {
    const float areaOverlap = overlapArea(a, b);
    if (areaOverlap <= 0.0f) {
        return 0.0f;
    }

    const float areaA = a.w * a.h;
    const float areaB = b.w * b.h;
    if (areaA <= 0.0f || areaB <= 0.0f) {
        return 0.0f;
    }

    const float minArea = std::min(areaA, areaB);
    return areaOverlap / minArea;
}

/**
 * @brief Core game world simulation: owns entities, updates logic, and resolves interactions.
 *
 * Responsibilities:
 * - entity lifetime and unique IDs
 * - updating entities each frame
 * - solid collision detection + resolution
 * - soft overlap detection + resolution (coins/fruits/ghost hits)
 * - level loading and progression
 * - fear mode management
 * - ghost gate release system
 */
class World {
public:
    using EntityPtr = std::shared_ptr<Entity>;
    using EntityId = Entity::Id;

    /**
     * @brief Constructs a world with a factory used to create entities.
     * @param factory Abstract factory instance.
     */
    explicit World(AbstractFactory& factory) : factory_{&factory} {}

    /**
     * @brief Registers an entity and assigns it a unique ID.
     * @param e Entity pointer to add.
     * @return Assigned entity ID (0 if e is null).
     */
    EntityId addEntity(EntityPtr e);

    /**
     * @brief Removes an entity by ID.
     * @param id Entity ID to remove.
     * @return True if an entity was removed.
     */
    bool removeEntity(EntityId id);

    /**
     * @brief Returns a mutable entity pointer by ID.
     * @param id Entity ID.
     * @return Pointer to entity or nullptr if not found.
     */
    Entity* get(EntityId id);

    /**
     * @brief Returns a read-only entity pointer by ID.
     * @param id Entity ID.
     * @return Pointer to entity or nullptr if not found.
     */
    const Entity* get(EntityId id) const;

    /**
     * @brief Updates world simulation: entities, collisions, overlaps, timers, and releases.
     * @param dt Time step in seconds.
     */
    void update(double dt);

    /**
     * @brief Ticks entities with dt=0.0 to allow animations without moving simulation forward.
     */
    void tickAnimationsOnly();

    /**
     * @brief Applies a functor to each active entity.
     * @tparam F Callable type.
     * @param f Callable invoked as f(Entity&).
     */
    template <typename F>
    void forEachEntity(F&& f) {
        for (auto& p : entities_) {
            if (p && p->active) {
                f(*p);
            }
        }
    }

    /**
     * @brief Returns collision pairs detected during the last update.
     * @return Vector of pairs (EntityId, EntityId).
     */
    const std::vector<std::pair<EntityId, EntityId>>& lastCollisions() const { return lastCollisions_; }

    /**
     * @brief Resets the level state (clears entities and collision state).
     */
    void resetLevel();

    /**
     * @brief Advances to the next level, reloads the current map, and applies speed scaling.
     */
    void advanceLevel();

    /**
     * @brief Returns the current level index (starting at 1).
     * @return Current level.
     */
    int currentLevel() const { return currentLevel_; }

    /**
     * @brief Returns remaining lives.
     * @return Number of lives.
     */
    int lives() const noexcept { return lives_; }

    /**
     * @brief Resets lives to 3.
     */
    void resetLives() noexcept { lives_ = 3; }

    /**
     * @brief Returns whether the game is over based on lives.
     * @return True if lives are depleted.
     */
    bool isGameOver() const noexcept { return lives_ <= 0; }

    /**
     * @brief Stores the current entity setup as a template for resets.
     */
    void snapshotLevelTemplate();

    /**
     * @brief Returns the list of entities (read-only).
     * @return Entity container.
     */
    const std::vector<EntityPtr>& entities() const { return entities_; }

    /**
     * @brief Loads a new level layout into the world.
     * @param map Tile map describing the level.
     */
    void loadLevel(const TileMap& map);

    /**
     * @brief Returns the current tile map.
     * @return Tile map stored in the world.
     */
    const TileMap& tileMap() const noexcept { return tileMap_; }

    /**
     * @brief Buffers a direction request for Pac-Man.
     * @param dir Desired direction.
     */
    void setPacManDirection(Direction dir);

    /**
     * @brief Returns the ghost gate wall if available.
     * @return Pointer to gate wall, or nullptr if not present.
     */
    const Wall* ghostGate() const noexcept;

    /**
     * @brief Returns whether the given ghost is currently allowed to pass the gate.
     * @param g Ghost pointer.
     * @return True if the ghost is authorized to pass.
     */
    bool canGhostPassGate(const Ghost* g) const noexcept;

    /**
     * @brief Returns whether the level is cleared (no active coins or fruits).
     * @return True if cleared.
     */
    bool isLevelCleared() const noexcept;

    /**
     * @brief Starts a delay timer (used at level start / after deaths).
     * @param seconds Duration in seconds.
     */
    void startDelay(double seconds = 1.0) noexcept {
        startDelayDuration_ = seconds;
        startDelayTimer_ = seconds;
    }

private:
    /**
     * @brief Checks whether Pac-Man can turn into its desired direction this frame.
     * @param pac Pac-Man instance.
     * @param dt Time step in seconds.
     * @return True if the turn was applied.
     */
    bool checkPacmanDesiredDirection(PacMan& pac, double dt);

    /**
     * @brief Evaluates buffered turning requests before movement each frame.
     * @param dt Time step in seconds.
     */
    void handlePacManTurning(double dt);

    /**
     * @brief Calls update(dt) on all active entities.
     * @param dt Time step in seconds.
     */
    void updateEntities(double dt);

    /**
     * @brief Detects solid vs solid collisions into lastCollisions_.
     */
    void updateCollisions();

    /**
     * @brief Resolves solid collisions by correcting positions.
     */
    void resolveCollisions();

    /**
     * @brief Detects soft overlaps into lastOverlaps_.
     * @param minOverlapRatio Minimum overlap ratio for overlap to count.
     */
    void updateOverlaps(float minOverlapRatio = 0.85f);

    /**
     * @brief Resolves overlaps: collects items, triggers fear mode, handles Pac-Man vs ghost.
     */
    void resolveOverlaps();

    /**
     * @brief Enables fear mode for all ghosts and resets the fear timer.
     */
    void startFearMode();

    /**
     * @brief Disables fear mode for all ghosts immediately.
     */
    void stopFearMode();

    /**
     * @brief Updates the fear timer and exits fear mode at timeout.
     * @param dt Time step in seconds.
     */
    void updateFearTimer(double dt);

    /**
     * @brief Initializes ghost release timers for a fresh level start.
     */
    void startGhostReleaseClocks();

    /**
     * @brief Updates ghost release queue and gate pass bookkeeping.
     */
    void updateGhostRelease();

    /**
     * @brief Respawns an eaten ghost and re-queues it for gate passing.
     * @param ghost Ghost instance.
     */
    void respawnEatenGhost(Ghost& ghost);

    /**
     * @brief Handles logic after Pac-Man is hit (lives, respawns, fear reset, delay).
     * @param pac Pac-Man instance.
     */
    void resetActorsAfterPacmanHit(PacMan& pac);

    /**
     * @brief Applies a level-based speed multiplier to Pac-Man and ghosts.
     */
    void applyLevelSpeedBoost();

private:
    AbstractFactory* factory_{nullptr};

    std::vector<EntityPtr> entities_;
    std::vector<std::pair<EntityId, EntityId>> lastCollisions_;
    std::vector<std::pair<EntityId, EntityId>> lastOverlaps_;

    std::vector<EntityPtr> levelTemplate_;

    int currentLevel_{1};
    EntityId nextId_{1};

    TileMap tileMap_{};

    int lives_{3};

    bool fearActive_{false};
    double fearTimer_{0.0};
    double fearDuration_{10.0};

    double levelStartTime_{0.0};
    std::vector<std::shared_ptr<Ghost>> ghostReleaseQueue_;
    std::vector<double> ghostReleaseDelays_{0.0, 0.0, 5.0, 10.0};
    std::size_t nextGhostToRelease_{0};

    std::weak_ptr<Wall> ghostGateWall_;
    std::vector<GatePass> gatePass_;

    double startDelayTimer_{0.0};
    double startDelayDuration_{1.0};
};

} // namespace pacman::logic