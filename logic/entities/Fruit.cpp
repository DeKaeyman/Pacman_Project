#include "Fruit.h"

#include "observer/Event.h"

namespace pacman::logic {

/**
 * @brief Constructs a fruit entity with a given bounding area and score value.
 *
 * Fruits are non-solid collectables that emit a Collected event
 * when picked up by the player.
 *
 * @param area World-space bounding box of the fruit.
 * @param value Score value awarded upon collection.
 */
Fruit::Fruit(const Rect& area, int value) : area_(area), value_(value) {
    solid = false;
    active = true;
}

/**
 * @brief Marks the fruit as collected and notifies observers.
 *
 * If the fruit is already inactive, the call has no effect.
 * On successful collection, a Collected event is emitted
 * containing the fruit's score value.
 */
void Fruit::collect() {
    if (!active) {
        return;
    }

    active = false;

    CollectedPayload payload{value_};
    Event event{};
    event.type = EventType::Collected;
    event.payload = payload;

    notify(event);
}

} // namespace pacman::logic