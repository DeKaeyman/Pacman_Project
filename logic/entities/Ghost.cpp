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

        {
            Event tick{};
            tick.type = EventType::Tick;
            notify(tick);
        }

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

        Event moved{}; // Create movement event for observers
        moved.type = EventType::Moved; // Mark it as a "moved" event
        moved.payload = payload; // Attach movement data
        notify(moved);
    }

    void Ghost::resetToSpawn() noexcept {
        bounds_ = spawnBounds_; // Restore original spawn point
        direction_ = Direction::None; // Reset movement state
    }

    void Ghost::setDirection(Direction dir) noexcept {
        if (direction_ == dir) return;
        direction_ = dir;

        StateChangedPayload payload{};
        switch (direction_) {
            case Direction::Right:
                payload.code = 0;
                break;
            case Direction::Left:
                payload.code = 1;
                break;
            case Direction::Up:
                payload.code = 2;
                break;
            case Direction::Down:
                payload.code = 3;
                break;
            default:
                return;
        }

        Event e{};
        e.type    = EventType::StateChanged;
        e.payload = payload;
        notify(e);
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