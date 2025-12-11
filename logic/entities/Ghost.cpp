// logic/entities/Ghost.cpp
#include "Ghost.h"
#include "../observer/Event.h"
#include "../world/World.h"
#include "PacMan.h"
#include "Wall.h"
#include "../utils/Random.h"
#include <array>
#include <vector>
#include <cmath>

namespace pacman::logic {

Ghost::Ghost(const pacman::logic::Rect& startBounds, pacman::logic::GhostKind kind, double speed)
    : bounds_(startBounds), spawnBounds_(startBounds), direction_(Direction::None), speed_(speed), baseSpeed_(speed), kind_(kind), mode_(GhostMode::Chase) {
    solid = false; // Ghost collide with pacman/walls
    active = true; // Ghost starts alive/enabled
}

void Ghost::update(double dt) {
    if (!active)
        return; // Do nothing when ghost is disabled

    {
        Event tick{};
        tick.type = EventType::Tick;
        notify(tick);
    }

    applyStrategy(dt); // Choose direction depending on ghost AI type

    if (direction_ == Direction::None)
        return; // If strategy produced no movement, stop

    float vx = dirToDx(direction_);               // Unit vector for ghost's direction x
    float vy = dirToDy(direction_);               // Unit vector y
    float dist = static_cast<float>(speed_ * dt); // Distance ghost moves this frame

    bounds_.x += vx * dist; // Apply movement horizontally
    bounds_.y += vy * dist; // Apply movement horizontally

    MovedPayload payload{};                // Prepare event payload describing movement
    payload.pos = {bounds_.x, bounds_.y};  // New ghost world position
    payload.size = {bounds_.w, bounds_.h}; // Ghost bounding box size

    Event moved{};                 // Create movement event for observers
    moved.type = EventType::Moved; // Mark it as a "moved" event
    moved.payload = payload;       // Attach movement data
    notify(moved);
}

void Ghost::resetToSpawn() noexcept {
    bounds_ = spawnBounds_;       // Restore original spawn point
    direction_ = Direction::None; // Reset movement state
}

void Ghost::setDirection(Direction dir) noexcept {
    if (direction_ == dir) return; // Ignore redundant direction changes
    direction_ = dir; // Ignore redundant direction changes

    StateChangedPayload payload{}; // Encode direction as an integer for the view layer
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

    Event e{}; // Notify observers that direction changed
    e.type = EventType::StateChanged;
    e.payload = payload;
    notify(e); // Trigger animation updates in GhostView
}

void Ghost::applyStrategy(double dt) {
    if (mode_ != GhostMode::Fear) return; // Only perform this AI behavior when ghost is in FEAR mode
    if (!world_) return; // AI cannot operate without access to the world to inspect other entities

    const auto& entities = world_->entities(); // Local reference to all world entities for convenience

    Rect pacBounds{};
    bool foundPacman = false;

    // Locate pacman in the world
    for (const auto& e : entities) {
        if (!e || !e->active) continue; // Skip inactive/null entities

        auto pac = std::dynamic_pointer_cast<PacMan>(e);
        if (pac) {
            pacBounds = pac->bounds(); // Save pacmans current bounding box
            foundPacman = true;
            break; // only one pacman exists
        }
    }

    if (!foundPacman) return; // If the pacman is missing, ghost does nothing this frame

    // Compute Pac-Man’s center position for Manhattan distance scoring
    const float pacCenterX = pacBounds.x + pacBounds.w / 2.0f;
    const float pacCenterY = pacBounds.y + pacBounds.h / 2.0f;

    // Lambda: check if moving in direction d is allowed
    auto isViable = [&](Direction d) {
        if (d == Direction::None) return false;

        // Predict a small movement step in direction d
        Rect next = bounds_;
        float step = static_cast<float>(speed_ * dt);
        next.x += dirToDx(d) * step;
        next.y += dirToDy(d) * step;

        // Test predicted position against walls
        for (const auto& ent : entities) {
            if (!ent || !ent->active || !ent->solid) continue;
            if (ent.get() == this) continue;

            auto* wall = dynamic_cast<Wall*>(ent.get());

            if (!wall) continue;
            if (intersects(next, wall->bounds(), 0.0003f)) return false;
        }

        return true; // Move is allowed
    };

    // Lambda: compute Manhattan distance from Pac-Man if ghost moved in direction d
    auto manhattanIfMove = [&](Direction d) {
        Rect next = bounds_;
        float step = static_cast<float>(speed_ * dt);
        next.x += dirToDx(d) * step;
        next.y += dirToDy(d) * step;

        const float gx = next.x + next.w / 2.0f;
        const float gy = next.y + next.h / 2.0f;

        // Manhattan distance = horizontal + vertical separation
        return std::fabs(gx - pacCenterX) + std::fabs(gy - pacCenterY);
    };

    // All directions ghost can consider
    std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};

    float bestScore = -1.0f; // Highest Manhattan distance found
    std::vector<Direction> bestDirs; // All directions tied for best score

    // Evaluate all viable moves
    for (Direction d : allDirs) {
        if (!isViable(d)) continue; // Skip illegal moves

        float score = manhattanIfMove(d); // Distance after moving in direction d
        if (score > bestScore + 1e-6f) {
            bestScore = score; // New best distance
            bestDirs.clear();
            bestDirs.push_back(d);
        } else if (std::fabs(score - bestScore) <= 1e-6f) {
            bestDirs.push_back(d);
        }
    }

    if (bestDirs.empty()) return; // No possible direction → stay in current direction

    Direction chosen = bestDirs.front();
    if (bestDirs.size() > 1) {
        auto& rng = Random::getInstance();
        std::size_t idx = rng.choiceIndex(bestDirs.size());
        chosen = bestDirs[idx]; // Pick randomly from tied options
    }

    setDirection(chosen); // Apply final chosen direction
}

void Ghost::setMode(pacman::logic::GhostMode m) noexcept {
    if (mode_ == m) return; // Ignore if already in that mode

    mode_ = m; // Apply new behavioral mode

    if (mode_ == GhostMode::Fear) {
        // Reverse current direction instantly (classic Pac-Man behavior)
        switch (direction_) {
            case Direction::Left:  direction_ = Direction::Right; break;
            case Direction::Right: direction_ = Direction::Left;  break;
            case Direction::Up:    direction_ = Direction::Down;  break;
            case Direction::Down:  direction_ = Direction::Up;    break;
            default: break;
        }

        speed_ = baseSpeed_ * 0.6; // Move slower while afraid
    } else {
        speed_ = baseSpeed_; // Restore default speed
    }

    StateChangedPayload payload{}; // Tell GhostView to switch appearance
    payload.code = (mode_ == GhostMode::Fear) ? 100 : 101; // 100 → enter fear mode, 101 → exit fear mode

    Event e{};
    e.type = EventType::StateChanged;
    e.payload = payload;

    notify(e);
}

} // namespace pacman::logic