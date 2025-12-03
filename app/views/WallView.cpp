#include "WallView.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

void WallView::draw(sf::RenderWindow &window) {
  if (!model_ || !camera_)
    return; // Can't draw without model or camera

  const auto worldRect = model_->bounds(); // Logic bounding box of the wall
  const auto pixelRect =
      camera_->worldToPixel(worldRect); // Convert to pixel coordinates

  sf::RectangleShape shape; // Simple rectangle to show wall tile
  shape.setPosition(static_cast<float>(pixelRect.x),
                    static_cast<float>(pixelRect.y)); // move to tile position
  shape.setSize(
      sf::Vector2f(static_cast<float>(pixelRect.w),
                   static_cast<float>(pixelRect.h))); // Match tile size

  shape.setFillColor(sf::Color(0, 0, 255)); // Blue block

  window.draw(shape); // Render rectangle
}

} // namespace pacman::app