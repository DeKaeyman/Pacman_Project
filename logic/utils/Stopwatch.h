#pragma once
#include <chrono>
#include <mutex>

namespace pacman::logic {

class Stopwatch {
public:
  static Stopwatch &getInstance(); // Returns the global singleton instance

  void tick();  // Updates deltaTime based on time since last tick
  void reset(); // Resets the clock to current time and clears deltaTime

  double deltaTime() const noexcept; // Time in seconds between last two ticks
  double
  elapsed() const noexcept; // Total time in seconds since stopwatch start

private:
  Stopwatch();
  ~Stopwatch() = default;
  Stopwatch(const Stopwatch &) = delete;            // Disable copy constructor
  Stopwatch &operator=(const Stopwatch &) = delete; // Disable assign operator

  using Clock = std::chrono::high_resolution_clock; // High precision clock type
  using TimePoint =
      std::chrono::time_point<Clock>; // Represents specific time points

  TimePoint startTime_;    // Timestamp when stopwatch started since last reset
  TimePoint lastTick_;     // Timestamp of last tick() call
  double deltaTime_{0.0};  // Duration in seconds between last two ticks
  mutable std::mutex mtx_; // Mutex to protect concurrent read/write access
};
} // namespace pacman::logic