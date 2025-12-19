#pragma once

#include "Observer.h"

#include <algorithm>
#include <vector>

namespace pacman::logic {

    /**
     * @brief Base class for observable logic entities.
     *
     * Manages a list of observers and dispatches events to them.
     * Observers are stored as raw pointers with externally managed lifetimes.
     */
    class Subject {
    public:
        /**
         * @brief Attaches an observer if it is not already registered.
         * @param observer Pointer to the observer.
         */
        void attach(Observer* observer);

        /**
         * @brief Detaches a previously registered observer.
         * @param observer Pointer to the observer.
         */
        void detach(Observer* observer);

    protected:
        /**
         * @brief Notifies all registered observers of an event.
         * @param event The event to dispatch.
         */
        void notify(const Event& event);

    private:
        std::vector<Observer*> observers_;
    };

} // namespace pacman::logic