// app/views/FruitView.cpp
#include "FruitView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include "../logic/observer/Event.h"

namespace pacman::app {

    sf::Texture FruitView::texture_{}; // Shared static texture
    bool FruitView::textureLoaded_{false}; // Prevents repeated loading

    namespace { // Fruit sprite selection in sprite sheet
        constexpr const char *SPRITE_SHEET_PATH = "assets/sprites/sprite.png"; // Asset location

        constexpr unsigned int SHEET_COLS = 19;
        constexpr unsigned int SHEET_ROWS = 19;

        // Fruit grid position (0-based col,row)
        constexpr unsigned int FRUIT_COL = 11;
        constexpr unsigned int FRUIT_ROW = 11;

        sf::IntRect spriteRectFromGrid(const sf::Texture& tex, unsigned int col, unsigned int row) {
            const auto size = tex.getSize(); // Full sheet size in pixels
            const float cellW = static_cast<float>(size.x) / static_cast<float>(SHEET_COLS); // Width of one grid cell
            const float cellH = static_cast<float>(size.y) / static_cast<float>(SHEET_ROWS); // Height of one grid cell

            const int left   = static_cast<int>(std::round(col * cellW));
            const int top    = static_cast<int>(std::round(row * cellH));
            const int right  = static_cast<int>(std::round((col + 1u) * cellW));
            const int bottom = static_cast<int>(std::round((row + 1u) * cellH));

            return { left, top, right - left, bottom - top }; // Sprite rectangle
        }
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
            sprite_.setTextureRect(spriteRectFromGrid(texture_, FRUIT_COL, FRUIT_ROW)); // Select fruit region
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

        pacman::logic::Rect r = model_->bounds(); // Get logic bounding box
        auto pixelRect = camera_->worldToPixel(r); // Convert to pixel-space rectangle

        const auto& texRect = sprite_.getTextureRect(); // Current sprite size in texture

        float scale = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width); // Uniform scale so coin fits tile

        float finalW = texRect.width  * scale; // Pixel width after scaling
        float finalH = texRect.height * scale; // Pixel height after scaling

        float posX = static_cast<float>(pixelRect.x) + (pixelRect.w - finalW) * 0.5f; // Center horizontally
        float posY = static_cast<float>(pixelRect.y) + (pixelRect.h - finalH) * 0.5f; // Center vertically

        sprite_.setPosition(posX, posY); // Put sprite on screen
        sprite_.setScale(scale, scale); // Apply scaling

        window.draw(sprite_); // Draw final coin sprite
    }
};