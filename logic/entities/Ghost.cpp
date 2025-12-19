#include "Ghost.h"

#include "../observer/Event.h"
#include "../utils/Random.h"
#include "../world/World.h"

#include "PacMan.h"
#include "Wall.h"

#include <array>
#include <cmath>
#include <limits>
#include <vector>

namespace {

    /**
     * @brief Returns the opposite direction.
     * @param d Input direction.
     * @return Opposite direction, or None for None/unknown.
     */
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

    /**
     * @brief Constructs a ghost at the given bounds, with a kind and movement speed.
     * @param startBounds Initial world-space bounds.
     * @param kind Ghost kind.
     * @param speed Movement speed in world units per second.
     */
    Ghost::Ghost(const pacman::logic::Rect& startBounds, pacman::logic::GhostKind kind, double speed)
            : bounds_(startBounds),
              spawnBounds_(startBounds),
              direction_(Direction::None),
              speed_(speed),
              baseSpeed_(speed),
              kind_(kind),
              mode_(GhostMode::Chase) {
        solid = false;
        active = true;
    }

    /**
     * @brief Updates AI and applies continuous movement.
     * @param dt Time step in seconds.
     */
    void Ghost::update(double dt) {
        if (!active) {
            return;
        }

        {
            Event tick{};
            tick.type = EventType::Tick;
            notify(tick);
        }

        applyStrategy(dt);

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
     * @brief Resets the ghost to its spawn position and clears movement.
     */
    void Ghost::resetToSpawn() noexcept {
        setMode(GhostMode::Chase);

        StateChangedPayload payload1{};
        payload1.code = (mode_ == GhostMode::Fear) ? 100 : 101;

        Event e1{};
        e1.type = EventType::StateChanged;
        e1.payload = payload1;
        notify(e1);

        bounds_ = spawnBounds_;
        direction_ = Direction::None;

        MovedPayload payload2{};
        payload2.pos = {bounds_.x, bounds_.y};
        payload2.size = {bounds_.w, bounds_.h};

        Event e2{};
        e2.type = EventType::Moved;
        e2.payload = payload2;
        notify(e2);
    }

    /**
     * @brief Forces the current movement direction and emits a StateChanged event.
     * @param dir New movement direction.
     */
    void Ghost::setDirection(Direction dir) noexcept {
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

        Event e{};
        e.type = EventType::StateChanged;
        e.payload = payload;
        notify(e);
    }

    /**
     * @brief Checks whether moving in the given direction for dt would collide with walls.
     * @param d Candidate direction.
     * @param dt Time step in seconds.
     * @return True if movement is allowed.
     */
    bool Ghost::isMoveViable(Direction d, double dt) const {
        if (d == Direction::None || !world_) {
            return false;
        }

        Rect next = bounds_;
        const float step = static_cast<float>(speed_ * dt);

        next.x += dirToDx(d) * step;
        next.y += dirToDy(d) * step;

        const auto& entities = world_->entities();

        for (const auto& ent : entities) {
            if (!ent || !ent->active || !ent->solid) {
                continue;
            }
            if (ent.get() == this) {
                continue;
            }

            auto* wall = dynamic_cast<Wall*>(ent.get());
            if (!wall) {
                continue;
            }

            if (wall == world_->ghostGate() && world_->canGhostPassGate(this)) {
                continue;
            }

            if (intersects(next, wall->bounds(), 0.000128f)) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Computes Manhattan distance to a target after a hypothetical move.
     * @param d Candidate direction.
     * @param dt Time step in seconds.
     * @param tx Target X (world-space).
     * @param ty Target Y (world-space).
     * @return Manhattan distance from ghost center after move to target.
     */
    float Ghost::manhattanDistanceAfterMove(Direction d, double dt, float tx, float ty) const {
        Rect next = bounds_;
        const float step = static_cast<float>(speed_ * dt);

        next.x += dirToDx(d) * step;
        next.y += dirToDy(d) * step;

        const float gx = next.x + next.w / 2.0f;
        const float gy = next.y + next.h / 2.0f;

        return std::fabs(gx - tx) + std::fabs(gy - ty);
    }

    /**
     * @brief Collects all viable movement directions for the current state.
     * @param dt Time step in seconds.
     * @return Vector of viable directions.
     */
    std::vector<Direction> Ghost::collectViableDirections(double dt) const {
        std::vector<Direction> result;
        std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};

        for (Direction d : allDirs) {
            if (isMoveViable(d, dt)) {
                result.push_back(d);
            }
        }

        return result;
    }

    /**
     * @brief Checks whether the viable set forms an intersection or corner.
     * @param viable Directions that are currently viable.
     * @return True if there is a choice (corner or intersection).
     */
    bool Ghost::isIntersectionOrCorner(const std::vector<Direction>& viable) const {
        if (viable.size() <= 1) {
            return false;
        }

        if (viable.size() >= 3) {
            return true;
        }

        const Direction a = viable[0];
        const Direction b = viable[1];

        const bool opposite = (a == Direction::Left && b == Direction::Right) ||
                              (a == Direction::Right && b == Direction::Left) ||
                              (a == Direction::Up && b == Direction::Down) ||
                              (a == Direction::Down && b == Direction::Up);

        return !opposite;
    }

    /**
     * @brief Randomly selects a direction from a set.
     * @param dirs Candidate directions.
     * @return Selected direction or Direction::None if empty.
     */
    Direction Ghost::randomDirectionFrom(const std::vector<Direction>& dirs) const {
        if (dirs.empty()) {
            return Direction::None;
        }
        if (dirs.size() == 1) {
            return dirs.front();
        }

        auto& rng = Random::getInstance();
        const std::size_t idx = rng.choiceIndex(dirs.size());
        return dirs[idx];
    }

    /**
     * @brief Dispatches AI behavior based on gate logic and current mode.
     * @param dt Time step in seconds.
     */
    void Ghost::applyStrategy(double dt) {
        if (!world_) {
            return;
        }

        if (world_->canGhostPassGate(this)) {
            if (isMoveViable(Direction::Up, dt)) {
                setDirection(Direction::Up);
                return;
            }

            auto viable = collectViableDirections(dt);
            if (!viable.empty()) {
                const Direction opp = oppositeOf(direction_);
                for (Direction d : viable) {
                    if (d != opp) {
                        setDirection(d);
                        return;
                    }
                }
                setDirection(viable.front());
            }
            return;
        }

        if (mode_ == GhostMode::Fear) {
            applyFearStrategy(dt);
        } else {
            applyChaseStrategy(dt);
        }
    }

    /**
     * @brief Fear behavior: attempts to maximize distance from Pac-Man.
     * @param dt Time step in seconds.
     */
    void Ghost::applyFearStrategy(double dt) {
        const auto& entities = world_->entities();

        Rect pacBounds{};
        bool foundPacman = false;

        for (const auto& e : entities) {
            if (!e || !e->active) {
                continue;
            }

            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (pac) {
                pacBounds = pac->bounds();
                foundPacman = true;
                break;
            }
        }

        if (!foundPacman) {
            return;
        }

        const float pacCenterX = pacBounds.x + pacBounds.w / 2.0f;
        const float pacCenterY = pacBounds.y + pacBounds.h / 2.0f;

        std::array<Direction, 4> allDirs{Direction::Right, Direction::Left, Direction::Up, Direction::Down};
        const Direction current = direction_;
        const Direction opposite = oppositeOf(current);

        std::vector<Direction> viable;
        viable.reserve(4);
        for (Direction d : allDirs) {
            if (isMoveViable(d, dt)) {
                viable.push_back(d);
            }
        }

        if (viable.empty()) {
            const Direction opp = oppositeOf(direction_);
            if (opp != Direction::None && isMoveViable(opp, dt)) {
                setDirection(opp);
            }
            return;
        }

        bool isCorridor = false;
        if (viable.size() == 2) {
            const Direction a = viable[0];
            const Direction b = viable[1];
            isCorridor = (oppositeOf(a) == b);
        }

        auto contains = [&](Direction d) {
            return std::find(viable.begin(), viable.end(), d) != viable.end();
        };

        if (isCorridor && current != Direction::None && contains(current)) {
            return;
        }

        std::vector<Direction> candidates;
        candidates.reserve(viable.size());
        for (Direction d : viable) {
            if (d != opposite) {
                candidates.push_back(d);
            }
        }

        if (candidates.empty()) {
            candidates = viable;
        }

        float bestScore = -1.0f;
        std::vector<Direction> bestDirs;
        const float eps = 1e-6f;

        for (Direction d : candidates) {
            float score = manhattanDistanceAfterMove(d, dt, pacCenterX, pacCenterY);

            if (d == current) {
                score += 1e-4f;
            }

            if (score > bestScore + eps) {
                bestScore = score;
                bestDirs.clear();
                bestDirs.push_back(d);
            } else if (std::fabs(score - bestScore) <= eps) {
                bestDirs.push_back(d);
            }
        }

        const Direction chosen = randomDirectionFrom(bestDirs);
        if (chosen != Direction::None) {
            setDirection(chosen);
        }
    }

    /**
     * @brief Chase behavior: selects a strategy based on ghost kind.
     * @param dt Time step in seconds.
     */
    void Ghost::applyChaseStrategy(double dt) {
        const auto& entities = world_->entities();

        Rect pacBounds{};
        bool foundPacman = false;
        Direction pacDir = Direction::None;

        for (const auto& e : entities) {
            if (!e || !e->active) {
                continue;
            }

            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (pac) {
                pacBounds = pac->bounds();
                pacDir = pac->direction();
                foundPacman = true;
                break;
            }
        }

        switch (kind_) {
            case GhostKind::A:
                applyChaseStrategyA(dt);
                break;

            case GhostKind::B:
            case GhostKind::C: {
                if (!foundPacman) {
                    applyChaseStrategyA(dt);
                    return;
                }

                const float pacCenterX = pacBounds.x + pacBounds.w / 2.0f;
                const float pacCenterY = pacBounds.y + pacBounds.h / 2.0f;

                float aheadDistance = bounds_.w * 2.0f;
                float dx = dirToDx(pacDir);
                float dy = dirToDy(pacDir);

                if (pacDir == Direction::None) {
                    dx = 0.0f;
                    dy = 0.0f;
                    aheadDistance = 0.0f;
                }

                const float targetX = pacCenterX + dx * aheadDistance;
                const float targetY = pacCenterY + dy * aheadDistance;

                const Direction chosen = chooseDirectionTowards(dt, targetX, targetY);
                if (chosen != Direction::None) {
                    setDirection(chosen);
                }
                break;
            }

            case GhostKind::D:
            default: {
                if (!foundPacman) {
                    applyChaseStrategyA(dt);
                    return;
                }

                const float pacCenterX = pacBounds.x + pacBounds.w / 2.0f;
                const float pacCenterY = pacBounds.y + pacBounds.h / 2.0f;

                const Direction chosen = chooseDirectionTowards(dt, pacCenterX, pacCenterY);
                if (chosen != Direction::None) {
                    setDirection(chosen);
                }
                break;
            }
        }
    }

    /**
     * @brief Chase behavior for GhostKind::A (random-ish patrol).
     * @param dt Time step in seconds.
     */
    void Ghost::applyChaseStrategyA(double dt) {
        auto viable = collectViableDirections(dt);
        if (viable.empty()) {
            return;
        }

        const Direction current = direction_;
        const Direction opposite = oppositeOf(current);

        auto contains = [&](Direction d) {
            return std::find(viable.begin(), viable.end(), d) != viable.end();
        };

        const bool isCorridor = (viable.size() == 2 && oppositeOf(viable[0]) == viable[1]);

        if (isCorridor && current != Direction::None && contains(current)) {
            return;
        }

        const bool isChoice = isIntersectionOrCorner(viable);
        const bool currentViable = (current != Direction::None && contains(current));

        if (!isChoice && currentViable) {
            return;
        }

        auto& rng = Random::getInstance();
        const bool shouldChange = (rng.choiceIndex(2) == 0);

        if (!shouldChange && currentViable) {
            return;
        }

        std::vector<Direction> candidates;
        for (Direction d : viable) {
            if (d != opposite) {
                candidates.push_back(d);
            }
        }

        if (candidates.empty()) {
            candidates = viable;
        }

        const Direction chosen = randomDirectionFrom(candidates);
        if (chosen != Direction::None) {
            setDirection(chosen);
        }
    }

    /**
     * @brief Chooses a direction that tends towards a target point.
     * @param dt Time step in seconds.
     * @param tx Target X (world-space).
     * @param ty Target Y (world-space).
     * @return Chosen direction or Direction::None if no viable move exists.
     */
    Direction Ghost::chooseDirectionTowards(double dt, float tx, float ty) const {
        auto viable = collectViableDirections(dt);
        if (viable.empty()) {
            return Direction::None;
        }

        const Direction current = direction_;
        const Direction opposite = oppositeOf(current);

        auto contains = [&](Direction d) {
            return std::find(viable.begin(), viable.end(), d) != viable.end();
        };

        const bool isCorridor = (viable.size() == 2 && oppositeOf(viable[0]) == viable[1]);

        if (isCorridor && current != Direction::None && contains(current)) {
            return current;
        }

        std::vector<Direction> candidates;
        for (Direction d : viable) {
            if (d != opposite) {
                candidates.push_back(d);
            }
        }

        if (candidates.empty()) {
            candidates = viable;
        }

        float best = std::numeric_limits<float>::max();
        std::vector<Direction> bestDirs;
        const float eps = 1e-6f;

        for (Direction d : candidates) {
            float score = manhattanDistanceAfterMove(d, dt, tx, ty);

            if (d == current) {
                score -= 1e-4f;
            }

            if (score < best - eps) {
                best = score;
                bestDirs.clear();
                bestDirs.push_back(d);
            } else if (std::fabs(score - best) <= eps) {
                bestDirs.push_back(d);
            }
        }

        return bestDirs.empty() ? Direction::None : randomDirectionFrom(bestDirs);
    }

    /**
     * @brief Sets the current mode and notifies observers of fear toggles.
     * @param m New mode.
     */
    void Ghost::setMode(pacman::logic::GhostMode m) noexcept {
        if (mode_ == m) {
            return;
        }

        mode_ = m;

        if (mode_ == GhostMode::Fear) {
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

            speed_ = baseSpeed_ * 0.6;
        } else {
            speed_ = baseSpeed_;
        }

        StateChangedPayload payload{};
        payload.code = (mode_ == GhostMode::Fear) ? 100 : 101;

        Event e{};
        e.type = EventType::StateChanged;
        e.payload = payload;

        notify(e);
    }

    /**
     * @brief Emits a Collected event with the ghost score value.
     */
    void Ghost::collectScore() {
        if (!active) {
            return;
        }

        CollectedPayload payload{value_};
        Event e{};
        e.type = EventType::Collected;
        e.payload = payload;

        notify(e);
    }

} // namespace pacman::logic