#include "Subject.h"

namespace pacman::logic {

void Subject::attach(Observer* observer) {
    if (!observer) {
        return;
    }

    if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

void Subject::detach(Observer* observer) {
    const auto it = std::remove(observers_.begin(), observers_.end(), observer);
    observers_.erase(it, observers_.end());
}

void Subject::notify(const Event& event) {
    const auto snapshot = observers_;
    for (auto* observer : snapshot) {
        if (observer) {
            observer->onEvent(event);
        }
    }
}

} // namespace pacman::logic