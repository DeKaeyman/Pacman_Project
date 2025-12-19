#pragma once

#include "View.h"

#include "../logic/entities/Fruit.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>

namespace pacman::app {

/**
 * @brief View responsible for rendering a fruit entity and reacting to collection events.
 */
class FruitView : public View {
public:
    /**
     * @brief Constructs a FruitView bound to the given fruit model.
     * @param model Shared pointer to the fruit logic entity.
     */
    explicit FruitView(const std::shared_ptr<pacman::logic::Fruit>& model);

    /**
     * @brief Draws the fruit using world-to-pixel mapping and sprite scaling.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window) override;

    /**
     * @brief Responds to logic events (e.g., fruit collected).
     * @param event Incoming logic event.
     */
    void onEvent(const pacman::logic::Event& event) override;

private:
    std::shared_ptr<pacman::logic::Fruit> model_;
    sf::Sprite sprite_;
    bool visible_{true};

    static sf::Texture texture_;
    static bool textureLoaded_;

    /**
     * @brief Loads the shared sprite sheet texture on first use.
     */
    static void ensureTextureLoaded();
};

} // namespace pacman::app