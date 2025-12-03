#pragma once
#include "State.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace pacman::app {

class StateManager {
public:
  using Id = std::string;
  using Factory = std::function<std::unique_ptr<State>(
      StateManager &)>; // A factory is a function that creates a State object

  void registerFactory(
      const Id &,
      Factory f); // Registers a state type and id so the manager can build it

  // Management of the stack
  void push(const Id &id);
  void replace(const Id &id);
  void pop();
  void clear();

  // Logic
  void handleEvent(const sf::Event &e);
  void update(double dt);
  void draw(sf::RenderWindow &w);

  bool empty() const { return stack_.empty(); }
  std::size_t size() const { return stack_.size(); }

private:
  std::unique_ptr<State>
  make(const Id &id); // Help function, creates a state via the correct factory
                      // based on id

  std::vector<std::unique_ptr<State>>
      stack_; // The actual stack, latest element is active state
  std::unordered_map<Id, Factory>
      factories_; // Registry of available state types based on the name
};
} // namespace pacman::app