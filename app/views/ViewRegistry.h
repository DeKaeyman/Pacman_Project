#pragma once
#include "View.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <vector>

namespace pacman::app {

class ViewRegistry { // Owns and batches all views for drawing
public:
  using Ptr = std::unique_ptr<View>;

  void add(Ptr v) { // Register a new view into the registry
    if (v)
      views_.push_back(std::move(v)); // Store only non-null views
  }

  void
  drawAll(sf::RenderWindow &w) { // Draw all registered views into the window
    for (auto &v : views_)
      v->draw(w); // Delegate draw to each view
  }

  std::vector<Ptr> &raw() { return views_; } // Expose container for management

private:
  std::vector<Ptr> views_; // Storage for all owned views
};
} // namespace pacman::app