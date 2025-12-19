#pragma once

namespace pacman::app {

/**
 * @brief Shared application-level context passed between states.
 *
 * This structure is intended to store data that must persist
 * across state transitions (e.g. final score).
 */
struct AppContext {
    int finalScore = 0;
};

} // namespace pacman::app