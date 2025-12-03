// logic/entities/Wall.h
#pragma once

#include "Entity.h"

namespace pacman::logic {

class Wall : public Entity {
public:
    explicit Wall(const Rect& bounds) : bounds_(bounds) {
        solid = true;  // Wals act as collision blocker
        active = true; // Walls are always active/visible
    }

    Rect bounds() const override { return bounds_; } // Return wall's world space bounds

    void update(double /*dt*/) override {} // Walls do not animate/move

    void setBounds(const Rect& r) noexcept { bounds_ = r; } // Allow resizing/repositioning

private:
    Rect bounds_{}; // Wall's AABB in world coordinates
};
} // namespace pacman::logic
