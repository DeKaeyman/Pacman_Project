#pragma once

#include "View.h"

#include "../logic/entities/Coin.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>

namespace pacman::app {

    /**
     * @brief View responsible for rendering a coin entity and reacting to collection events.
     */
    class CoinView : public View {
    public:
        /**
         * @brief Constructs a CoinView bound to the given coin model.
         * @param model Shared pointer to the coin logic entity.
         */
        explicit CoinView(const std::shared_ptr<pacman::logic::Coin>& model);

        /**
         * @brief Draws the coin using world-to-pixel mapping and sprite scaling.
         * @param window The render window to draw to.
         */
        void draw(sf::RenderWindow& window) override;

        /**
         * @brief Responds to logic events (e.g., coin collected).
         * @param event Incoming logic event.
         */
        void onEvent(const pacman::logic::Event& event) override;

    private:
        std::shared_ptr<pacman::logic::Coin> model_;
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