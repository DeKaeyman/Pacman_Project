#pragma once
#include "State.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

namespace pacman::app {

    class PausedState : public State {
    public:
        using State::State;

        void handleEvent(const sf::Event& e) override;
        void draw(sf::RenderWindow& w) override;

    private:
        void init(const sf::RenderWindow& w);
        void layout(const sf::RenderWindow& w);
        bool hitButton(std::size_t i, float mx, float my) const;

    private:
        sf::Font font_;
        sf::Text title_;
        bool initialized_ = false;

        sf::RectangleShape overlay_;

        struct Button {
            sf::RectangleShape rect;
            sf::Text label;
        };

        std::vector<Button> buttons_; // 0=Resume,1=Restart,2=Menu
    };

} // namespace pacman::app