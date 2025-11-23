#include "World.h"

#include "../entities/PacMan.h"
#include "../entities/Coin.h"
#include "../entities/Fruit.h"
#include "../entities/Wall.h"

#include <algorithm>
#include <chrono>

namespace pacman::logic {

    World::EntityId World::addEntity(EntityPtr e) {
        if (!e) return 0;
        e->setId(nextId_++); // Gives unique id to the entity
        EntityId id = e->id(); // Take a snapshot of the id
        entities_.push_back(std::move(e)); // Make world owner of the entity
        return id;
    }

    bool World::removeEntity(EntityId id) {
        auto it = std::remove_if(entities_.begin(), entities_.end(), [id](const EntityPtr& p) { return p && p->id() == id; });
        const bool removed = it != entities_.end(); // removed = something has been removed
        entities_.erase(it, entities_.end()); // Finalised it by removing it
        return removed;
    }

    Entity* World::get(EntityId id) {
        for (auto& p : entities_) if (p && p->id() == id) return p.get(); // Search for a valid entity based on the id
        return nullptr;
    }

    const Entity *World::get(EntityId id) const { // Const variant for only read access
        for (auto& p : entities_) if (p && p->id() == id) return p.get(); // Search for a valid entity based on the id
        return nullptr;
    }

    void World::update(double dt) {
        for (auto& e : entities_) {
            if (!e || !e->active) continue;
            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (!pac) continue;

            checkPacmanDesiredDirection(*pac, dt);
        }

        for (auto& e : entities_) { // Update all active entities
            if (e && e->active) e->update(dt);
        }

        lastCollisions_.clear(); // Calculate all pairs of entities that overlap after update
        const std::size_t n = entities_.size();
        for (std::size_t i = 0; i < n; i++) {
            const auto& a = entities_[i];
            if (!a || !a->active || !a -> solid) continue; // Skip non active / non solid entities
            const Rect ra = a->bounds(); // Ask AABB from the logic part
            for (std::size_t j = i + 1; j < n; j++) { // i+1 prevent double checks (a,a)
                const auto& b = entities_[j];
                if (!b || !b->active || !b->solid) continue;
                const Rect rb = b->bounds();
                if (intersects(ra, rb)) {
                    lastCollisions_.emplace_back(a->id(), b->id());
                }
            }
        }

        auto resolvePacmanWall = [](PacMan& pac, const Wall& wall) {
            Rect p = pac.bounds();
            Rect w = wall.bounds();

            float pxCenter = p.x + p.w / 2.f;
            float pyCenter = p.y + p.h / 2.f;

            float wxCenter = w.x + w.w / 2.f;
            float wyCenter = w.y + w.h / 2.f;

            float halfW = (p.w + w.w) / 2.f;
            float halfH = (p.h + w.h) / 2.f;

            float dx = pxCenter - wxCenter;
            float dy = pyCenter - wyCenter;

            float overlapX = halfW - std::abs(dx);
            float overlapY = halfH - std::abs(dy);

            if (overlapX < overlapY) {
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

            pac.setBounds(p);
            //pac.setDirection(Direction::None);
        };

        for (const auto& [idA, idB] : lastCollisions_) {
            Entity* a = get(idA);
            Entity* b = get(idB);
            if (!a || !b) continue;

            PacMan* pac = dynamic_cast<PacMan*>(a);
            Wall* wall = dynamic_cast<Wall*>(b);

            if (!pac || !wall) {
                pac  = dynamic_cast<PacMan*>(b);
                wall = dynamic_cast<Wall*>(a);
            }

            if (pac && wall) {
                resolvePacmanWall(*pac, *wall);
            }
        }
    }

    void World::setPacManDirection(Direction dir) {
        for (auto& e : entities_) {
            if (!e) continue;
            auto pac = std::dynamic_pointer_cast<PacMan>(e);
            if (pac) {
                pac->setDesiredDirection(dir);
                break;
            }
        }
    }

    bool World::checkPacmanDesiredDirection(PacMan& pac, double dt) {
        Direction desired = pac.desiredDirection();
        if (desired == Direction::None) return false;
        if (desired == pac.direction()) return false;

        Rect next = pac.bounds();
        float step = static_cast<float>(dt);
        next.x += dirToDx(desired) * step;
        next.y += dirToDy(desired) * step;

        for (const auto& ent : entities_) {
            if (!ent || !ent->active || !ent->solid) continue;
            if (ent.get() == &pac) continue;

            auto* wall = dynamic_cast<Wall*>(ent.get());
            if (!wall) continue;

            if (intersects(next, wall->bounds(), 0.0003f)) {
                return false;
            }
        }

        pac.setDirection(desired);
        return true;
    }

    void World::snapshotLevelTemplate() { // Setup a startstate snapshot to support level reset
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

    void World::loadLevel(const pacman::logic::TileMap &map) {
        tileMap_ = map; // Store a copy of the given tile layout

        entities_.clear(); // Remove any existing entities
        lastCollisions_.clear(); // Clear previous collision data
        nextId_ = 1; // Reset entity id counter

        if (!factory_) {
            return; // Without a factory we cannot create models/views
        }

        for (int y = 0; y < TileMap::Height; y++) { // Loop over all rows
            for (int x = 0; x < TileMap::Width; x++) { // Loop over all columns

                const TileType t = tileMap_.at(x, y); // Type of tile on this location
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
                            addEntity(pac);
                        }
                        break;
                    }

                    default:
                        break;
                }
            }
        }

        snapshotLevelTemplate(); // Store current entity setup as "initial state" for reset
    }
}
