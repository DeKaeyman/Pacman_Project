#pragma once

#include "View.h"

#include "../logic/entities/Direction.h"
#include "../logic/entities/Ghost.h"
#include "../logic/observer/Event.h"
#include "../logic/utils/Stopwatch.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>

namespace pacman::app {

    /**
     * @brief View responsible for rendering a ghost entity and reacting to its state changes.
     */
    class GhostView : public View {
    public:
        /**
         * @brief Constructs a GhostView bound to the given ghost model.
         * @param model Shared pointer to the ghost logic entity.
         */
        explicit GhostView(const std::shared_ptr<pacman::logic::Ghost>& model);

        /**
         * @brief Draws the ghost sprite using world-to-pixel mapping and animation frame selection.
         * @param window The render window to draw to.
         */
        void draw(sf::RenderWindow& window) override;

        /**
         * @brief Reacts to logic events (direction changes and fear mode toggles).
         * @param event Incoming logic event.
         */
        void onEvent(const pacman::logic::Event& event) override;

    private:
        /**
         * @brief Loads the shared sprite sheet texture on first use.
         */
        static void ensureTextureLoaded();

        /**
         * @brief Updates the sprite frame selection based on direction, fear mode, and elapsed time.
         */
        void updateSpriteFrame();

    private:
        std::shared_ptr<pacman::logic::Ghost> model_;
        sf::Sprite sprite_;

        pacman::logic::Direction direction_{pacman::logic::Direction::None};
        bool fearMode_{false};

        static sf::Texture texture_;
        static bool textureLoaded_;
    };

} // namespace pacman::app