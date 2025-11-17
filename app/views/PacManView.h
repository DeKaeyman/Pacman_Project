#pragma once

#include "View.h"
#include "../logic/entities/PacMan.h"
#include "../logic/observer/Event.h"
#include "../logic/utils/Stopwatch.h"
#include "../logic/entities/Direction.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pacman::app {

    class PacManView : public View {
    public:
        explicit PacManView(const std::shared_ptr<pacman::logic::PacMan>& model);

        void draw(sf::RenderWindow& window) override;
        void onEvent(const pacman::logic::Event& e) override;

    private:
        std::shared_ptr<pacman::logic::PacMan> model_;
        sf::Sprite sprite_;
        logic::Direction direction_{logic::Direction::Right};

        static sf::Texture texture_;
        static bool textureLoaded_;

        static void ensureTextureLoaded();
        void updateSpriteFrame();
    };
}