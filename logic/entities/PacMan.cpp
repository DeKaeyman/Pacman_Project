// logic/entities/PacMan.cpp
#include "PacMan.h"
#include "../observer/Event.h"

namespace pacman::logic {

PacMan::PacMan(const pacman::logic::Rect& startBounds, double speed)
    : bounds_(startBounds), speed_(speed), spawnBounds_(startBounds) {
    solid = true;  // Pacman collides with walls/ghosts
    active = true; // Pacman is alive/visible
}

void PacMan::update(double dt) {
    if (!active)
        return; // Dead/Inactive pacman shouldn't move

    {
        Event tick{}; // Fire a “Tick” event each frame
        tick.type = EventType::Tick;
        notify(tick);
    }

    if (direction_ == Direction::None)
        return; // No active direction -> no movement

    // Unit direction vector in world space
    float vx = dirToDx(direction_);
    float vy = dirToDy(direction_);

    // Distance travelled this frame
    float dist = static_cast<float>(speed_ * dt);

    bounds_.x += vx * dist; // Advance Pacman horizontally
    bounds_.y += vy * dist; // Advance Pacman vertically

    MovedPayload payload{};
    payload.pos = {bounds_.x, bounds_.y};
    payload.size = {bounds_.w, bounds_.h};

    Event moved{};
    moved.type = EventType::Moved;
    moved.payload = payload;

    notify(moved); // Broadcast movement update
}

void PacMan::setDesiredDirection(pacman::logic::Direction dir) noexcept {
    if (desiredDirection_ == dir)
        return;              // Ignore if direction already active
    desiredDirection_ = dir; // Player requests new direction
}

void PacMan::setDirection(Direction dir) noexcept {
    if (direction_ == dir)
        return;       // Do nothing if direction already active
    direction_ = dir; // Apply new actual direction

    StateChangedPayload payload{}; // Payload encoding direction as numeric code
    switch (direction_) {          // Map direction enum -> integer code for view
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

    Event e{};                        // Create Observer event
    e.type = EventType::StateChanged; // Notify views of direction change
    e.payload = payload;

    notify(e); // Send to all attached observers
}

void PacMan::resetToSpawn() noexcept {
    bounds_ = spawnBounds_;

    // Clear movement intent + actual motion
    direction_ = Direction::None;
    desiredDirection_ = Direction::None;

    MovedPayload payload{};
    payload.pos = {bounds_.x, bounds_.y};
    payload.size = {bounds_.w, bounds_.h};

    Event moved{};
    moved.type = EventType::Moved;
    moved.payload = payload;
    notify(moved);
}

void PacMan::dieScore() {
    if (!active)
        return; // Skip if already collected

    CollectedPayload payload{deathValue_}; // Score value inside payload
    Event e{};                        // Event object
    e.type = EventType::Died;    // Event identifies “Ghost collected”
    e.payload = payload;              // Attach payload data

    notify(e); // Notify views
}
} // namespace pacman::logic