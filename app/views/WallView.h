#pragma once

#include "../logic/entities/Wall.h"
#include "View.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace pacman::app {

class WallView : public View {
public:
  explicit WallView(const std::shared_ptr<pacman::logic::Wall> &model)
      : model_(model) {} // Store wall model so we can querry its bounds

  void
  draw(sf::RenderWindow &window) override; // Draw rectangle representing wall
  void onEvent(const pacman::logic::Event &) override {
  } // Walls never emit events

private:
  std::shared_ptr<pacman::logic::Wall> model_; // Logical wall entity
};

} // namespace pacman::app