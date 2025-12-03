#pragma once

#include "../logic/entities/Direction.h"
#include "../logic/entities/PacMan.h"
#include "../logic/observer/Event.h"
#include "../logic/utils/Stopwatch.h"
#include "View.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pacman::app {

class PacManView : public View {
public:
  explicit PacManView(const std::shared_ptr<pacman::logic::PacMan>
                          &model); // Store model and init sprite

  void draw(sf::RenderWindow &window)
      override; // Draw Pacman with correct frame and position
  void onEvent(const pacman::logic::Event &e)
      override; // React to direction/state changes from model

private:
  std::shared_ptr<pacman::logic::PacMan>
      model_;         // Pacman model this view follows
  sf::Sprite sprite_; // Visual sprite for Pacman
  logic::Direction direction_{
      logic::Direction::None}; // current facing direction for sprite selection

  static sf::Texture texture_; // Shared texture across all Pacman views
  static bool textureLoaded_;  // Guard flag to ensure we load texture only once

  static void ensureTextureLoaded(); // loads sprite sheet if not yet loaded
  void updateSpriteFrame(); // Picks correct animation frame based on direction
                            // + time
};
} // namespace pacman::app