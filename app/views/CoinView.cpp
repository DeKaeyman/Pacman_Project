// app/views/CoinView.cpp
#include "CoinView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../logic/observer/Event.h"

namespace pacman::app {

    sf::Texture CoinView::texture_{}; // Static shared texture across all CoinViews
    bool CoinView::textureLoaded_{false}; // Tracks if the texture has been loaded

    namespace { // Sprite sheet coordinates for coin tile
        constexpr int COIN_SPRITE_X = 400;
        constexpr int COIN_SPRITE_Y = 400;
        constexpr int COIN_SPRITE_W = 16;
        constexpr int COIN_SPRITE_H = 16;

        constexpr const char* SPRITE_SHEET_PATH = "assets/sprites/sprite.png"; // Path to sprite sheet asset
    }

    void CoinView::ensureTextureLoaded() {
        if (textureLoaded_) return; // Already loaded
        if (!texture_.loadFromFile(SPRITE_SHEET_PATH)) return; // Attempt to load sprite sheet
        textureLoaded_ = true; // Mark texture as loaded
    }

    CoinView::CoinView(const std::shared_ptr<pacman::logic::Coin> &model) : model_(model) {
        ensureTextureLoaded(); // Ensure shared texture is available
        if(textureLoaded_) { // Setup sprite only if texture is loaded
            sprite_.setTexture(texture_); // Assign sprite sheet texture
            sprite_.setTextureRect(sf::IntRect(COIN_SPRITE_X, COIN_SPRITE_Y, COIN_SPRITE_W, COIN_SPRITE_H)); // Select only the coin's region
        }
    }

    void CoinView::onEvent(const pacman::logic::Event &e) {
        if (e.type == pacman::logic::EventType::Collected) { // Coin has been collected
            visible_ = false; // Stop drawing this coin forever
        }
    }

    void CoinView::draw(sf::RenderWindow &window) {
        if (!visible_) return; // Skip drawing if already collected
        if (!model_) return; // No model -> nothing to draw
        if (!model_->active) return; // Inactive coin -> don't display
        if (!camera_) return; // Camera required for world -> pixel mapping
        if (!textureLoaded_) return; // No texture loaded -> skip drawing

        pacman::logic::Rect r = model_->bounds(); // Get coin's world space rectangle
        auto pixelRect = camera_->worldToPixel(r); // Convert to pixel coordinates

        const auto& texRect = sprite_.getTextureRect(); // Get original sprite's dimensions
        float scaleX = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width); // Fit sprite width
        float scaleY = static_cast<float>(pixelRect.h) / static_cast<float>(texRect.height); // Fit sprite height

        sprite_.setPosition(static_cast<float>(pixelRect.x), static_cast<float>(pixelRect.y)); // Position on screen
        sprite_.setScale(scaleX, scaleY); // Scale sprite to tile size

        window.draw(sprite_); // Final draw call
    }
}