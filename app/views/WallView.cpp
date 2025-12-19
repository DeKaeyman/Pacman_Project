#include "WallView.h"

#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

/**
 * @brief Constructs a WallView bound to the given wall model.
 * @param model Shared pointer to the wall logic entity.
 */
    WallView::WallView(const std::shared_ptr<pacman::logic::Wall>& model)
            : model_(model) {}

/**
 * @brief Ignores logic events, as walls have no dynamic behavior.
 * @param event Incoming logic event.
 */
    void WallView::onEvent(const pacman::logic::Event& event) {
        (void)event;
    }

/**
 * @brief Draws the wall as a filled rectangle in world space.
 * @param window The render window to draw to.
 */
    void WallView::draw(sf::RenderWindow& window) {
        if (!model_ || !camera_) {
            return;
        }

        if (!model_->active || !model_->visible) {
            return;
        }

        const auto worldRect = model_->bounds();
        const auto pixelRect = camera_->worldToPixel(worldRect);

        sf::RectangleShape shape;
        shape.setPosition(static_cast<float>(pixelRect.x), static_cast<float>(pixelRect.y));
        shape.setSize(sf::Vector2f(static_cast<float>(pixelRect.w),
                                   static_cast<float>(pixelRect.h)));
        shape.setFillColor(sf::Color(0, 0, 255));

        window.draw(shape);
    }

} // namespace pacman::app