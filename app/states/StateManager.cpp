#include "StateManager.h"
#include "State.h"
#include <stdexcept>

namespace pacman::app {

void StateManager::registerFactory(const Id &id, Factory f) {
  factories_[id] =
      std::move(f); // Registers a construction function for a given ID
}

std::unique_ptr<State> StateManager::make(const Id &id) {
  auto it = factories_.find(id);
  if (it == factories_.end())
    throw std::runtime_error("Unknown state id: " +
                             id); // If id not found, return error message
  return (it->second)(
      *this); // Calls the saved factory function to create a new state
}

void StateManager::push(const Id &id) {
  stack_.push_back(make(id)); // Make a new state via the factory and push them
                              // on top of the stack
}

void StateManager::replace(const Id &id) {
  if (!stack_.empty())
    stack_.pop_back(); // Removes the current state
  stack_.push_back(make(
      id)); // Place the new state on top of the stack via the make function
}

void StateManager::pop() {
  if (!stack_.empty())
    stack_.pop_back(); // Remove the top state and return to the previous one
}

void StateManager::clear() {
  stack_.clear(); // Clear the full stack
}

void StateManager::handleEvent(const sf::Event &e) {
  if (!stack_.empty())
    stack_.back()->handleEvent(
        e); // Send the user input to the current top stack
}

void StateManager::update(double dt) {
  if (!stack_.empty())
    stack_.back()->update(dt); // Only allow the top state to execute logic
}

void StateManager::draw(sf::RenderWindow &w) {
  if (!stack_.empty())
    stack_.back()->draw(w); // only draw the top state
}
} // namespace pacman::app