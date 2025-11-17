#include "World.h"

#include "../entities/PacMan.h"
#include "../entities/Coin.h"
#include "../entities/Fruit.h"

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
        tileMap_ = map;

        entities_.clear();
        lastCollisions_.clear();
        nextId_ = 1;

        if (!factory_) {
            return;
        }

        for (int y = 0; y < TileMap::Height; y++) {
            for (int x = 0; x < TileMap::Width; x++) {

                const TileType t = tileMap_.at(x, y);
                const Rect r = tileMap_.tileRect(x, y);

                switch (t) {
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

        snapshotLevelTemplate();
    }
}
