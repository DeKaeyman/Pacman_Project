#include "Coin.h"

#include "observer/Event.h"

namespace pacman::logic {

    /**
     * @brief Constructs a coin entity with a given bounding area and score value.
     *
     * Coins are non-solid entities that can be collected once.
     *
     * @param area World-space bounding box of the coin.
     * @param value Score value granted when the coin is collected.
     */
    Coin::Coin(const Rect& area, int value)
            : area_(area),
              value_(value) {
        solid = false;
        active = true;
    }

    /**
     * @brief Marks the coin as collected and notifies observers.
     *
     * If the coin is already inactive, the call has no effect.
     * On successful collection, a Collected event is emitted
     * containing the coin's score value.
     */
    void Coin::collect() {
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