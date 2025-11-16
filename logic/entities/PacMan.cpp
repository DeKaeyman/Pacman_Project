// logic/entities/PacMan.cpp
#include "PacMan.h"

namespace pacman::logic {

    PacMan::PacMan(const pacman::logic::Rect &startBounds, double speed) : bounds_(startBounds), speed_(speed) {
        solid = true; // Pacman collides with walls/ghosts
        active = true; // Pacman is alive/visible
    }

    void PacMan::update(double dt) {
        if (!active) return;
        if (direction_ == Direction::None) return;

        // Unit direction vector in world space
        float vx = dirToDx(direction_);
        float vy = dirToDy(direction_);

        // Distance travelled this frame
        float dist = static_cast<float>(speed_ * dt);

        bounds_.x += vx * dist;
        bounds_.y += vy * dist;
    }
}