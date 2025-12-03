// logic/entities/Fruit.h
#pragma once

#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

class Fruit : public Entity, public Subject {
public:
  explicit Fruit(const Rect &area,
                 int value = 50); // Defines fruit bounds + score value

  Rect bounds() const override {
    return area_;
  }                                      // World bounds for rendering/collision
  void update(double /*dt*/) override {} // Fruit has no per-frame logic

  int value() const noexcept {
    return value_;
  } // Score value returned on collection
  void setBounds(const Rect &r) { area_ = r; } // Allows repositioning
  bool isCollected() const noexcept {
    return !active;
  } // Collected once inactive

  void collect(); // Marks fruit collected + notifies observers

private:
  Rect area_{};   // World rectangle of the fruit
  int value_{50}; // Default fruit score
};
} // namespace pacman::logic