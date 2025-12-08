// logic/entities/Ghost.h
#pragma once

#include "Direction.h"
#include "Entity.h"
#include "factory/AbstractFactory.h"
#include "observer/Subject.h"

namespace pacman::logic {

enum class GhostMode { Chase, Fear };

class Ghost : public Entity,
              public Subject { // Ghost is both an Entity and event publisher
public:
    Ghost(const Rect& startBounds, GhostKind kind,
          double speed = 0.3); // Create a ghost, stores spawn position, movement
                               // speed, and ghost type

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

private:
    void applyStrategy(double dt); // Placeholder for AI logic decisions

    Rect bounds_{};                        // Current position and size in world space
    Rect spawnBounds_{};                   // Original spawn position
    Direction direction_{Direction::None}; // Active direction ghost is moving in
    double speed_{0.0};                    // Movement speed per second
    GhostKind kind_{GhostKind::A};         // Ghost type used for selecting AI behavior
    GhostMode mode_{GhostMode::Chase};     // Ghost mode used for selecting AI behavior
    double baseSpeed_{0.0};
};
} // namespace pacman::logic