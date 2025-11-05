#pragma once
#include "Event.h"

namespace pacman::logic {

    class Observer { // Interface foor anything that reacts to logic events
    public:
        virtual ~Observer() = default;
        virtual void onEvent(const Event& e) = 0; // Handle a dispatched event
    };
}