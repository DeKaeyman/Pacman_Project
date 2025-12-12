// logic/entities/Ghost.cpp
#include "Ghost.h"
#include "../observer/Event.h"
#include "../utils/Random.h"
#include "../world/World.h"
#include "PacMan.h"
#include "Wall.h"
#include <array>
#include <cmath>
#include <vector>

namespace {
// Helper function to get the opposite direction
pacman::logic::Direction oppositeOf(pacman::logic::Direction d) {
    using pacman::logic::Direction;
    switch (d) {
    case Direction::Left:
        return Direction::Right;
    case Direction::Right:
        return Direction::Left;
    case Direction::Up:
        return Direction::Down;
    case Direction::Down:
        return Direction::Up;
    default:
        return Direction::None;
    }
}
} // namespace

namespace pacman::logic {

Ghost::Ghost(const pacman::logic::Rect& startBounds, pacman::logic::GhostKind kind, double speed)
    : bounds_(startBounds), spawnBounds_(startBounds), direction_(Direction::None), speed_(speed), baseSpeed_(speed),
      kind_(kind), mode_(GhostMode::Chase) {
    solid = false; // Ghosts don't collide with walls (soft collision only)
    active = true; // Ghost starts alive and active
}

void Ghost::update(double dt) {
    if (!active)
        return; // Skip update if ghost is disabled/dead

    {
        Event tick{}; // Create tick event for observers
        tick.type = EventType::Tick;
        notify(tick); // Notify all observers (views can use this)
    }

    applyStrategy(dt); // Decide which direction to move based on AI

    if (direction_ == Direction::None)
        return; // No movement if no direction set

    float vx = dirToDx(direction_);               // Unit vector for x-axis movement
    float vy = dirToDy(direction_);               // Unit vector for y-axis movement
    float dist = static_cast<float>(speed_ * dt); // Distance to move this frame

    bounds_.x += vx * dist; // Apply horizontal movement
    bounds_.y += vy * dist; // Apply vertical movement

    MovedPayload payload{};                // Prepare event data for observers
    payload.pos = {bounds_.x, bounds_.y};  // New world position
    payload.size = {bounds_.w, bounds_.h}; // Ghost bounding box size

    Event moved{};                 // Create movement event
    moved.type = EventType::Moved; // Mark as movement event
    moved.payload = payload;       // Attach position data
    notify(moved);                 // Notify observers (GhostView updates sprite)
}

void Ghost::resetToSpawn() noexcept {
    bounds_ = spawnBounds_;       // Restore original spawn position
    direction_ = Direction::None; // Clear current movement direction
}

void Ghost::setDirection(Direction dir) noexcept {
    if (direction_ == dir)
        return;       // Ignore if already moving in that direction
    direction_ = dir; // Update current movement direction

    StateChangedPayload payload{}; // Prepare direction code for view layer
    switch (direction_) {
    case Direction::Right:
        payload.code = 0; // Right = 0
        break;
    case Direction::Left:
        payload.code = 1; // Left = 1
        break;
    case Direction::Up:
        payload.code = 2; // Up = 2
        break;
    case Direction::Down:
        payload.code = 3; // Down = 3
        break;
    default:
        return; // Invalid direction, don't notify
    }

    Event e{}; // Create state change event
    e.type = EventType::StateChanged;
    e.payload = payload;
    notify(e); // Trigger animation update in GhostView
}

bool Ghost::isMoveViable(Direction d, double dt) const {
    if (d == Direction::None || !world_)
        return false; // Can't move without direction or world reference

    Rect next = bounds_;                          // Copy current position
    float step = static_cast<float>(speed_ * dt); // Calculate movement distance
    next.x += dirToDx(d) * step;                  // Apply movement in x direction
    next.y += dirToDy(d) * step;                  // Apply movement in y direction

    const auto& entities = world_->entities(); // Get all world entities

    for (const auto& ent : entities) {
        if (!ent || !ent->active || !ent->solid)
            continue; // Skip inactive or non-solid entities
        if (ent.get() == this)
            continue; // Skip self-collision check

        auto* wall = dynamic_cast<Wall*>(ent.get());
        if (!wall)
            continue; // Only check collision with walls

        if (intersects(next, wall->bounds(), 0.00014f))
            return false; // Movement blocked by wall
    }

    return true; // Movement is viable
}

float Ghost::manhattanDistanceAfterMove(Direction d, double dt, float tx, float ty) const {
    Rect next = bounds_;                          // Copy current position
    float step = static_cast<float>(speed_ * dt); // Calculate movement distance
    next.x += dirToDx(d) * step;                  // Apply movement in x direction
    next.y += dirToDy(d) * step;                  // Apply movement in y direction

    const float gx = next.x + next.w / 2.0f; // Ghost center x after move
    const float gy = next.y + next.h / 2.0f; // Ghost center y after move

    return std::fabs(gx - tx) + std::fabs(gy - ty); // Manhattan distance to target
}

std::vector<Direction> Ghost::collectViableDirections(double dt) const {
    std::vector<Direction> result; // List of valid directions
    std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};

