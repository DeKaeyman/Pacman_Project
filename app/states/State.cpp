#include "State.h"
#include "StateManager.h"

namespace pacman::app {

void State::push(const Id &id) {
  manager_.push(id);
} // Push a new state on top of the stack
void State::replace(const Id &id) {
  manager_.replace(id);
} // Replace the current top state by a new one
void State::pop() {
  manager_.pop();
} // Remove the current state from the stack and go back to the previous
void State::clear() {
  manager_.clear();
} // Remove all the states from the stack
} // namespace pacman::app