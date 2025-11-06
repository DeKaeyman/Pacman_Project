#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include "logic/utils/Stopwatch.h"

using pacman::logic::Stopwatch;

TEST_CASE("Stopwatch basic sequence: reset -> tick -> dt/elapsed sane", "[stopwatch]") {
    auto& sw = Stopwatch::getInstance(); // Access singleton
    sw.reset(); // Reset
    sw.tick(); // Do a tick
    const double dt = sw.deltaTime(); // Get delta since last tick
    REQUIRE(dt >= 0.0); // dt should be small
    REQUIRE(dt < 0.05);
    REQUIRE(sw.elapsed() >= 0.0);
}

TEST_CASE("Stopwatch measures time between ticks (~sleep 10ms)", "[stopwatch]") {
    using namespace std::chrono_literals;
    auto& sw = Stopwatch::getInstance();
    sw.reset();
    sw.tick();
    std::this_thread::sleep_for(10ms); //Simulate frame delay
    sw.tick(); // Measure time elapsed between frames
    const double dt = sw.deltaTime();
    REQUIRE(dt >= 0.005); // 5-100ms acceptable window
    REQUIRE(dt <= 0.100);
}

TEST_CASE("Stopwatch monotonic elapsed, delta near-zero for quick successive tick", "[stopwatch]") {
    auto& sw = Stopwatch::getInstance();
    sw.reset();
    sw.tick();
    const double e1 = sw.elapsed(); // Read elapsed time
    sw.tick(); // immediate tick
    const double e2 = sw.elapsed(); // Read again
    REQUIRE(e2 >= e1); // Elapsed never decreased
    REQUIRE(sw.deltaTime() >= 0.0); // Delta non negative
    REQUIRE(sw.deltaTime() < 0.02); // Verry small, almost instant
}