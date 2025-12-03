// logic/entities/Coin.cpp
#include "Coin.h"
#include "observer/Event.h"

namespace pacman::logic {

Coin::Coin(const Rect &area, int value) : area_(area), value_(value) {
  solid = false; // Coins do not block movement
  active = true; // Coin starts active
}

void Coin::collect() {
  if (!active)
    return; // prevent double collection

  active = false; // Mark coin as removed from world

  CollectedPayload payload{value_}; // Payload containing score value
  Event e{};                        // Create event object
  e.type = EventType::Collected;    // Mark event as "collected"
  e.payload = payload;              // Attach payload

  notify(e); // Notify all attached views/observers
}
} // namespace pacman::logic