// logic/entities/Ghost.cpp
#include "Ghost.h"
#include "../observer/Event.h"

namespace pacman::logic {

    Ghost::Ghost(const pacman::logic::Rect &startBounds, pacman::logic::GhostKind kind, double speed) : bounds_(startBounds), spawnBounds_(startBounds), direction_(Direction::None), speed_(speed), kind_(kind) {
        solid = true; // Ghost collide with pacman/walls
        active = true; // Ghost starts alive/enabled
    }

    void Ghost::update(double dt) {
        if (!active) return; // Do nothing when ghost is disabled

        applyStrategy(dt); // Choose direction depending on ghost AI type

        if (direction_ == Direction::None) return; // If strategy produced no movement, stop

        float vx = dirToDx(direction_); // Unit vector for ghost's direction x
        float vy = dirToDy(direction_); // Unit vector y
        float dist = static_cast<float>(speed_ * dt); // Distance ghost moves this frame

        bounds_.x += vx * dist; // Apply movement horizontally
        bounds_.y += vy * dist; // Apply movement horizontally

        MovedPayload payload{}; // Prepare event payload describing movement
        payload.pos = {bounds_.x, bounds_.y}; // New ghost world position
        payload.size = {bounds_.w, bounds_.h}; // Ghost bounding box size

        Event e{}; // Create movement event for observers
        e.type = EventType::Moved; // Mark it as a "moved" event
        e.payload = payload; // Attach movement data
    }

    void Ghost::resetToSpawn() noexcept {
        bounds_ = spawnBounds_; // Restore original spawn point
        direction_ = Direction::None; // Reset movement state
    }

    void Ghost::applyStrategy(double /*dt*/) {
        switch (kind_) {
            case GhostKind::A:
                break;

            case GhostKind::B:
            case GhostKind::C:
                break;

            case GhostKind::D:
                break;
        }
    }
}