    for (Direction d : allDirs) {
        if (isMoveViable(d, dt)) // Check if direction doesn't hit walls
            result.push_back(d); // Add to viable list
    }
    return result;
}

bool Ghost::isIntersectionOrCorner(const std::vector<Direction>& viable) const {
    if (viable.size() <= 1)
        return false; // Dead end or no choice

    if (viable.size() >= 3)
        return true; // Intersection with 3+ directions

    // Check if two directions are NOT opposite (meaning it's a corner)
    Direction a = viable[0];
    Direction b = viable[1];

    const bool opposite = (a == Direction::Left && b == Direction::Right) ||
                          (a == Direction::Right && b == Direction::Left) ||
                          (a == Direction::Up && b == Direction::Down) || (a == Direction::Down && b == Direction::Up);

    return !opposite; // Corner if directions are perpendicular
}

Direction Ghost::randomDirectionFrom(const std::vector<Direction>& dirs) const {
    if (dirs.empty())
        return Direction::None; // No options available
    if (dirs.size() == 1)
        return dirs.front(); // Only one option, return it

    auto& rng = Random::getInstance();              // Get random number generator
    std::size_t idx = rng.choiceIndex(dirs.size()); // Pick random index
    return dirs[idx];                               // Return randomly chosen direction
}

void Ghost::applyStrategy(double dt) {
    if (!world_)
        return; // Can't run AI without world reference

    if (mode_ == GhostMode::Fear) {
        applyFearStrategy(dt); // Run away from Pac-Man
    } else {
        applyChaseStrategy(dt); // Chase Pac-Man using ghost-specific AI
    }
}

