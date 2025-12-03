// app/views/CoinView.h
#pragma once

#include "../logic/entities/Coin.h"
#include "View.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pacman::app {

class CoinView : public View {
public:
  explicit CoinView(const std::shared_ptr<pacman::logic::Coin>
                        &model); // Stores model and sets up sprite

  void draw(sf::RenderWindow &window)
      override; // Draws the coin using world -> pixel convertion
  void onEvent(const pacman::logic::Event &e)
      override; // Responds to coin collection events

private:
  std::shared_ptr<pacman::logic::Coin>
      model_;          // Logical model this view represents
  sf::Sprite sprite_;  // Visual sprite for the Coin
  bool visible_{true}; // Whether this coin should be drawn

  static sf::Texture texture_; // Shared sprite sheet
  static bool textureLoaded_;  // Tracks if texture_ has been loaded

  static void ensureTextureLoaded(); // Lazy loader for texture_
};
} // namespace pacman::app