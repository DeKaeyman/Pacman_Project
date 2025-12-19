#include "PacMan.h"

#include "../observer/Event.h"

namespace pacman::logic {

/**
 * @brief Constructs Pac-Man with initial bounds and movement speed.
 * @param startBounds Initial world-space bounds.
 * @param speed Movement speed in world units per second.
 */
PacMan::PacMan(const pacman::logic::Rect& startBounds, double speed)
    : bounds_(startBounds), direction_(Direction::None), desiredDirection_(Direction::Right), speed_(speed),
      baseSpeed_(speed), spawnBounds_(startBounds) {
    solid = true;
    active = true;
}

/**
 * @brief Advances Pac-Man movement and emits Tick/Moved events.
 * @param dt Time step in seconds.
 */
void PacMan::update(double dt) {
    if (!active) {
        return;
    }

    {
        Event tick{};
        tick.type = EventType::Tick;
        notify(tick);
    }

    if (direction_ == Direction::None) {
        return;
    }

    const float vx = dirToDx(direction_);
    const float vy = dirToDy(direction_);

    const float dist = static_cast<float>(speed_ * dt);

    bounds_.x += vx * dist;
    bounds_.y += vy * dist;

    MovedPayload payload{};
    payload.pos = {bounds_.x, bounds_.y};
    payload.size = {bounds_.w, bounds_.h};

    Event moved{};
    moved.type = EventType::Moved;
    moved.payload = payload;

    notify(moved);
}

/**
 * @brief Sets the desired (requested) direction.
 * @param dir Requested movement direction.
 */
void PacMan::setDesiredDirection(pacman::logic::Direction dir) noexcept {
    if (desiredDirection_ == dir) {
        return;
    }
    desiredDirection_ = dir;
}

/**
 * @brief Sets the current applied movement direction and emits a StateChanged event.
 * @param dir New applied direction.
 */
void PacMan::setDirection(Direction dir) noexcept {
    if (direction_ == dir) {
        return;
    }

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

    Event event{};
    event.type = EventType::StateChanged;
    event.payload = payload;

    notify(event);
}

/**
 * @brief Resets Pac-Man to its spawn bounds and clears movement state.
 */
void PacMan::resetToSpawn() noexcept {
    bounds_ = spawnBounds_;

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

/**
 * @brief Emits a Died event with the configured death score value.
 */
void PacMan::dieScore() {
    if (!active) {
        return;
    }

    CollectedPayload payload{deathValue_};
    Event event{};
    event.type = EventType::Died;
    event.payload = payload;

    notify(event);
}

} // namespace pacman::logic