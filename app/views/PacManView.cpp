#include "PacManView.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <cmath>

namespace pacman::app {

sf::Texture PacManView::texture_{};
bool PacManView::textureLoaded_{false};

namespace {
constexpr const char* kSpriteSheetPath = "../assets/sprites/sprite.png";

constexpr unsigned int kSheetCols = 19;
constexpr unsigned int kSheetRows = 19;

constexpr unsigned int kPacManCol = 17;

constexpr unsigned int kRowClosed = 0;
constexpr unsigned int kRowRightSmall = 1;
constexpr unsigned int kRowRightBig = 2;
constexpr unsigned int kRowDownSmall = 4;
constexpr unsigned int kRowDownBig = 5;
constexpr unsigned int kRowLeftSmall = 7;
constexpr unsigned int kRowLeftBig = 8;
constexpr unsigned int kRowUpSmall = 10;
constexpr unsigned int kRowUpBig = 11;

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

/**
 * @brief Selects the appropriate animation row for Pac-Man based on direction and elapsed time.
 * @param dir Current facing direction.
 * @param t Elapsed time used to animate the mouth.
 * @return Sprite sheet row index representing the chosen frame.
 */
unsigned int pickRowFor(logic::Direction dir, double t) {
    const double speed = 10.0;
    const double phase = std::fmod(t * speed, 4.0);

    const bool useClosed = (phase < 1.0);
    const bool useSmall = (phase >= 1.0 && phase < 2.5);
    const bool useBig = (phase >= 2.5 && phase < 3.5);

    if (useClosed) {
        return kRowClosed;
    }

    switch (dir) {
    case logic::Direction::Right:
        return useBig ? kRowRightBig : (useSmall ? kRowRightSmall : kRowClosed);
    case logic::Direction::Down:
        return useBig ? kRowDownBig : (useSmall ? kRowDownSmall : kRowClosed);
    case logic::Direction::Left:
        return useBig ? kRowLeftBig : (useSmall ? kRowLeftSmall : kRowClosed);
    case logic::Direction::Up:
        return useBig ? kRowUpBig : (useSmall ? kRowUpSmall : kRowClosed);
    case logic::Direction::None:
    default:
        return kRowClosed;
    }
}
} // namespace

/**
 * @brief Loads the shared sprite sheet texture on first use.
 *
 * If loading fails, rendering will be skipped (matching current behavior).
 */
void PacManView::ensureTextureLoaded() {
    if (textureLoaded_) {
        return;
    }

    if (!texture_.loadFromFile(kSpriteSheetPath)) {
        return;
    }

    textureLoaded_ = true;
}

/**
 * @brief Constructs a PacManView bound to the given Pac-Man model and prepares its sprite.
 * @param model Shared pointer to the Pac-Man logic entity.
 */
PacManView::PacManView(const std::shared_ptr<pacman::logic::PacMan>& model) : model_(model) {
    ensureTextureLoaded();

    if (textureLoaded_) {
        sprite_.setTexture(texture_);
        updateSpriteFrame();
    }
}

/**
 * @brief Reacts to Pac-Man state change events (direction changes).
 * @param event Incoming logic event.
 */
void PacManView::onEvent(const pacman::logic::Event& event) {
    using pacman::logic::EventType;
    using pacman::logic::StateChangedPayload;

    if (event.type != EventType::StateChanged) {
        return;
    }

    const auto* payload = std::get_if<StateChangedPayload>(&event.payload);
    if (!payload) {
        return;
    }

    switch (payload->code) {
    case 0:
        direction_ = logic::Direction::Right;
        break;
    case 1:
        direction_ = logic::Direction::Left;
        break;
    case 2:
        direction_ = logic::Direction::Up;
        break;
    case 3:
        direction_ = logic::Direction::Down;
        break;
    default:
        break;
    }
}

/**
 * @brief Updates the sprite frame selection based on direction and elapsed time.
 */
void PacManView::updateSpriteFrame() {
    if (!textureLoaded_) {
        return;
    }

    auto& sw = pacman::logic::Stopwatch::getInstance();
    const double t = sw.elapsed();

    const unsigned int row = pickRowFor(direction_, t);
    sprite_.setTextureRect(spriteRectFromGrid(texture_, kPacManCol, row));
}

/**
 * @brief Draws Pac-Man centered in his tile using world-to-pixel mapping and animated frames.
 * @param window The render window to draw to.
 */
void PacManView::draw(sf::RenderWindow& window) {
    if (!model_ || !model_->active || !camera_ || !textureLoaded_) {
        return;
    }

    updateSpriteFrame();

    const pacman::logic::Rect worldRect = model_->bounds();
    const auto pixelRect = camera_->worldToPixel(worldRect);

    const auto texRect = sprite_.getTextureRect();
    const float scale = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width);

    const float finalW = static_cast<float>(texRect.width) * scale;
    const float finalH = static_cast<float>(texRect.height) * scale;

    const float posX = static_cast<float>(pixelRect.x) + (static_cast<float>(pixelRect.w) - finalW) * 0.5f;
    const float posY = static_cast<float>(pixelRect.y) + (static_cast<float>(pixelRect.h) - finalH) * 0.5f;

    sprite_.setPosition(posX - 3.5f, posY);
    sprite_.setScale(scale, scale);

    window.draw(sprite_);
}

} // namespace pacman::app