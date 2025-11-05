#pragma once
#include <cstdint>
#include <variant>

namespace pacman::logic {

    enum class EventType : std::uint8_t { // Kinds of domain events emitted by models
        Tick, // Per update
        Moved, // Position/size change of an entity
        StateChanged, // State change code (e.g. mode/phase)
        Collected, // Item collected with value attached
        Died, // Entity death
    };

    struct Vec2 { float x{}, y{}; }; // Simple 2D vector for positions/sizes

    struct MovedPayload { Vec2 pos; Vec2 size; }; // Data for movement/resize events
    struct StateChangedPayload { int code; }; // Generic code to qualify a state change
    struct CollectedPayload { int value; }; // Value carried by collected items

    using EventPayload = std::variant<std::monostate, MovedPayload, StateChangedPayload, CollectedPayload>; // Payload for events

    struct Event { // Event passed to observers
        EventType type{}; // Event category
        EventPayload payload{}; // Optional data for the event
    };
}