#pragma once

#include <cstdint>
#include <variant>

namespace pacman::logic {

    /**
     * @brief Enumeration of all domain events emitted by logic entities.
     */
    enum class EventType : std::uint8_t {
        Tick,
        Moved,
        StateChanged,
        Collected,
        Died
    };

    /**
     * @brief Simple 2D vector structure.
     */
    struct Vec2 {
        float x{};
        float y{};
    };

    /**
     * @brief Payload for movement or resize events.
     */
    struct MovedPayload {
        Vec2 pos;
        Vec2 size;
    };

    /**
     * @brief Payload describing a generic state change.
     */
    struct StateChangedPayload {
        int code{};
    };

    /**
     * @brief Payload for collection events carrying a score value.
     */
    struct CollectedPayload {
        int value{};
    };

    /**
     * @brief Variant holding optional payload data for an event.
     */
    using EventPayload =
            std::variant<std::monostate, MovedPayload, StateChangedPayload, CollectedPayload>;

    /**
     * @brief Event dispatched by logic subjects to observers.
     */
    struct Event {
        EventType type{};
        EventPayload payload{};
    };

} // namespace pacman::logic