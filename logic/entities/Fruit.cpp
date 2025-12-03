// logic/entities/Fruit.cpp
#include "Fruit.h"
#include "observer/Event.h"

namespace pacman::logic {

Fruit::Fruit(const Rect& area, int value) : area_(area), value_(value) {
    solid = false; // Fruit does not block movement
    active = true; // Fruit begins active and drawable
}

void Fruit::collect() {
    if (!active)
        return; // Skip if already collected

    active = false; // Mark fruit as removed from world

    CollectedPayload payload{value_}; // Score value inside payload
    Event e{};                        // Event object
    e.type = EventType::Collected;    // Event identifies “Fruit/Coin collected”
    e.payload = payload;              // Attach payload data

    notify(e); // Notify views
}
} // namespace pacman::logic