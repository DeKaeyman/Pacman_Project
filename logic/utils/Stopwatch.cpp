#include "Stopwatch.h"

#include <chrono>
#include <mutex>

namespace pacman::logic {

/**
 * @brief Constructs the stopwatch and initializes its internal timestamps.
 */
Stopwatch::Stopwatch() : startTime_(Clock::now()), lastTick_(startTime_), deltaTime_(0.0) {}

/**
 * @brief Returns the singleton Stopwatch instance.
 * @return Reference to the global stopwatch.
 */
Stopwatch& Stopwatch::getInstance() {
    static Stopwatch instance;
    return instance;
}

/**
 * @brief Updates the delta time based on the time since the last tick.
 */
void Stopwatch::tick() {
    std::scoped_lock lock(mtx_);

    const auto now = Clock::now();
    deltaTime_ = std::chrono::duration<double>(now - lastTick_).count();
    lastTick_ = now;
}

/**
 * @brief Resets the stopwatch start time and clears delta time.
 */
void Stopwatch::reset() {
    std::scoped_lock lock(mtx_);

    startTime_ = Clock::now();
    lastTick_ = startTime_;
    deltaTime_ = 0.0;
}

/**
 * @brief Returns the time in seconds between the last two tick() calls.
 * @return Delta time in seconds.
 */
double Stopwatch::deltaTime() const noexcept {
    std::scoped_lock lock(mtx_);
    return deltaTime_;
}

/**
 * @brief Returns the elapsed time in seconds since the last reset (or construction).
 * @return Elapsed time in seconds.
 */
double Stopwatch::elapsed() const noexcept {
    std::scoped_lock lock(mtx_);

    const auto now = Clock::now();
    return std::chrono::duration<double>(now - startTime_).count();
}

} // namespace pacman::logic