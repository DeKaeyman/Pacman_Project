#pragma once

#include "Event.h"

namespace pacman::logic {

    /**
     * @brief Interface for objects that react to logic events.
     */
    class Observer {
    public:
        /**
         * @brief Virtual destructor for safe polymorphic deletion.
         */
        virtual ~Observer() = default;

        /**
         * @brief Handles a dispatched logic event.
         * @param event The event emitted by a subject.
         */
        virtual void onEvent(const Event& event) = 0;
    };

} // namespace pacman::logic