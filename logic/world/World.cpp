#include "World.h"
#include <algorithm>
#include <chrono>

namespace pacman::logic {

    World::EntityId World::addEntity(EntityPtr e) {
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
        for (auto& p : entities_) { // Update all active entities
            if (p && p->active) p->update(dt);
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
        levelTemplate_.clear();
        levelTemplate_.reserve(entities_.size());
        for (const auto& e : entities_) {
            (void)e;
        }
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
}
