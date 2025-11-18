// app/views/CoinView.cpp
#include "CoinView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include "../logic/observer/Event.h"

namespace pacman::app {

    sf::Texture CoinView::texture_{}; // Static shared texture across all CoinViews
    bool CoinView::textureLoaded_{false}; // Tracks if the texture has been loaded

    namespace { // Sprite sheet coordinates for coin tile
        constexpr const char* SPRITE_SHEET_PATH = "assets/sprites/sprite.png"; // Path to sprite sheet asset

        // Sprite sheet is 19 x 19 cells.
        constexpr unsigned int SHEET_COLS = 19;
        constexpr unsigned int SHEET_ROWS = 19;

        // Coin position in the grid (0-based indices).
        constexpr unsigned int COIN_COL = 8;
        constexpr unsigned int COIN_ROW = 6;

        sf::IntRect spriteRectFromGrid(const sf::Texture& tex, unsigned int col, unsigned int row) {
            const auto size = tex.getSize();
            const float cellW = static_cast<float>(size.x) / static_cast<float>(SHEET_COLS);
            const float cellH = static_cast<float>(size.y) / static_cast<float>(SHEET_ROWS);

            const int left = static_cast<int>(std::round(col * cellW));
            const int top = static_cast<int>(std::round(row * cellH));
            const int right = static_cast<int>(std::round((col + 1u) * cellW));
            const int bottom = static_cast<int>(std::round((row + 1u) * cellH));

            return {left, top, right - left, bottom - top};
        }
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
            sprite_.setTextureRect(spriteRectFromGrid(texture_, COIN_COL, COIN_ROW)); // Select only the coin's region
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

        pacman::logic::Rect r = model_->bounds();
        auto pixelRect = camera_->worldToPixel(r);

        const auto& texRect = sprite_.getTextureRect();

        float scale = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width);

        float finalW = texRect.width  * scale;
        float finalH = texRect.height * scale;

        float posX = static_cast<float>(pixelRect.x) + (pixelRect.w - finalW) * 0.5f;
        float posY = static_cast<float>(pixelRect.y) + (pixelRect.h - finalH) * 0.5f;

        sprite_.setPosition(posX, posY);
        sprite_.setScale(scale, scale);

        window.draw(sprite_);
    }
}