// PausedState.cpp
#include "PausedState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace pacman::app {

    namespace {
        constexpr float BTN_W = 260.f;
        constexpr float BTN_H = 64.f;
        constexpr float BTN_GAP = 18.f;
    } // namespace

    void PausedState::init(const sf::RenderWindow& w) {
        if (initialized_)
            return;
        initialized_ = true;

        font_.loadFromFile("assets/fonts/Crackman.otf");

        title_.setFont(font_);
        title_.setString("PAUSED");
        title_.setCharacterSize(72);
        title_.setFillColor(sf::Color::Yellow);
        title_.setOutlineThickness(3.f);
        title_.setOutlineColor(sf::Color::Black);

        overlay_.setFillColor(sf::Color(0, 0, 0, 170));

        buttons_.resize(3);

        const char* labels[3] = {"RESUME", "RESTART", "MENU"};
        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            buttons_[i].rect.setSize({BTN_W, BTN_H});
            buttons_[i].rect.setFillColor(sf::Color(30, 30, 30, 220));
            buttons_[i].rect.setOutlineThickness(3.f);
            buttons_[i].rect.setOutlineColor(sf::Color::White);

            buttons_[i].label.setFont(font_);
            buttons_[i].label.setString(labels[i]);
            buttons_[i].label.setCharacterSize(32);
            buttons_[i].label.setFillColor(sf::Color::White);
        }

        layout(w);
    }

    void PausedState::layout(const sf::RenderWindow& w) {
        // Overlay size (Vector2u -> Vector2f)
        const auto ws = w.getSize();
        overlay_.setSize(sf::Vector2f{static_cast<float>(ws.x), static_cast<float>(ws.y)});

        auto centerTextX = [](sf::Text& t) {
            auto b = t.getLocalBounds();
            t.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        };

        // Title position
        centerTextX(title_);
        title_.setPosition(static_cast<float>(ws.x) * 0.5f, static_cast<float>(ws.y) * 0.25f);

        // Buttons block
        const float cx = static_cast<float>(ws.x) * 0.5f;
        const float startY = static_cast<float>(ws.y) * 0.45f;

        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            auto& b = buttons_[i];

            b.rect.setOrigin(BTN_W * 0.5f, BTN_H * 0.5f);
            b.rect.setPosition(cx, startY + i * (BTN_H + BTN_GAP));

            centerTextX(b.label);
            b.label.setPosition(b.rect.getPosition().x, b.rect.getPosition().y - 2.f);
        }
    }

    bool PausedState::hitButton(std::size_t i, float mx, float my) const {
        const auto& r = buttons_[i].rect;
        const auto p = r.getPosition();
        const auto o = r.getOrigin();
        const auto s = r.getSize();

        const float left = p.x - o.x;
        const float top = p.y - o.y;
        const float right = left + s.x;
        const float bottom = top + s.y;

        return (mx >= left && mx <= right && my >= top && my <= bottom);
    }

    void PausedState::handleEvent(const sf::Event& e) {
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
            pop(); // resume
            return;
        }

        if (e.type == sf::Event::Resized) {
            // Not strictly needed if your Game already handles camera,
            // but we DO need to re-layout the UI.
            // We can't access window here, so do it in draw() each frame.
            return;
        }

        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            const float mx = static_cast<float>(e.mouseButton.x);
            const float my = static_cast<float>(e.mouseButton.y);

            // Resume
            if (hitButton(0, mx, my)) {
                pop();
                return;
            }

            // Restart: remove Pause + Level, then start fresh Level
            if (hitButton(1, mx, my)) {
                pop(); // Paused
                pop(); // Level
                push("level");
                return;
            }

            // Menu: remove Pause + Level, then go Menu
            if (hitButton(2, mx, my)) {
                pop(); // Paused
                pop(); // Level
                push("menu");
                return;
            }
        }
    }

    void PausedState::draw(sf::RenderWindow& w) {
        init(w);

        // Ensure UI follows window size changes
        layout(w);

        w.draw(overlay_);
        w.draw(title_);

        // Hover effect (optional but nice)
        const auto mp = sf::Mouse::getPosition(w);
        const float mx = static_cast<float>(mp.x);
        const float my = static_cast<float>(mp.y);

        for (std::size_t i = 0; i < buttons_.size(); ++i) {
            auto& b = buttons_[i];

            if (hitButton(i, mx, my)) {
                b.rect.setFillColor(sf::Color(70, 70, 70, 230));
            } else {
                b.rect.setFillColor(sf::Color(30, 30, 30, 220));
            }

            w.draw(b.rect);
            w.draw(b.label);
        }
    }

} // namespace pacman::app