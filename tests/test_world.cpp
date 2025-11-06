#include <catch2/catch_test_macros.hpp>
#include <memory>
#include "logic/world/World.h"
#include "logic/entities/Entity.h"
#include "logic/factory/AbstractFactory.h"

using namespace pacman::logic;

namespace {
    struct MockFactory : AbstractFactory { // Dummy factory returning nullptrs
        std::shared_ptr<PacMan> createPacMan() override { return nullptr; }
        std::shared_ptr<Ghost>  createGhost(GhostKind) override { return nullptr; }
        std::shared_ptr<Coin>   createCoin() override { return nullptr; }
        std::shared_ptr<Fruit>  createFruit() override { return nullptr; }
        std::shared_ptr<Wall>   createWall() override { return nullptr; }
    };

    struct TestEntity : Entity { // Minimal entity tracking updates
        explicit TestEntity(Rect r, bool solid_ = true) : rect_(r) {
            solid = solid_;
            active = true;
        }
        void update(double dt) override {
            rect_.x += static_cast<float>(dt);
            ++updated;
        }
        Rect bounds() const override { return rect_; }
        int updated{0};
    private:
        Rect rect_{};
    };
}

TEST_CASE("AABB intersects covers overlaps and edge-touch", "[world][aabb]") {
    Rect a{0,0,10,10};
    Rect b{5,5,10,10};
    Rect c{10,0,5,5};
    Rect d{0,10,5,5};
    Rect e{11,0,5,5};

    REQUIRE(intersects(a,b)); // Overlap area
    REQUIRE(intersects(a,c)); // Touching on the right edge
    REQUIRE(intersects(a,d)); // Touching on the bottom edge
    REQUIRE_FALSE(intersects(a,e)); // Seperated rectangles do not intersect
}

TEST_CASE("World add/remove assigns incremental ids and handles empties", "[world]") {
    MockFactory mf;
    World w(mf);

    auto id1 = w.addEntity(std::make_unique<TestEntity>(Rect{0,0,1,1}));
    auto id2 = w.addEntity(std::make_unique<TestEntity>(Rect{1,1,1,1}));

    REQUIRE(id1 != 0); // IDs increment sequentially
    REQUIRE(id2 == id1 + 1);

    REQUIRE(w.removeEntity(id1) == true); // Remove existing entity
    REQUIRE(w.removeEntity(9999) == false); // Ignore non existing id
}

TEST_CASE("World update calls Entity::update and records solid-solid collisions", "[world]") {
    MockFactory mf;
    World w(mf);

    auto idA = w.addEntity(std::make_unique<TestEntity>(Rect{0,0,10,10}, true));
    auto idB = w.addEntity(std::make_unique<TestEntity>(Rect{5,5,10,10}, true));
    w.addEntity(std::make_unique<TestEntity>(Rect{0,0,100,100}, false)); // Non solid ignored

    w.update(0.016); // Simulate frame update

    int updatedCount = 0;
    w.forEachEntity([&](Entity& e){ (void)e; ++updatedCount; });
    REQUIRE(updatedCount == 3); // All 3 got updated

    const auto& pairs = w.lastCollisions(); // Only solid pair recorded
    REQUIRE(pairs.size() == 1);
    const auto p = pairs.front();
    REQUIRE(( (p.first == idA && p.second == idB) || (p.first == idB && p.second == idA) ));
}

TEST_CASE("World reset/advance clear entities and collisions", "[world]") {
    MockFactory mf;
    World w(mf);

    w.addEntity(std::make_unique<TestEntity>(Rect{0,0,1,1}));
    w.update(0.01);
    REQUIRE_FALSE(w.entities().empty()); // Entities exist
    REQUIRE(w.lastCollisions().empty()); // No collisions yet

    w.resetLevel(); // Clear level data
    REQUIRE(w.entities().empty());
    REQUIRE(w.lastCollisions().empty());

    w.advanceLevel(); // increment level and clear
    REQUIRE(w.entities().empty());
    REQUIRE(w.lastCollisions().empty());
}