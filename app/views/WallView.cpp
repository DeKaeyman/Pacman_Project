#include "WallView.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

    void WallView::draw(sf::RenderWindow& window) {
        if (!model_ || !camera_) return;

        const auto worldRect  = model_->bounds();
        const auto pixelRect  = camera_->worldToPixel(worldRect);

        sf::RectangleShape shape;
        shape.setPosition(static_cast<float>(pixelRect.x), static_cast<float>(pixelRect.y));
        shape.setSize(sf::Vector2f(static_cast<float>(pixelRect.w), static_cast<float>(pixelRect.h)));

        shape.setFillColor(sf::Color(0, 0, 255));

        window.draw(shape);
    }

}