void Ghost::applyFearStrategy(double dt) {
    const auto& entities = world_->entities(); // Get all world entities

    // Find Pac-Man in the world
    Rect pacBounds{};
    bool foundPacman = false;

    for (const auto& e : entities) {
        if (!e || !e->active)
            continue; // Skip inactive entities

        auto pac = std::dynamic_pointer_cast<PacMan>(e);
        if (pac) {
            pacBounds = pac->bounds(); // Store Pac-Man's position
            foundPacman = true;
            break; // Found Pac-Man, stop searching
        }
    }

    if (!foundPacman)
        return; // Can't flee if Pac-Man not found

    float pacCenterX = pacBounds.x + pacBounds.w / 2.0f; // Pac-Man center x
    float pacCenterY = pacBounds.y + pacBounds.h / 2.0f; // Pac-Man center y

    std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};
    Direction current = direction_;           // Current movement direction
    Direction opposite = oppositeOf(current); // Opposite of current direction

    // Collect all viable movement directions
    std::vector<Direction> viable;
    viable.reserve(4);
    for (Direction d : allDirs) {
        if (isMoveViable(d, dt)) // Check if direction is not blocked
            viable.push_back(d); // Add to viable list
    }

    if (viable.empty()) {
        // fallback: allow at least opposite if it's viable (prevents freezing)
        Direction opp = oppositeOf(direction_);
        if (opp != Direction::None && isMoveViable(opp, dt)) {
            setDirection(opp);
        }
        return;
    }

    // Check if ghost is in a straight corridor
    bool isCorridor = false;
    if (viable.size() == 2) {
        Direction a = viable[0];
        Direction b = viable[1];
        isCorridor = (oppositeOf(a) == b); // Corridor = two opposite directions
    }

    auto contains = [&](Direction d) { return std::find(viable.begin(), viable.end(), d) != viable.end(); };

    // In corridor: keep current direction if viable (avoid jitter)
    if (isCorridor && current != Direction::None && contains(current)) {
        return; // Keep moving in current direction
    }

    // Build candidates: avoid 180° turn unless it's the only option (dead end)
    std::vector<Direction> candidates;
    candidates.reserve(viable.size());

    for (Direction d : viable) {
        if (d != opposite) // Exclude 180° turn
            candidates.push_back(d);
    }

    // If no options (dead end), allow 180° turn
    if (candidates.empty())
        candidates = viable; // Use all viable directions including 180°

    // Choose move that MAXIMIZES Manhattan distance from Pac-Man (flee)
    float bestScore = -1.0f;         // Track best (highest) distance
    std::vector<Direction> bestDirs; // Store all directions with best score
    const float eps = 1e-6f;         // Epsilon for float comparison

    for (Direction d : candidates) {
        float score = manhattanDistanceAfterMove(d, dt, pacCenterX, pacCenterY);

        if (d == current)
            score += 1e-4f; // Small bias to keep current direction (reduce jitter)

        if (score > bestScore + eps) {
            bestScore = score; // New best score found
            bestDirs.clear();
            bestDirs.push_back(d);
        } else if (std::fabs(score - bestScore) <= eps) {
            bestDirs.push_back(d); // Tie, add to list
        }
    }

    Direction chosen = randomDirectionFrom(bestDirs); // Pick random from best options
    if (chosen != Direction::None)
        setDirection(chosen); // Apply chosen direction
}

void Ghost::applyChaseStrategy(double dt) {
    const auto& entities = world_->entities(); // Get all world entities

    // Find Pac-Man in the world
    Rect pacBounds{};
    bool foundPacman = false;
    Direction pacDir = Direction::None;

    for (const auto& e : entities) {
        if (!e || !e->active)
            continue; // Skip inactive entities

        auto pac = std::dynamic_pointer_cast<PacMan>(e);
        if (pac) {
            pacBounds = pac->bounds(); // Store Pac-Man's position
            pacDir = pac->direction(); // Store Pac-Man's facing direction
            foundPacman = true;
            break; // Found Pac-Man, stop searching
        }
    }

    switch (kind_) {
    case GhostKind::A:
        applyChaseStrategyA(dt); // Random patrol behavior
        break;

    case GhostKind::B:
    case GhostKind::C: {
        if (!foundPacman) {
            applyChaseStrategyA(dt); // Fallback to random patrol
            return;
        }

        // Target: 2 tiles ahead of Pac-Man in his facing direction
        float pacCenterX = pacBounds.x + pacBounds.w / 2.0f; // Pac-Man center x
        float pacCenterY = pacBounds.y + pacBounds.h / 2.0f; // Pac-Man center y

        float aheadDistance = bounds_.w * 2.0f; // Distance ahead (2 tiles)
        float dx = dirToDx(pacDir);             // Direction unit vector x
        float dy = dirToDy(pacDir);             // Direction unit vector y

        if (pacDir == Direction::None) {
            dx = 0.0f; // Pac-Man not moving
            dy = 0.0f;
            aheadDistance = 0.0f; // Target current position instead
        }

        float targetX = pacCenterX + dx * aheadDistance; // Calculate target x
        float targetY = pacCenterY + dy * aheadDistance; // Calculate target y

        Direction chosen = chooseDirectionTowards(dt, targetX, targetY);
        if (chosen != Direction::None)
            setDirection(chosen); // Move towards predicted position
        break;
    }

    case GhostKind::D:
    default: {
        if (!foundPacman) {
            applyChaseStrategyA(dt); // Fallback to random patrol
            return;
        }

        // Target: Pac-Man's current position (direct chase)
        float pacCenterX = pacBounds.x + pacBounds.w / 2.0f; // Pac-Man center x
        float pacCenterY = pacBounds.y + pacBounds.h / 2.0f; // Pac-Man center y

        Direction chosen = chooseDirectionTowards(dt, pacCenterX, pacCenterY);
        if (chosen != Direction::None)
            setDirection(chosen); // Move towards Pac-Man
        break;
    }
    }
}

