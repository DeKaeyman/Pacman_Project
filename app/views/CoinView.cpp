#include "CoinView.h"

#include "../logic/observer/Event.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <cmath>

namespace pacman::app {

sf::Texture CoinView::texture_{};
bool CoinView::textureLoaded_{false};

namespace {
constexpr const char* kSpriteSheetPath = "assets/sprites/sprite.png";

constexpr unsigned int kSheetCols = 19;
constexpr unsigned int kSheetRows = 19;

constexpr unsigned int kCoinCol = 8;
constexpr unsigned int kCoinRow = 6;

/**
 * @brief Computes an SFML texture rectangle from a grid-based sprite sheet.
 * @param texture The sprite sheet texture.
 * @param col Zero-based column index in the sheet grid.
 * @param row Zero-based row index in the sheet grid.
 * @return The corresponding sub-rectangle in pixel coordinates.
 */
sf::IntRect spriteRectFromGrid(const sf::Texture& texture, unsigned int col, unsigned int row) {
    const auto size = texture.getSize();
    const float cellW = static_cast<float>(size.x) / static_cast<float>(kSheetCols);
    const float cellH = static_cast<float>(size.y) / static_cast<float>(kSheetRows);

    const int left = static_cast<int>(std::round(static_cast<float>(col) * cellW));
    const int top = static_cast<int>(std::round(static_cast<float>(row) * cellH));
    const int right = static_cast<int>(std::round(static_cast<float>(col + 1u) * cellW));
    const int bottom = static_cast<int>(std::round(static_cast<float>(row + 1u) * cellH));

    return {left, top, right - left, bottom - top};
}
} // namespace

/**
 * @brief Ensures the shared coin sprite texture is loaded once.
 *
 * If loading fails, the view will simply not render (matching current behavior).
 */
void CoinView::ensureTextureLoaded() {
    if (textureLoaded_) {
        return;
    }

    if (!texture_.loadFromFile(kSpriteSheetPath)) {
        return;
    }

    textureLoaded_ = true;
}

/**
 * @brief Constructs a CoinView bound to a coin model and prepares its sprite.
 * @param model Shared pointer to the coin logic entity.
 */
CoinView::CoinView(const std::shared_ptr<pacman::logic::Coin>& model) : model_(model) {
    ensureTextureLoaded();

    if (textureLoaded_) {
        sprite_.setTexture(texture_);
        sprite_.setTextureRect(spriteRectFromGrid(texture_, kCoinCol, kCoinRow));
    }
}

/**
 * @brief Handles logic events emitted by the coin.
 * @param event Incoming logic event.
 */
void CoinView::onEvent(const pacman::logic::Event& event) {
    if (event.type == pacman::logic::EventType::Collected) {
        visible_ = false;
    }
}

/**
 * @brief Draws the coin sprite centered inside its tile.
 *
 * Rendering is skipped if the coin is not visible, inactive, missing a model/camera,
 * or if the sprite sheet texture failed to load.
 *
 * @param window The render window to draw to.
 */
void CoinView::draw(sf::RenderWindow& window) {
    if (!visible_ || !model_ || !model_->active || !camera_ || !textureLoaded_) {
        return;
    }

    const pacman::logic::Rect worldRect = model_->bounds();
    const auto pixelRect = camera_->worldToPixel(worldRect);

    const auto texRect = sprite_.getTextureRect();
    const float scale = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width);

    const float finalW = static_cast<float>(texRect.width) * scale;
    const float finalH = static_cast<float>(texRect.height) * scale;

    const float posX = static_cast<float>(pixelRect.x) + (static_cast<float>(pixelRect.w) - finalW) * 0.5f;
    const float posY = static_cast<float>(pixelRect.y) + (static_cast<float>(pixelRect.h) - finalH) * 0.5f;

    sprite_.setPosition(posX, posY);
    sprite_.setScale(scale, scale);

    window.draw(sprite_);
}

} // namespace pacman::app