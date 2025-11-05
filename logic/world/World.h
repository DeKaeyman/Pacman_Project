#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../entities/Entity.h"
#include "factory/AbstractFactory.h"

namespace pacman::logic {

    inline bool intersects(const Rect& a, const Rect& b) { // Small AABB overlap test in world coordinates
        const bool xoverlap = (a.x <= b.x + b.w) && (b.x <= a.x + a.w);
        const bool yOverlap = (a.y <= b.y + b.h) && (b.y <= a.y + a.h);
        return xoverlap && yOverlap;
    }

    class World {
    public:
        using EntityPtr = std::unique_ptr<Entity>; // World is owner of all entities
        using EntityId = Entity::Id;

        explicit World(AbstractFactory& factory) : factory_{&factory} {};

        EntityId addEntity(EntityPtr e); // Registers entity and returns id
        bool removeEntity(EntityId id); // removes entity based on id

        // Small entity Utils
        Entity* get(EntityId id);
        const Entity* get(EntityId id) const;

        void update(double dt); // Ticks all entities and calculates possible collisions
        const std::vector<std::pair<EntityId, EntityId>>& collisions() const { return lastCollisions_; } // exports all collisions from last update moment

        // Level flow
        void resetLevel(); // reset to startstate
        void advanceLevel(); // go to next level
        int currentLevel() const { return currentLevel_; }

        void snapshotLevelTemplate(); // Startstate of level

        const std::vector<EntityPtr>& entities() const { return entities_; } // For iterating entities

    private:
        AbstractFactory* factory_;
        std::vector<EntityPtr> entities_; // All entities
        std::vector<std::pair<EntityId, EntityId>> lastCollisions_; // Collisions of last frame

        std::vector<EntityPtr> levelTemplate_; // Start state to later reset to
        int currentLevel_{1};
        EntityId nextId_{1}; // Counter of unique id's
    };
}
