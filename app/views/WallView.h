#pragma once

#include "View.h"
#include "../logic/entities/Wall.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace pacman::app {

    class WallView : public View {
    public:
        explicit WallView(const std::shared_ptr<pacman::logic::Wall>& model) : model_(model) {}

        void draw(sf::RenderWindow& window) override;
        void onEvent(const pacman::logic::Event&) override {}

    private:
        std::shared_ptr<pacman::logic::Wall> model_;
    };

}