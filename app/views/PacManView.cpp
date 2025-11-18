#include "PacManView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

namespace pacman::app {

    sf::Texture PacManView::texture_{};
    bool PacManView::textureLoaded_{false};

    namespace {
        constexpr const char* SPRITE_SHEET_PATH = "assets/sprites/sprite.png";

        constexpr unsigned int SHEET_COLS = 19;
        constexpr unsigned int SHEET_ROWS = 19;

        // All Pac-Man frames sit in one column (0-based).
        constexpr unsigned int PACMAN_COL = 17;

        // Row indices for each animation frame (0-based).
        constexpr unsigned int ROW_CLOSED = 0;
        constexpr unsigned int ROW_RIGHT_SMALL = 1;
        constexpr unsigned int ROW_RIGHT_BIG = 2;
        constexpr unsigned int ROW_DOWN_SMALL = 4;
        constexpr unsigned int ROW_DOWN_BIG = 5;
        constexpr unsigned int ROW_LEFT_SMALL = 7;
        constexpr unsigned int ROW_LEFT_BIG = 8;
        constexpr unsigned int ROW_UP_SMALL = 10;
        constexpr unsigned int ROW_UP_BIG = 11;

        sf::IntRect spriteRectFromGrid(const sf::Texture& tex, unsigned int col, unsigned int row) {
            const auto size = tex.getSize(); // Pixel dimension of the whole sheet
            const float cellW = static_cast<float>(size.x) / static_cast<float>(SHEET_COLS); // Width of one cell
            const float cellH = static_cast<float>(size.y) / static_cast<float>(SHEET_ROWS); // Height of one cell

            const int left = static_cast<int>(std::round(col * cellW));
            const int top = static_cast<int>(std::round(row * cellH));
            const int right = static_cast<int>(std::round((col + 1u) * cellW));
            const int bottom = static_cast<int>(std::round((row + 1u) * cellH));

            return { left, top, right - left, bottom - top }; // intRect describing the cell region
        }

        unsigned int pickRowFor(logic::Direction dir, double t) {
            const double speed = 8.0; // Controls how fast the mouth cycles
            const double phase = std::fmod(t * speed, 4.0); // Animation phase in range [0,4)

            bool useClosed = (phase < 1.0);
            bool useSmall = (phase >= 1.0 && phase < 2.5);
            bool useBig = (phase >= 2.5 && phase < 3.5);

            if (useClosed) {
                return ROW_CLOSED; // Always use closed frame in closed phase
            }

            // Choose row based on facing direction and mouth state
            switch (dir) {
                case logic::Direction::Right:
                    if (useBig)  return ROW_RIGHT_BIG;
                    if (useSmall) return ROW_RIGHT_SMALL;
                    return ROW_CLOSED;
                case logic::Direction::Down:
                    if (useBig)  return ROW_DOWN_BIG;
                    if (useSmall) return ROW_DOWN_SMALL;
                    return ROW_CLOSED;
                case logic::Direction::Left:
                    if (useBig)  return ROW_LEFT_BIG;
                    if (useSmall) return ROW_LEFT_SMALL;
                    return ROW_CLOSED;
                case logic::Direction::Up:
                    if (useBig)  return ROW_UP_BIG;
                    if (useSmall) return ROW_UP_SMALL;
                    return ROW_CLOSED;
            }
            return ROW_CLOSED;
        }
    }

    void PacManView::ensureTextureLoaded() {
        if (textureLoaded_) return; // Do nothing if already loaded
        if (!texture_.loadFromFile(SPRITE_SHEET_PATH)) return; // Try to load sprite sheet
        textureLoaded_ = true; // Mark as loaded on success
    }

    PacManView::PacManView(const std::shared_ptr<pacman::logic::PacMan> &model) : model_(model) {
        ensureTextureLoaded(); // Make sure texture is ready

        if (textureLoaded_) {
            sprite_.setTexture(texture_); // Attach texture to sprite
            updateSpriteFrame(); // Pick initial frame based on default direction
        }
    }

    void PacManView::onEvent(const pacman::logic::Event &e) {
        using pacman::logic::EventType;
        using pacman::logic::MovedPayload;
        using pacman::logic::StateChangedPayload;

        if (e.type == EventType::StateChanged) { // StateChanged encodes direction changes
            if (const auto* payload = std::get_if<pacman::logic::StateChangedPayload>(&e.payload)) {
                switch (payload->code) { // Interpret code as direction enum
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
        }
    }

    void PacManView::updateSpriteFrame() {
        if (!textureLoaded_) return; // no texture, no frame update

        auto& sw = pacman::logic::Stopwatch::getInstance(); // Global stopwatch singleton
        const double t = sw.elapsed(); // Total elapsed time since start/reset

        const unsigned int row = pickRowFor(direction_, t); // Select proper animation row
        const auto rect = spriteRectFromGrid(texture_, PACMAN_COL, row); // Compute UV rectangle

        sprite_.setTextureRect(rect); // Apply new texture rectangle to sprite
    }

    void PacManView::draw(sf::RenderWindow &window) {
        if (!model_) return;
        if (!model_->active) return;
        if (!camera_) return;
        if (!textureLoaded_) return;

        updateSpriteFrame(); // Update frame before drawing

        pacman::logic::Rect r = model_->bounds(); // World space bounding box of Pacman
        auto pixelRect = camera_->worldToPixel(r); // Convert to pixel space rectangle

        const auto& texRect = sprite_.getTextureRect(); // Current sprite frame size in pixels

        float scale = static_cast<float>(pixelRect.w) / static_cast<float>(texRect.width); // Uniform scaling

        float finalW = texRect.width  * scale; // Actual drawn width
        float finalH = texRect.height * scale; // Actual drawn height

        float posX = static_cast<float>(pixelRect.x) + (pixelRect.w - finalW) * 0.5f; // Center horizontally
        float posY = static_cast<float>(pixelRect.y) + (pixelRect.h - finalH) * 0.5f; // Center vertically

        sprite_.setPosition(posX, posY); // Place sprite on screen
        sprite_.setScale(scale, scale); // Apply uniform scaling

        window.draw(sprite_); // Draw Pacman sprite
    }
}