void Ghost::applyChaseStrategyA(double dt) {
    auto viable = collectViableDirections(dt); // Get all non-blocked directions
    if (viable.empty())
        return; // Nowhere to move, ghost is stuck

    Direction current = direction_; // Current movement direction

    auto contains = [&](Direction d) { return std::find(viable.begin(), viable.end(), d) != viable.end(); };

    bool currentViable = (current != Direction::None) && contains(current);
    bool isChoice = isIntersectionOrCorner(viable); // Check if at decision point

    // Lock direction until corner/intersection
    if (!isChoice && currentViable)
        return; // Keep moving in current direction

    // At corner/intersection: pick random viable direction with p=0.5
    auto& rng = Random::getInstance();             // Get random number generator
    bool shouldChange = (rng.choiceIndex(2) == 0); // 50% chance to change

    if (!shouldChange && currentViable)
        return; // Decided to keep current direction

    // Try to avoid 180° turns
    Direction opposite = oppositeOf(current); // Get opposite direction
    std::vector<Direction> candidates;
    for (Direction d : viable) {
        if (d != opposite) // Exclude 180° turn
            candidates.push_back(d);
    }

    // If no options (dead end), allow 180° turn
    if (candidates.empty())
        candidates = viable; // Use all viable directions including 180°

    Direction chosen = randomDirectionFrom(candidates); // Pick random direction
    if (chosen != Direction::None)
        setDirection(chosen); // Apply chosen direction
}

Direction Ghost::chooseDirectionTowards(double dt, float tx, float ty) const {
    std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};

    float bestScore = std::numeric_limits<float>::max(); // Track best (lowest) distance
    std::vector<Direction> bestDirs;                     // Store all directions with best score
    const float eps = 1e-6f;                             // Epsilon for float comparison

    Direction opposite = oppositeOf(direction_); // Get opposite of current direction

    // First pass: collect viable directions excluding 180° turn
    for (Direction d : allDirs) {
        if (!isMoveViable(d, dt))
            continue; // Skip blocked directions

        // Skip 180° turns in first pass
        if (d == opposite)
            continue;

        float score = manhattanDistanceAfterMove(d, dt, tx, ty); // Calculate distance
        if (score < bestScore - eps) {
            bestScore = score; // New best (shortest) distance found
            bestDirs.clear();
            bestDirs.push_back(d);
        } else if (std::fabs(score - bestScore) <= eps) {
            bestDirs.push_back(d); // Tie, add to list
        }
    }

    // If no viable moves found (dead end), allow 180° turn
    if (bestDirs.empty() && opposite != Direction::None && isMoveViable(opposite, dt)) {
        return opposite; // Only option is to turn back
    }

    if (bestDirs.empty())
        return Direction::None; // Truly stuck, no viable moves

    return randomDirectionFrom(bestDirs); // Pick random from best options
}

void Ghost::setMode(pacman::logic::GhostMode m) noexcept {
    if (mode_ == m)
        return; // Already in that mode, ignore

    mode_ = m; // Update behavioral mode

    if (mode_ == GhostMode::Fear) {
        // Reverse direction instantly when entering fear mode (classic Pac-Man)
        switch (direction_) {
        case Direction::Left:
            direction_ = Direction::Right;
            break;
        case Direction::Right:
            direction_ = Direction::Left;
            break;
        case Direction::Up:
            direction_ = Direction::Down;
            break;
        case Direction::Down:
            direction_ = Direction::Up;
            break;
        default:
            break;
        }

        speed_ = baseSpeed_ * 0.6; // Move slower while frightened
    } else {
        speed_ = baseSpeed_; // Restore normal speed
    }

    StateChangedPayload payload{};                         // Prepare mode change data for view
    payload.code = (mode_ == GhostMode::Fear) ? 100 : 101; // 100 = enter fear, 101 = exit fear

    Event e{}; // Create state change event
    e.type = EventType::StateChanged;
    e.payload = payload;

    notify(e); // Notify GhostView to change appearance (blue/normal)
}

} // namespace pacman::logic