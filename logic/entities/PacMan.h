// logic/entities/PacMan.h
#pragma once

#include "Entity.h"
#include "observer/Subject.h"
#include "Direction.h"

namespace pacman::logic {

    class PacMan : public Entity, public Subject { // Logic only model for Pacman
    public:
        PacMan(const Rect& startBounds, double speed = 1.0f);

        Rect bounds() const override { return bounds_; } // Current bounding box of Pacman
        void update(double dt) override; // Continuous movement based on current direction and speed
        void setDirection(Direction dir) noexcept { direction_ = dir; } // Set the current movement direction
        Direction direction() const noexcept { return direction_; } // Get current movement direction
        void setSpeed(double s) noexcept { speed_ = s; } // Set movement speed
        double speed() const noexcept { return speed_; } // Get movement speed
        void setBounds(const Rect& r) noexcept { bounds_ = r; } // Directly set the bounding box

    private:
        Rect bounds_{}; // Pacman bounding box in world space
        Direction direction_{Direction::None};
        double speed_{0.0}; // Units per second in world coordinates
    };
}