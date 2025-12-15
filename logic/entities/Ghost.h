// logic/entities/Ghost.h
#pragma once

#include "Direction.h"
#include "Entity.h"
#include "factory/AbstractFactory.h"
#include "observer/Subject.h"

namespace pacman::logic {

class World;

enum class GhostMode { Chase, Fear };

class Ghost : public Entity,
              public Subject { // Ghost is both an Entity and event publisher
public:
    Ghost(const Rect& startBounds, GhostKind kind,
          double speed = 0.3); // Create a ghost, stores spawn position, movement
                               // speed, and ghost type

    void setWorld(World* world) noexcept { world_ = world; }

    Rect bounds() const override { return bounds_; } // Current world space bounding box
    void update(double dt) override;                 // Apply AI strategy + movement

    void setDirection(Direction dir) noexcept;                  // Force direction change
    Direction direction() const noexcept { return direction_; } // Get current direction

    void setSpeed(double s) noexcept { speed_ = s; } // Modify movement speed
    double speed() const noexcept { return speed_; } // Read movement speed

    GhostKind kind() const noexcept { return kind_; } // Which ghost kind

    void setBounds(const Rect& r) noexcept { bounds_ = r; } // Reposition/resize ghost

    const Rect& spawnBounds() const noexcept { return spawnBounds_; } // Original spawn location
    void resetToSpawn() noexcept;                                     // Reset ghost after being eaten

    GhostMode mode() const noexcept { return mode_; }
    bool isInFearMode() const noexcept { return mode_ == GhostMode::Fear; } // Check if the ghost is in fear mode
    void setMode(GhostMode m) noexcept;

    void setStartBounds(const Rect& r) noexcept { spawnBounds_ = r; }

private:
    // High-level AI dispatch
    void applyStrategy(double dt);

    // Behaviour per mode
    void applyFearStrategy(double dt);
    void applyChaseStrategy(double dt);

    // Chase sub-behaviour per ghost type
    void applyChaseStrategyA(double dt); // Ghost A: random lock + corners/intersections
    Direction chooseDirectionTowards(double dt, float tx, float ty) const; // Ghost B/C/D: towards target

    // Shared helpers
    bool isMoveViable(Direction d, double dt) const;
    float manhattanDistanceAfterMove(Direction d, double dt, float tx, float ty) const;
    std::vector<Direction> collectViableDirections(double dt) const;
    bool isIntersectionOrCorner(const std::vector<Direction>& viable) const;
    Direction randomDirectionFrom(const std::vector<Direction>& dirs) const;

    Rect bounds_{};                        // Current position and size in world space
    Rect spawnBounds_{};                   // Original spawn position
    Direction direction_{Direction::None}; // Active direction ghost is moving in
    double speed_{0.0};                    // Movement speed per second
    GhostKind kind_{GhostKind::A};         // Ghost type used for selecting AI behavior
    GhostMode mode_{GhostMode::Chase};     // Ghost mode used for selecting AI behavior
    double baseSpeed_{0.0};

    World* world_{nullptr}; // World context for AI queries
};
} // namespace pacman::logic