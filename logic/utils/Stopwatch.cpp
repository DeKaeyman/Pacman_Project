#include "Stopwatch.h"

namespace pacman::logic {

    Stopwatch::Stopwatch() {
        startTime_ = Clock::now(); // Capture current time as start
        lastTick_  = startTime_; // Initialize lastTick to same value
    }

    Stopwatch& Stopwatch::getInstance() {
        static Stopwatch instance; // Static instance ensures one global stopwatch
        return instance;
    }

    void Stopwatch::tick() {
        std::scoped_lock lock(mtx_); // Thread safe access to timing data
        const auto now = Clock::now(); // Capture current time
        deltaTime_ = std::chrono::duration<double>(now - lastTick_).count(); // Compute time since last tick
        lastTick_ = now; // Update last tick
    }

    void Stopwatch::reset() {
        std::scoped_lock lock(mtx_); // Thread safe reset
        startTime_ = Clock::now(); // Reset start time to now
        lastTick_  = startTime_; // Reset last tick to same value
        deltaTime_ = 0.0; // Clear previous delta
    }

    double Stopwatch::deltaTime() const noexcept {
        std::scoped_lock lock(mtx_); // Protect concurrent reads
        return deltaTime_; // Return time difference between ticks
    }

    double Stopwatch::elapsed() const noexcept {
        std::scoped_lock lock(mtx_); // Thread safe read of elapsed time
        const auto now = Clock::now(); // Capture current time
        return std::chrono::duration<double>(now - startTime_).count(); // Seconds since start/reset
    }
}