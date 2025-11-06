#include <catch2/catch_test_macros.hpp>
#include "app/states/StateManager.h"
#include "app/states/State.h"
#include <SFML/Window/Event.hpp>

using namespace pacman::app;

namespace {
    struct DummyState : State { // Fake state that counts actions
        using State::State;
        int events{0}, updates{0}, draws{0};
        void handleEvent(const sf::Event&) override { ++events; } // Count event calls
        void update(double) override { ++updates; } // Count updates
        void draw(sf::RenderWindow&) override { ++draws; } // Would draw if called
    };

    struct TrackingFactories { // Keeps track of the created states by ID
        std::vector<DummyState*> createdA;
        std::vector<DummyState*> createdB;

        void registerAll(StateManager& mgr) {
            mgr.registerFactory("A", [this](StateManager& m){
                auto* raw = new DummyState(m);
                createdA.push_back(raw);
                return std::unique_ptr<State>(raw);
            });
            mgr.registerFactory("B", [this](StateManager& m){
                auto* raw = new DummyState(m);
                createdB.push_back(raw);
                return std::unique_ptr<State>(raw);
            });
        }
    };
}

TEST_CASE("StateManager push/replace/pop/clear", "[app][states]") {
    StateManager mgr;
    TrackingFactories track;
    track.registerAll(mgr);

    REQUIRE(mgr.empty());

    // push A
    mgr.push("A");
    REQUIRE(mgr.size() == 1);
    REQUIRE(track.createdA.size() == 1);

    // push B (on top)
    mgr.push("B");
    REQUIRE(mgr.size() == 2);
    REQUIRE(track.createdB.size() == 1);

    // replace top with A (size stays 2)
    mgr.replace("A");
    REQUIRE(mgr.size() == 2);
    REQUIRE(track.createdA.size() == 2); // new A created for replace

    // pop -> back to 1
    mgr.pop();
    REQUIRE(mgr.size() == 1);

    // clear -> 0
    mgr.clear();
    REQUIRE(mgr.empty());
}

TEST_CASE("StateManager dispatches only to the top state", "[app][states]") {
    StateManager mgr;
    TrackingFactories track;
    track.registerAll(mgr);

    // Start with A, then push B on top
    mgr.push("A");
    mgr.push("B");

    REQUIRE(track.createdA.size() == 1);
    REQUIRE(track.createdB.size() == 1);

    auto* A1 = track.createdA.back();
    auto* B1 = track.createdB.back();

    // Call event/update: only top (B1) should increment
    sf::Event ev{};
    mgr.handleEvent(ev);
    mgr.update(0.016);

    REQUIRE(A1->events == 0);
    REQUIRE(A1->updates == 0);
    REQUIRE(B1->events == 1);
    REQUIRE(B1->updates == 1);

    // Replace top with a new A (A2)
    mgr.replace("A");
    REQUIRE(track.createdA.size() == 2);
    auto* A2 = track.createdA.back();

    // Call again: only A2 (new top) should increment
    mgr.handleEvent(ev);
    mgr.update(0.010);

    REQUIRE(A1->events == 0);
    REQUIRE(A1->updates == 0);
    REQUIRE(B1->events == 1);
    REQUIRE(B1->updates == 1);
    REQUIRE(A2->events == 1);
    REQUIRE(A2->updates == 1);

    // Pop -> go back to the previous A1 (stack size now 1)
    mgr.pop();
    // Now top is A1
    mgr.handleEvent(ev);
    mgr.update(0.005);
    REQUIRE(A1->events == 1);
    REQUIRE(A1->updates == 1);
    REQUIRE(A2->events == 1);
    REQUIRE(A2->updates == 1);
}