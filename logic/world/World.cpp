#include "World.h"

#include "../entities/Coin.h"
#include "../entities/Fruit.h"
#include "../entities/Ghost.h"
#include "../entities/PacMan.h"
#include "../entities/Wall.h"

#include "../utils/Stopwatch.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <vector>

namespace pacman::logic {

    /**
     * @brief Registers an entity and assigns it a unique ID.
     * @param e Entity pointer to add.
     * @return Assigned entity ID (0 if e is null).
     */
    World::EntityId World::addEntity(EntityPtr e) {
        if (!e) {
            return 0;
        }

        e->setId(nextId_++);

        if (auto ghost = std::dynamic_pointer_cast<Ghost>(e)) {
            ghost->setWorld(this);
        }

        const EntityId id = e->id();
        entities_.push_back(std::move(e));
        return id;
    }

    /**
     * @brief Removes an entity by ID.
     * @param id Entity ID to remove.
     * @return True if an entity was removed.
     */
    bool World::removeEntity(EntityId id) {
        auto it = std::remove_if(entities_.begin(), entities_.end(),
                                 [id](const EntityPtr& p) { return p && p->id() == id; });

        const bool removed = (it != entities_.end());
        entities_.erase(it, entities_.end());
        return removed;
    }

    /**
     * @brief Returns a mutable entity pointer by ID.
     * @param id Entity ID.
     * @return Pointer to entity or nullptr if not found.
     */
    Entity* World::get(EntityId id) {
        for (auto& p : entities_) {
            if (p && p->id() == id) {
                return p.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Returns a read-only entity pointer by ID.
     * @param id Entity ID.
     * @return Pointer to entity or nullptr if not found.
     */
    const Entity* World::get(EntityId id) const {
        for (auto& p : entities_) {
            if (p && p->id() == id) {
                return p.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Updates world simulation: entities, collisions, overlaps, timers, and releases.
     * @param dt Time step in seconds.
     */
    void World::update(double dt) {
        updateGhostRelease();

        handlePacManTurning(dt);
        updateEntities(dt);

        updateCollisions();
        resolveCollisions();

        updateOverlaps();
        resolveOverlaps();

        updateFearTimer(dt);
    }

    /**
     * @brief Ticks entities with dt=0.0 to allow animations without moving simulation forward.
     */
    void World::tickAnimationsOnly() {
        for (auto& e : entities_) {
            if (e && e->active) {
                e->update(0.0);
            }
        }
    }

    /**
     * @brief Evaluates buffered turning requests before movement each frame.
     * @param dt Time step in seconds.
     */
    void World::handlePacManTurning(double dt) {
        for (auto& e : entities_) {
            if (!e || !e->active) {
                continue;
            }

            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (!pac) {
                continue;
            }

            checkPacmanDesiredDirection(*pac, dt);
        }
    }

    /**
     * @brief Calls update(dt) on all active entities.
     * @param dt Time step in seconds.
     */
    void World::updateEntities(double dt) {
        for (auto& e : entities_) {
            if (e && e->active) {
                e->update(dt);
            }
        }
    }

    /**
     * @brief Buffers a direction request for Pac-Man.
     * @param dir Desired direction.
     */
    void World::setPacManDirection(Direction dir) {
        for (auto& e : entities_) {
            if (!e) {
                continue;
            }

            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (pac) {
                pac->setDesiredDirection(dir);
                break;
            }
        }
    }

    /**
     * @brief Detects solid vs solid collisions into lastCollisions_.
     */
    void World::updateCollisions() {
        lastCollisions_.clear();

        const std::size_t n = entities_.size();
        for (std::size_t i = 0; i < n; ++i) {
            const auto& a = entities_[i];
            if (!a || !a->active || !a->solid) {
                continue;
            }

            const Rect ra = a->bounds();

            for (std::size_t j = i + 1; j < n; ++j) {
                const auto& b = entities_[j];
                if (!b || !b->active || !b->solid) {
                    continue;
                }

                const Rect rb = b->bounds();
                if (intersects(ra, rb)) {
                    lastCollisions_.emplace_back(a->id(), b->id());
                }
            }
        }
    }

    /**
     * @brief Detects soft overlaps into lastOverlaps_.
     * @param minOverlapRatio Minimum overlap ratio for overlap to count.
     */
    void World::updateOverlaps(float minOverlapRatio) {
        lastOverlaps_.clear();

        const std::size_t n = entities_.size();
        for (std::size_t i = 0; i < n; ++i) {
            const auto& a = entities_[i];
            if (!a || !a->active) {
                continue;
            }

            const Rect ra = a->bounds();

            for (std::size_t j = i + 1; j < n; ++j) {
                const auto& b = entities_[j];
                if (!b || !b->active) {
                    continue;
                }

                const Rect rb = b->bounds();
                if (!intersects(ra, rb)) {
                    continue;
                }

                const float ratio = overlapRatio(ra, rb);
                if (ratio >= minOverlapRatio) {
                    lastOverlaps_.emplace_back(a->id(), b->id());
                }
            }
        }
    }

    /**
     * @brief Resolves solid collisions by correcting positions.
     */
    void World::resolveCollisions() {
        auto resolvePacmanWall = [](PacMan& pac, const Wall& wall) {
            Rect p = pac.bounds();
            const Rect w = wall.bounds();

            const float pxCenter = p.x + p.w / 2.0f;
            const float pyCenter = p.y + p.h / 2.0f;

            const float wxCenter = w.x + w.w / 2.0f;
            const float wyCenter = w.y + w.h / 2.0f;

            const float halfW = (p.w + w.w) / 2.0f;
            const float halfH = (p.h + w.h) / 2.0f;

            const float dx = pxCenter - wxCenter;
            const float dy = pyCenter - wyCenter;

            const float overlapX = halfW - std::abs(dx);
            const float overlapY = halfH - std::abs(dy);

            if (overlapX < overlapY) {
                p.x = (dx > 0.0f) ? (w.x + w.w) : (w.x - p.w);
            } else {
                p.y = (dy > 0.0f) ? (w.y + w.h) : (w.y - p.h);
            }

            pac.setBounds(p);
        };

        for (const auto& [idA, idB] : lastCollisions_) {
            Entity* a = get(idA);
            Entity* b = get(idB);
            if (!a || !b) {
                continue;
            }

            PacMan* pac = dynamic_cast<PacMan*>(a);
            Wall* wall = dynamic_cast<Wall*>(b);

            if (!pac || !wall) {
                pac = dynamic_cast<PacMan*>(b);
                wall = dynamic_cast<Wall*>(a);
            }

            if (pac && wall) {
                resolvePacmanWall(*pac, *wall);
            }
        }
    }

    /**
     * @brief Resolves overlaps: collects items, triggers fear mode, handles Pac-Man vs ghost.
     */
    void World::resolveOverlaps() {
        for (const auto& [idA, idB] : lastOverlaps_) {
            Entity* a = get(idA);
            Entity* b = get(idB);
            if (!a || !b) {
                continue;
            }

            {
                PacMan* pac = dynamic_cast<PacMan*>(a);
                Coin* coin = dynamic_cast<Coin*>(b);

                if (!pac || !coin) {
                    pac = dynamic_cast<PacMan*>(b);
                    coin = dynamic_cast<Coin*>(a);
                }

                if (pac && coin && coin->active) {
                    coin->collect();
                    coin->active = false;
                }
            }

            {
                PacMan* pac = dynamic_cast<PacMan*>(a);
                Fruit* fruit = dynamic_cast<Fruit*>(b);

                if (!pac || !fruit) {
                    pac = dynamic_cast<PacMan*>(b);
                    fruit = dynamic_cast<Fruit*>(a);
                }

                if (pac && fruit && fruit->active) {
                    fruit->collect();
                    fruit->active = false;

                    startFearMode();
                }
            }

            {
                PacMan* pac = dynamic_cast<PacMan*>(a);
                Ghost* ghost = dynamic_cast<Ghost*>(b);

                if (!pac || !ghost) {
                    pac = dynamic_cast<PacMan*>(b);
                    ghost = dynamic_cast<Ghost*>(a);
                }

                if (pac && ghost && pac->active && ghost->active) {
                    if (ghost->mode() == GhostMode::Fear) {
                        respawnEatenGhost(*ghost);
                    } else {
                        resetActorsAfterPacmanHit(*pac);
                        break;
                    }
                }
            }
        }
    }

    /**
     * @brief Respawns an eaten ghost and re-queues it for gate passing.
     * @param ghost Ghost instance.
     */
    void World::respawnEatenGhost(Ghost& ghost) {
        ghost.resetToSpawn();
        ghost.collectScore();

        for (auto& e : entities_) {
            auto g = std::dynamic_pointer_cast<Ghost>(e);
            if (!g || !g->active) {
                continue;
            }

            if (g.get() == &ghost) {
                gatePass_.push_back(GatePass{g, false});
                if (g->direction() == Direction::None) {
                    g->setDirection(Direction::Up);
                }
                break;
            }
        }
    }

    /**
     * @brief Handles logic after Pac-Man is hit (lives, respawns, fear reset, delay).
     * @param pac Pac-Man instance.
     */
    void World::resetActorsAfterPacmanHit(PacMan& pac) {
        if (lives_ > 0) {
            lives_--;
        }

        pac.resetToSpawn();
        pac.dieScore();

        stopFearMode();

        for (auto& e : entities_) {
            auto g = std::dynamic_pointer_cast<Ghost>(e);
            if (!g || !g->active) {
                continue;
            }
            g->resetToSpawn();
        }

        startGhostReleaseClocks();
        startDelay(1.0);
    }

    /**
     * @brief Returns whether the level is cleared (no active coins or fruits).
     * @return True if cleared.
     */
    bool World::isLevelCleared() const noexcept {
        for (const auto& e : entities_) {
            if (!e || !e->active) {
                continue;
            }

            if (dynamic_cast<const Coin*>(e.get()) != nullptr) {
                return false;
            }
            if (dynamic_cast<const Fruit*>(e.get()) != nullptr) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Checks whether Pac-Man can turn into its desired direction this frame.
     * @param pac Pac-Man instance.
     * @param dt Time step in seconds.
     * @return True if the turn was applied.
     */
    bool World::checkPacmanDesiredDirection(PacMan& pac, double dt) {
        const Direction desired = pac.desiredDirection();
        if (desired == Direction::None) {
            return false;
        }
        if (desired == pac.direction()) {
            return false;
        }

        Rect next = pac.bounds();

        const float step = static_cast<float>(dt);
        next.x += dirToDx(desired) * step;
        next.y += dirToDy(desired) * step;

        for (const auto& ent : entities_) {
            if (!ent || !ent->active || !ent->solid) {
                continue;
            }
            if (ent.get() == &pac) {
                continue;
            }

            auto* wall = dynamic_cast<Wall*>(ent.get());
            if (!wall) {
                continue;
            }

            if (intersects(next, wall->bounds(), 0.0003f)) {
                return false;
            }
        }

        pac.setDirection(desired);
        return true;
    }

    /**
     * @brief Stores the current entity setup as a template for resets.
     */
    void World::snapshotLevelTemplate() {
        levelTemplate_ = entities_;
    }

    /**
     * @brief Resets the level state (clears entities and collision state).
     */
    void World::resetLevel() {
        entities_.clear();
        nextId_ = 1;
        lastCollisions_.clear();
    }

    /**
     * @brief Advances to the next level, reloads the current map, and applies speed scaling.
     */
    void World::advanceLevel() {
        currentLevel_++;
        loadLevel(tileMap_);
        applyLevelSpeedBoost();
    }

    /**
     * @brief Applies a level-based speed multiplier to Pac-Man and ghosts.
     */
    void World::applyLevelSpeedBoost() {
        const double factor = 1.0 + 0.09 * (currentLevel_ - 1);

        for (auto& e : entities_) {
            if (!e || !e->active) {
                continue;
            }

            if (auto pac = dynamic_cast<PacMan*>(e.get())) {
                pac->setSpeed(pac->baseSpeed() * factor);
            } else if (auto g = dynamic_cast<Ghost*>(e.get())) {
                g->setSpeed(g->baseSpeed() * factor);
            }
        }

        fearDuration_ = fearDuration_ * 0.9;
    }

    /**
     * @brief Loads a new level layout into the world.
     * @param map Tile map describing the level.
     */
    void World::loadLevel(const pacman::logic::TileMap& map) {
        tileMap_ = map;

        entities_.clear();
        lastCollisions_.clear();
        nextId_ = 1;

        ghostGateWall_.reset();
        ghostReleaseQueue_.clear();
        nextGhostToRelease_ = 0;

        if (!factory_) {
            return;
        }

        for (int y = 0; y < TileMap::Height; ++y) {
            for (int x = 0; x < TileMap::Width; ++x) {
                const TileType t = tileMap_.at(x, y);
                const Rect r = tileMap_.tileRect(x, y);

                switch (t) {
                    case TileType::Wall: {
                        auto wall = factory_->createWall();
                        if (wall) {
                            wall->setBounds(r);
                            addEntity(wall);
                        }
                        break;
                    }

                    case TileType::Coin: {
                        auto coin = factory_->createCoin();
                        if (coin) {
                            coin->setBounds(r);
                            addEntity(coin);
                        }
                        break;
                    }

                    case TileType::Fruit: {
                        auto fruit = factory_->createFruit();
                        if (fruit) {
                            fruit->setBounds(r);
                            addEntity(fruit);
                        }
                        break;
                    }

                    case TileType::PacManSpawn: {
                        auto pac = factory_->createPacMan();
                        if (pac) {
                            pac->setBounds(r);
                            pac->setStartBounds(r);
                            addEntity(pac);
                        }
                        break;
                    }

                    case TileType::GhostSpawn: {
                        auto gA = factory_->createGhost(GhostKind::A);
                        auto gB = factory_->createGhost(GhostKind::B);
                        auto gC = factory_->createGhost(GhostKind::C);
                        auto gD = factory_->createGhost(GhostKind::D);

                        Rect rA = r;
                        Rect rB = r;
                        Rect rC = r;
                        Rect rD = r;

                        const float ox = r.w * 0.25f;

                        rA.x -= ox;
                        rB.x += ox;
                        rC.x -= ox;
                        rD.x += ox;

                        if (gA) {
                            gA->setBounds(rA);
                            gA->setStartBounds(rA);
                            addEntity(gA);
                            ghostReleaseQueue_.push_back(gA);
                        }
                        if (gB) {
                            gB->setBounds(rB);
                            gB->setStartBounds(rB);
                            addEntity(gB);
                            ghostReleaseQueue_.push_back(gB);
                        }
                        if (gC) {
                            gC->setBounds(rC);
                            gC->setStartBounds(rC);
                            addEntity(gC);
                            ghostReleaseQueue_.push_back(gC);
                        }
                        if (gD) {
                            gD->setBounds(rD);
                            gD->setStartBounds(rD);
                            addEntity(gD);
                            ghostReleaseQueue_.push_back(gD);
                        }

                        break;
                    }

                    case TileType::GhostGate: {
                        auto wall = factory_->createWall();
                        if (wall) {
                            wall->setBounds(r);
                            addEntity(wall);

                            wall->visible = false;
                            wall->solid = true;

                            ghostGateWall_ = std::dynamic_pointer_cast<Wall>(wall);
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        }

        startGhostReleaseClocks();
        snapshotLevelTemplate();
        startDelay(1.0);
    }

    /**
     * @brief Returns the ghost gate wall if available.
     * @return Pointer to gate wall, or nullptr if not present.
     */
    const Wall* World::ghostGate() const noexcept {
        if (auto g = ghostGateWall_.lock()) {
            return g.get();
        }
        return nullptr;
    }

    /**
     * @brief Returns whether the given ghost is currently allowed to pass the gate.
     * @param g Ghost pointer.
     * @return True if the ghost is authorized to pass.
     */
    bool World::canGhostPassGate(const Ghost* g) const noexcept {
        if (!g) {
            return false;
        }

        for (const auto& p : gatePass_) {
            if (p.ghost && p.ghost.get() == g) {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Enables fear mode for all ghosts and resets the fear timer.
     */
    void World::startFearMode() {
        fearActive_ = true;
        fearTimer_ = fearDuration_;

        for (auto& e : entities_) {
            if (!e || !e->active) {
                continue;
            }

            auto ghost = std::dynamic_pointer_cast<Ghost>(e);
            if (!ghost) {
                continue;
            }

            ghost->setMode(GhostMode::Fear);
        }
    }

    /**
     * @brief Disables fear mode for all ghosts immediately.
     */
    void World::stopFearMode() {
        fearActive_ = false;
        fearTimer_ = 0.0;

        for (auto& e : entities_) {
            if (!e || !e->active) {
                continue;
            }

            auto ghost = std::dynamic_pointer_cast<Ghost>(e);
            if (!ghost) {
                continue;
            }

            ghost->setMode(GhostMode::Chase);
        }
    }

    /**
     * @brief Updates the fear timer and exits fear mode at timeout.
     * @param dt Time step in seconds.
     */
    void World::updateFearTimer(double dt) {
        if (!fearActive_) {
            return;
        }

        fearTimer_ -= dt;
        if (fearTimer_ <= 0.0) {
            stopFearMode();
        }
    }

    /**
     * @brief Initializes ghost release timers for a fresh level start.
     */
    void World::startGhostReleaseClocks() {
        levelStartTime_ = Stopwatch::getInstance().elapsed();
        nextGhostToRelease_ = 0;
        gatePass_.clear();
    }

    /**
     * @brief Updates ghost release queue and gate pass bookkeeping.
     */
    void World::updateGhostRelease() {
        if (auto gate = ghostGateWall_.lock()) {
            for (auto it = gatePass_.begin(); it != gatePass_.end();) {
                if (!it->ghost || !it->ghost->active) {
                    it = gatePass_.erase(it);
                    continue;
                }

                const bool touching = intersects(it->ghost->bounds(), gate->bounds(), 0.0003f);
                if (touching) {
                    it->touchedGate = true;
                }

                if (it->touchedGate && !touching) {
                    it = gatePass_.erase(it);
                } else {
                    ++it;
                }
            }
        } else {
            gatePass_.clear();
        }

        if (nextGhostToRelease_ >= ghostReleaseQueue_.size()) {
            return;
        }

        const double now = Stopwatch::getInstance().elapsed();
        const double elapsed = now - levelStartTime_;

        while (nextGhostToRelease_ < ghostReleaseQueue_.size()) {
            const double delay =
                    (nextGhostToRelease_ < ghostReleaseDelays_.size()) ? ghostReleaseDelays_[nextGhostToRelease_] : 0.0;

            if (elapsed < delay) {
                break;
            }

            auto g = ghostReleaseQueue_[nextGhostToRelease_++];
            if (!g || !g->active) {
                continue;
            }

            gatePass_.push_back(GatePass{g, false});

            if (g->direction() == Direction::None) {
                g->setDirection(Direction::Up);
            }
        }
    }

} // namespace pacman::logic