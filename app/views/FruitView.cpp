// app/views/FruitView.cpp
#include "FruitView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../logic/observer/Event.h"

namespace pacman::app {

    sf::Texture FruitView::texture_{}; // Shared static texture
    bool FruitView::textureLoaded_{false}; // Prevents repeated loading

    namespace { // Fruit sprite selection in sprite sheet
        constexpr int FRUIT_SPRITE_X = 450;
        constexpr int FRUIT_SPRITE_Y = 400;
        constexpr int FRUIT_SPRITE_W = 16;
        constexpr int FRUIT_SPRITE_H = 16;

        constexpr const char *SPRITE_SHEET_PATH = "assets/sprites/sprite.png"; // Asset location
    }

    void FruitView::ensureTextureLoaded() {
        if (textureLoaded_) return; // Load only once
        if (!texture_.loadFromFile(SPRITE_SHEET_PATH)) return; // Load file, abort if failure
        textureLoaded_ = true; // Mark loaded
    }

    FruitView::FruitView(const std::shared_ptr<pacman::logic::Fruit> &model) : model_(model) {
        ensureTextureLoaded(); // Make sure texture exists
        if(textureLoaded_) { // Configure sprite only if OK
            sprite_.setTexture(texture_); // Use shared texture
            sprite_.setTextureRect(sf::IntRect(FRUIT_SPRITE_X, FRUIT_SPRITE_Y, FRUIT_SPRITE_W, FRUIT_SPRITE_H)); // Select fruit region
        }
    }

    void FruitView::onEvent(const pacman::logic::Event& e) {
        if (e.type == pacman::logic::EventType::Collected) { // Fruit eaten
            visible_ = false; // Hide fruit permanently
        }
    }

    void FruitView::draw(sf::RenderWindow& window) {
        if (!visible_) return; // Hidden after collection
        if (!model_) return; // No model -> can't draw
        if (!model_->active) return; // Inactive fruit -> skip
        if (!camera_) return; // Need camera for world -> pixel transform
        if (!textureLoaded_) return; // No texture loaded -> skip draw

        pacman::logic::Rect r = model_->bounds(); // World coordinates of fruit
        auto pixelRect = camera_->worldToPixel(r); // Convert to screen coordinates

        const auto& texRect = sprite_.getTextureRect(); // Size of sprite frame
        float scaleX = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width); // Scale X fit
        float scaleY = static_cast<float>(pixelRect.h) / static_cast<float>(texRect.height); // Scale Y fit

        sprite_.setPosition(static_cast<float>(pixelRect.x), static_cast<float>(pixelRect.y)); // Position on screen
        sprite_.setScale(scaleX, scaleY); // Apply scaling

        window.draw(sprite_); // Draw sprite
    }
};