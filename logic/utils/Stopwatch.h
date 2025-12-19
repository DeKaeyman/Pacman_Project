#pragma once

#include <chrono>
#include <mutex>

namespace pacman::logic {

/**
 * @brief Thread-safe singleton stopwatch for timing and animation.
 *
 * tick() updates the delta time since the previous tick.
 * reset() restarts elapsed time and clears delta time.
 */
class Stopwatch {
public:
    /**
     * @brief Returns the singleton Stopwatch instance.
     * @return Reference to the global stopwatch.
     */
    static Stopwatch& getInstance();

    /**
     * @brief Updates deltaTime based on time since the last tick() call.
     */
    void tick();

    /**
     * @brief Resets the stopwatch to the current time and clears deltaTime.
     */
    void reset();

    /**
     * @brief Returns time in seconds between the last two tick() calls.
     * @return Delta time in seconds.
     */
    double deltaTime() const noexcept;

    /**
     * @brief Returns total time in seconds since construction or the last reset().
     * @return Elapsed time in seconds.
     */
    double elapsed() const noexcept;

private:
    /**
     * @brief Constructs the stopwatch and initializes timestamps.
     */
    Stopwatch();

    /**
     * @brief Default destructor.
     */
    ~Stopwatch() = default;

    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint startTime_;
    TimePoint lastTick_;
    double deltaTime_{0.0};
    mutable std::mutex mtx_;
};

} // namespace pacman::logic