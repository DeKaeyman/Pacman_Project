#include <catch2/catch_test_macros.hpp>
#include <variant>
#include "logic/observer/Event.h"

using namespace pacman::logic;

TEST_CASE("Event payload type safety for Moved/Collected/StateChanged", "[events]") {
    Event moved{EventType::Moved, MovedPayload{{10.f,20.f},{3.f,4.f}}}; // Create Moved event with position/size
    REQUIRE(std::holds_alternative<MovedPayload>(moved.payload)); // Ensure payload type is MovedPayload
    auto m = std::get<MovedPayload>(moved.payload); // Extract payload
    REQUIRE(m.pos.x == 10.f); // Verify position x
    REQUIRE(m.pos.y == 20.f); // Verify position y
    REQUIRE(m.size.x == 3.f); // Verify size x
    REQUIRE(m.size.y == 4.f); // Verify size y

    Event collected{EventType::Collected, CollectedPayload{250}}; // Create collected event with value
    REQUIRE(std::get<CollectedPayload>(collected.payload).value == 250); // Verify value is correct

    Event changed{EventType::StateChanged, StateChangedPayload{7}}; // Create StateChanged event with code 7
    REQUIRE(std::get<StateChangedPayload>(changed.payload).code == 7); // Ensure stored code matches expected
}