#pragma once

#include "View.h"

#include "../logic/entities/Direction.h"
#include "../logic/entities/PacMan.h"
#include "../logic/observer/Event.h"
#include "../logic/utils/Stopwatch.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>

namespace pacman::app {

    /**
     * @brief View responsible for rendering the Pac-Man entity and reacting to direction changes.
     */
    class PacManView : public View {
    public:
        /**
         * @brief Constructs a PacManView bound to the given Pac-Man model.
         * @param model Shared pointer to the Pac-Man logic entity.
         */
        explicit PacManView(const std::shared_ptr<pacman::logic::PacMan>& model);

        /**
         * @brief Draws Pac-Man using world-to-pixel mapping and time-based animation.
         * @param window The render window to draw to.
         */
        void draw(sf::RenderWindow& window) override;

        /**
         * @brief Reacts to logic events (direction/state changes).
         * @param event Incoming logic event.
         */
        void onEvent(const pacman::logic::Event& event) override;

    private:
        /**
         * @brief Loads the shared sprite sheet texture on first use.
         */
        static void ensureTextureLoaded();

        /**
         * @brief Updates the sprite frame selection based on direction and elapsed time.
         */
        void updateSpriteFrame();

    private:
        std::shared_ptr<pacman::logic::PacMan> model_;
        sf::Sprite sprite_;
        logic::Direction direction_{logic::Direction::None};

        static sf::Texture texture_;
        static bool textureLoaded_;
    };

} // namespace pacman::app