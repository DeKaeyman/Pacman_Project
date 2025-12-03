// app/views/GhostView.h
#pragma once

#include "../logic/entities/Direction.h"
#include "../logic/entities/Ghost.h"
#include "../logic/observer/Event.h"
#include "../logic/utils/Stopwatch.h"
#include "View.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pacman::app {

class GhostView : public View {
public:
  explicit GhostView(const std::shared_ptr<pacman::logic::Ghost>
                         &model); // Store model + load texture

  void draw(sf::RenderWindow &window) override; // Draw ghost sprite on screen
  void onEvent(const pacman::logic::Event &e)
      override; // React to movement/direction/fear event

private:
  std::shared_ptr<pacman::logic::Ghost> model_; // Ghost model this view follows
  sf::Sprite sprite_;                           // Visual sprite for the ghost

  pacman::logic::Direction direction_{
      pacman::logic::Direction::None}; // Current facing direction
  bool fearMode_{false};               // Whether ghost is in blue blinking mode

  static sf::Texture texture_; // Shared texture across all ghost views
  static bool textureLoaded_;  // Guard to load texture only once

  static void ensureTextureLoaded(); // Load sprite sheet if not loaded yet
  void updateSpriteFrame(); // Select correct frame based on direction + time +
                            // fear mode
};
} // namespace pacman::app