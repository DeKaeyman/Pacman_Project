#pragma once

#include "View.h"

#include "../logic/entities/Wall.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include <memory>

namespace pacman::app {

/**
 * @brief View responsible for rendering a wall entity.
 *
 * Walls are static objects and do not react to logic-side events.
 */
class WallView : public View {
public:
    /**
     * @brief Constructs a WallView bound to the given wall model.
     * @param model Shared pointer to the wall logic entity.
     */
    explicit WallView(const std::shared_ptr<pacman::logic::Wall>& model);

    /**
     * @brief Draws the wall representation.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window) override;

    /**
     * @brief Ignores logic events, as walls have no dynamic behavior.
     * @param event Incoming logic event.
     */
    void onEvent(const pacman::logic::Event& event) override;

private:
    std::shared_ptr<pacman::logic::Wall> model_;
};

} // namespace pacman::app