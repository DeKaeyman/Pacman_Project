#pragma once
#include "Observer.h"
#include <algorithm>
#include <vector>

namespace pacman::logic {

class Subject { // Observable base that manages a list of observers
public:
    void attach(Observer* o) { // Register an observer if not already present
        if (!o)
            return;
        if (std::find(observers_.begin(), observers_.end(), o) == observers_.end())
            observers_.push_back(o); // Avoid duplicates
    }

    void detach(Observer* o) { // Unregister an observer if present
        auto it = std::remove(observers_.begin(), observers_.end(), o);
        observers_.erase(it, observers_.end());
    }

protected:
    void notify(const Event& e) { // Broadcast an event to all observers
        auto copy = observers_;   // Copy to allow safe modification
        for (auto* o : copy)
            if (o)
                o->onEvent(e); // Call each observer if still valid
    }

private:
    std::vector<Observer*> observers_{}; // Raw pointers with external lifetime management
};
} // namespace pacman::logic