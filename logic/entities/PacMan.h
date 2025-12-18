// logic/entities/PacMan.h
#pragma once

#include "Direction.h"
#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

class PacMan : public Entity, public Subject { // Logic only model for Pacman
public:
    PacMan(const Rect& startBounds, double speed = 0.4f);

    Rect bounds() const override { return bounds_; } // Current bounding box of Pacman
    void update(double dt) override;                 // Continuous movement based on current direction and speed
    void setDirection(Direction dir) noexcept;       // Set the current movement direction
    Direction direction() const noexcept { return direction_; } // Get current movement direction
    void setDesiredDirection(Direction dir) noexcept;
    Direction desiredDirection() const noexcept { return desiredDirection_; }
    void setSpeed(double s) noexcept { speed_ = s; } // Set movement speed
    double speed() const noexcept { return speed_; } // Get movement speed
    double baseSpeed() const noexcept { return baseSpeed_; }
    void setBounds(const Rect& r) noexcept { bounds_ = r; } // Directly set the bounding box
    void setStartBounds(const Rect& r) noexcept { spawnBounds_ = r; }
    void resetToSpawn() noexcept;
    void dieScore();

private:
    Rect bounds_{}; // Pacman bounding box in world space
    Direction direction_{Direction::None};
    Direction desiredDirection_{Direction::Right};
    double speed_{0.0}; // Units per second in world coordinates
    double baseSpeed_{0.0};
    Rect spawnBounds_{};

    int deathValue_{-500};
};
} // namespace pacman::logic