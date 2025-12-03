#include "logic/observer/Event.h"
#include "logic/observer/Observer.h"
#include "logic/observer/Subject.h"
#include <catch2/catch_test_macros.hpp>

using namespace pacman::logic;

namespace {
struct TestObserver : Observer { // Simple observer counting received events
  int received{0};
  void onEvent(const Event &e) override {
    (void)e;
    ++received; // Increment counter for each event
  }
};

struct TestSubject : Subject {             // Exposes notify()
  void emit(const Event &e) { notify(e); } // Allow manual notify from tests
};

struct DetachingObserver
    : Observer { // Observer that detaches itself on first event
  Subject *subject{};
  int received{0};
  void onEvent(const Event &e) override {
    (void)e;
    ++received;
    if (subject)
      subject->detach(this); // detach during notify
  }
};
} // namespace

TEST_CASE("Subject attach ignores duplicates; detach removes", "[observer]") {
  TestSubject s;
  TestObserver a, b;

  s.attach(&a);
  s.attach(&a); // duplicate ignored
  s.attach(&b);

  Event e{EventType::Tick, std::monostate{}};
  s.emit(e);
  REQUIRE(a.received == 1); // Each observer notified once
  REQUIRE(b.received == 1);

  s.detach(&a); // Remove 1 observer
  s.emit(e);    // only b remains active
  REQUIRE(a.received == 1);
  REQUIRE(b.received == 2);
}

TEST_CASE("Detach during notify does not break iteration (copy-notify)",
          "[observer]") {
  TestSubject s;
  DetachingObserver a; // Observer that will remove itself
  a.subject = &s;
  TestObserver b;

  s.attach(&a);
  s.attach(&b);

  Event e{EventType::Tick, std::monostate{}}; // Trigger notify
  s.emit(e);

  REQUIRE(a.received == 1); // a got event and detached itself
  REQUIRE(b.received == 1); // b still attached

  s.emit(e);                // Emit again, only b should receive
  REQUIRE(a.received == 1); // No further increments
  REQUIRE(b.received == 2); // Still working normally
}