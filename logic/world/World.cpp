#include "World.h"

#include "../entities/Coin.h"
#include "../entities/Fruit.h"
#include "../entities/Ghost.h"
#include "../entities/PacMan.h"
#include "../entities/Wall.h"

#include "../utils/Stopwatch.h"

#include <algorithm>
#include <chrono>

namespace pacman::logic {

World::EntityId World::addEntity(EntityPtr e) {
    if (!e)
        return 0;
    e->setId(nextId_++); // Gives unique id to the entity

    if (auto ghost = std::dynamic_pointer_cast<Ghost>(e)) {
        ghost->setWorld(this);
    }

    EntityId id = e->id();             // Take a snapshot of the id
    entities_.push_back(std::move(e)); // Make world owner of the entity
    return id;
}

bool World::removeEntity(EntityId id) {
    auto it =
        std::remove_if(entities_.begin(), entities_.end(), [id](const EntityPtr& p) { return p && p->id() == id; });
    const bool removed = it != entities_.end(); // removed = something has been removed
    entities_.erase(it, entities_.end());       // Finalised it by removing it
    return removed;
}

Entity* World::get(EntityId id) {
    for (auto& p : entities_)
        if (p && p->id() == id)
            return p.get(); // Search for a valid entity based on the id
    return nullptr;
}

const Entity* World::get(EntityId id) const { // Const variant for only read access
    for (auto& p : entities_)
        if (p && p->id() == id)
            return p.get(); // Search for a valid entity based on the id
    return nullptr;
}

void World::update(double dt) {
    updateGhostRelease();

    handlePacManTurning(dt); // 1) apply buffered input to Pac-Man if possible
    updateEntities(dt);      // 2) move all entities

    updateCollisions(); // 3) hard collisions
    resolveCollisions();

    updateOverlaps(); // 4) soft overlaps
    resolveOverlaps();

    updateFearTimer(dt);
}

void World::handlePacManTurning(double dt) {
    for (auto& e : entities_) {
        if (!e || !e->active)
            continue; // Only active entities participate
        auto pac = std::dynamic_pointer_cast<PacMan>(e);
        if (!pac)
            continue; // Skip non Pacman entities

        checkPacmanDesiredDirection(*pac, dt); // Try to turn Pacman if buffered direction is valid
    }
}

void World::updateEntities(double dt) {
    for (auto& e : entities_) { // Update all active entities
        if (e && e->active)
            e->update(dt);
    }
}

void World::setPacManDirection(Direction dir) {
    for (auto& e : entities_) {
        if (!e)
            continue;
        auto pac = std::dynamic_pointer_cast<PacMan>(e);
        if (pac) {
            pac->setDesiredDirection(dir); // Store buffered input
            break;
        }
    }
}

void World::updateCollisions() {
    lastCollisions_.clear(); // Calculate all pairs of entities that overlap after update
    const std::size_t n = entities_.size();
    for (std::size_t i = 0; i < n; i++) {
        const auto& a = entities_[i];
        if (!a || !a->active || !a->solid)
            continue;                             // Skip non active / non solid entities
        const Rect ra = a->bounds();              // Ask AABB from the logic part
        for (std::size_t j = i + 1; j < n; j++) { // i+1 prevent double checks (a,a)=mù:
            const auto& b = entities_[j];
            if (!b || !b->active || !b->solid)
                continue;
            const Rect rb = b->bounds();
            if (intersects(ra, rb)) {
                lastCollisions_.emplace_back(a->id(), b->id());
            }
        }
    }
}

void World::updateOverlaps(float minOverlapRatio) {
    lastOverlaps_.clear();
    const std::size_t n = entities_.size();

    for (std::size_t i = 0; i < n; ++i) {
        const auto& a = entities_[i];
        if (!a || !a->active)
            continue;
        const Rect ra = a->bounds();

        for (std::size_t j = i + 1; j < n; ++j) {
            const auto& b = entities_[j];
            if (!b || !b->active)
                continue;
            const Rect rb = b->bounds();

            if (!intersects(ra, rb))
                continue;

            const float ratio = overlapRatio(ra, rb);
            if (ratio >= minOverlapRatio) {
                lastOverlaps_.emplace_back(a->id(), b->id());
            }
        }
    }
}

void World::resolveCollisions() {
    auto resolvePacmanWall = [](PacMan& pac, const Wall& wall) {
        Rect p = pac.bounds();  // Pacman current position
        Rect w = wall.bounds(); // Wall bounds

        float pxCenter = p.x + p.w / 2.f; // Pacman center
        float pyCenter = p.y + p.h / 2.f;

        float wxCenter = w.x + w.w / 2.f; // Wall center
        float wyCenter = w.y + w.h / 2.f;

        float halfW = (p.w + w.w) / 2.f; // Half extents combined
        float halfH = (p.h + w.h) / 2.f;

        float dx = pxCenter - wxCenter; // Distance between centers
        float dy = pyCenter - wyCenter;

        float overlapX = halfW - std::abs(dx); // Overlap on X axis
        float overlapY = halfH - std::abs(dy); // Overlap on Y axis

        if (overlapX < overlapY) { // Push back on shallowest penetration axis
            if (dx > 0)
                p.x = w.x + w.w;
            else
                p.x = w.x - p.w;
        } else {
            if (dy > 0)
                p.y = w.y + w.h;
            else
                p.y = w.y - p.h;
        }

        pac.setBounds(p); // Apply correct position
    };

    for (const auto& [idA, idB] : lastCollisions_) {
        Entity* a = get(idA);
        Entity* b = get(idB);
        if (!a || !b)
            continue;

        // Pac-Man <=> Wall
        {
            PacMan* pac = dynamic_cast<PacMan*>(a);
            Wall* wall = dynamic_cast<Wall*>(b);

            if (!pac || !wall) { // Try reversed order
                pac = dynamic_cast<PacMan*>(b);
                wall = dynamic_cast<Wall*>(a);
            }

            if (pac && wall) {
                resolvePacmanWall(*pac, *wall); // Clip Pacman against wall
            }
        }
    }
}

void World::resolveOverlaps() {
    for (const auto& [idA, idB] : lastOverlaps_) {
        Entity* a = get(idA);
        Entity* b = get(idB);
        if (!a || !b)
            continue;

        // Pac-Man <=> Coin
        {
            PacMan* pac = dynamic_cast<PacMan*>(a);
            Coin* coin = dynamic_cast<Coin*>(b);

            if (!pac || !coin) { // Reverse lookup
                pac = dynamic_cast<PacMan*>(b);
                coin = dynamic_cast<Coin*>(a);
            }

            // Pacman touching a coin -> collect it
            if (pac && coin && coin->active) {
                coin->collect();      // Notify score + views
                coin->active = false; // Hide/remove from gameplay
            }
        }

        // Pac-Man <=> Fruit
        {
            PacMan* pac = dynamic_cast<PacMan*>(a);
            Fruit* fruit = dynamic_cast<Fruit*>(b);

            if (!pac || !fruit) { // Reverse lookup
                pac = dynamic_cast<PacMan*>(b);
                fruit = dynamic_cast<Fruit*>(a);
            }

            if (pac && fruit && fruit->active) {
                fruit->collect();      // Notify score + views
                fruit->active = false; // Hide/remove from gameplay

                startFearMode();
            }
        }

        // Pac-Man <=> Ghost
        {
            PacMan* pac = dynamic_cast<PacMan*>(a);
            Ghost* ghost = dynamic_cast<Ghost*>(b);

            if (!pac || !ghost) { // Reverse lookup
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
                continue;
            }
        }
    }
}

void World::respawnEatenGhost(Ghost& ghost) {
    ghost.resetToSpawn();

    for (auto& e : entities_) {
        auto g = std::dynamic_pointer_cast<Ghost>(e);
        if (!g || !g->active)
            continue;

        if (g.get() == &ghost) {
            gatePass_.push_back(GatePass{g, false});
            if (g->direction() == Direction::None)
                g->setDirection(Direction::Up);
            break;
        }
    }
}

void World::resetActorsAfterPacmanHit(PacMan& pac) {
    if (lives_ > 0)
        lives_--;

    pac.resetToSpawn();

    stopFearMode();

    for (auto& e : entities_) {
        auto g = std::dynamic_pointer_cast<Ghost>(e);
        if (!g || !g->active)
            continue;

        g->resetToSpawn();
    }

    startGhostReleaseClocks();
}

bool World::checkPacmanDesiredDirection(PacMan& pac, double dt) {
    Direction desired = pac.desiredDirection();
    if (desired == Direction::None)
        return false; // No intent
    if (desired == pac.direction())
        return false; // Already facing that way

    Rect next = pac.bounds(); // Simulate small step in new direction
    float step = static_cast<float>(dt);
    next.x += dirToDx(desired) * step;
    next.y += dirToDy(desired) * step;

    for (const auto& ent : entities_) { // Check if Pacman would hit wall
        if (!ent || !ent->active || !ent->solid)
            continue;
        if (ent.get() == &pac)
            continue;

        auto* wall = dynamic_cast<Wall*>(ent.get());
        if (!wall)
            continue;

        if (intersects(next, wall->bounds(),
                       0.0003f)) { // Tiny epsilon to avoid jitter
            return false;          // Wall blocks turn
        }
    }

    pac.setDirection(desired); // Turn allowed -> apply it
    return true;
}

void World::snapshotLevelTemplate() { // Setup a startstate snapshot to support
                                      // level reset
    levelTemplate_ = entities_;
}

void World::resetLevel() {
    entities_.clear();
    nextId_ = 1;
    lastCollisions_.clear();
}

void World::advanceLevel() {
    currentLevel_++;
    resetLevel(); // Start new level
}

void World::loadLevel(const pacman::logic::TileMap& map) {
    tileMap_ = map; // Store a copy of the given tile layout

    entities_.clear();       // Remove any existing entities
    lastCollisions_.clear(); // Clear previous collision data
    nextId_ = 1;             // Reset entity id counter
    ghostGateWall_.reset();
    ghostReleaseQueue_.clear();
    nextGhostToRelease_ = 0;

    if (!factory_) {
        return; // Without a factory we cannot create models/views
    }

    for (int y = 0; y < TileMap::Height; y++) {    // Loop over all rows
        for (int x = 0; x < TileMap::Width; x++) { // Loop over all columns

            const TileType t = tileMap_.at(x, y);   // Type of tile on this location
            const Rect r = tileMap_.tileRect(x, y); // World space rectangle for that tile

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
    snapshotLevelTemplate(); // Store current entity setup as "initial state" for
                             // reset
}

const Wall* World::ghostGate() const noexcept {
    if (auto g = ghostGateWall_.lock())
        return g.get();
    return nullptr;
}

bool World::canGhostPassGate(const Ghost* g) const noexcept {
    if (!g)
        return false;
    for (const auto& p : gatePass_) {
        if (p.ghost && p.ghost.get() == g)
            return true;
    }
    return false;
}

void World::startFearMode() {
    fearActive_ = true;         // Mark fear mode as globally active
    fearTimer_ = fearDuration_; // Reset countdown timer

    for (auto& e : entities_) {
        if (!e || !e->active)
            continue; // Ignore inactive entities
        auto ghost = std::dynamic_pointer_cast<Ghost>(e);
        if (!ghost)
            continue; // Only apply to ghosts

        ghost->setMode(GhostMode::Fear); // Switch ghost to blue/slow mode immediately
    }
}

void World::stopFearMode() {
    fearActive_ = false; // Fear mode no longer active
    fearTimer_ = 0.0;    // Timer cleared

    for (auto& e : entities_) {
        if (!e || !e->active)
            continue;
        auto ghost = std::dynamic_pointer_cast<Ghost>(e);
        if (!ghost)
            continue;

        ghost->setMode(GhostMode::Chase); // Restore normal behavior & appearance
    }
}

void World::updateFearTimer(double dt) {
    if (!fearActive_)
        return; // No need to track timer if not afraid

    fearTimer_ -= dt;        // Decrease remaining fear duration
    if (fearTimer_ <= 0.0) { // Blue mode finished
        stopFearMode();      // Restore ghosts to normal state
    }
}

void World::startGhostReleaseClocks() {
    levelStartTime_ = Stopwatch::getInstance().elapsed();
    nextGhostToRelease_ = 0;
    gatePass_.clear();
}

void World::updateGhostRelease() {
    if (auto gate = ghostGateWall_.lock()) {
        for (auto it = gatePass_.begin(); it != gatePass_.end();) {
            if (!it->ghost || !it->ghost->active) {
                it = gatePass_.erase(it);
                continue;
            }
            bool touching = intersects(it->ghost->bounds(), gate->bounds(), 0.0003f);
            if (touching)
                it->touchedGate = true;

            if (it->touchedGate && !touching)
                it = gatePass_.erase(it);
            else
                ++it;
        }
    } else {
        gatePass_.clear();
    }

    if (nextGhostToRelease_ >= ghostReleaseQueue_.size())
        return;

    const double now = Stopwatch::getInstance().elapsed();
    const double elapsed = now - levelStartTime_;

    while (nextGhostToRelease_ < ghostReleaseQueue_.size()) {
        double delay =
            (nextGhostToRelease_ < ghostReleaseDelays_.size()) ? ghostReleaseDelays_[nextGhostToRelease_] : 0.0;

        if (elapsed < delay)
            break;

        auto g = ghostReleaseQueue_[nextGhostToRelease_++];
        if (!g || !g->active)
            continue;

        gatePass_.push_back(GatePass{g, false});

        if (g->direction() == Direction::None)
            g->setDirection(Direction::Up);
    }
}
} // namespace pacman::logic
