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

        // All Pac-Man frames sit in one column.
        constexpr unsigned int PACMAN_COL = 17; // 0-based

        // Row indices for each animation frame (0-based)
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
            const auto size = tex.getSize();
            const float cellW = static_cast<float>(size.x) / static_cast<float>(SHEET_COLS);
            const float cellH = static_cast<float>(size.y) / static_cast<float>(SHEET_ROWS);

            const int left   = static_cast<int>(std::round(col * cellW));
            const int top    = static_cast<int>(std::round(row * cellH));
            const int right  = static_cast<int>(std::round((col + 1u) * cellW));
            const int bottom = static_cast<int>(std::round((row + 1u) * cellH));

            return { left, top, right - left, bottom - top };
        }

        unsigned int pickRowFor(logic::Direction dir, double t) {
            const double speed   = 8.0;
            const double phase   = std::fmod(t * speed, 4.0);

            bool useClosed = (phase < 1.0);
            bool useSmall  = (phase >= 1.0 && phase < 2.5);
            bool useBig    = (phase >= 2.5 && phase < 3.5);

            if (useClosed) {
                return ROW_CLOSED;
            }

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
        if (textureLoaded_) return;
        if (!texture_.loadFromFile(SPRITE_SHEET_PATH)) return;
        textureLoaded_ = true;
    }

    PacManView::PacManView(const std::shared_ptr<pacman::logic::PacMan> &model) : model_(model) {
        ensureTextureLoaded();

        if (textureLoaded_) {
            sprite_.setTexture(texture_);
            updateSpriteFrame();
        }
    }

    void PacManView::onEvent(const pacman::logic::Event &e) {
        using pacman::logic::EventType;
        using pacman::logic::MovedPayload;
        using pacman::logic::StateChangedPayload;

        if (e.type == EventType::StateChanged) {
            if (const auto* payload = std::get_if<pacman::logic::StateChangedPayload>(&e.payload)) {
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
        }
    }

    void PacManView::updateSpriteFrame() {
        if (!textureLoaded_) return;

        auto& sw = pacman::logic::Stopwatch::getInstance();
        const double t = sw.elapsed();

        const unsigned int row = pickRowFor(direction_, t);
        const auto rect = spriteRectFromGrid(texture_, PACMAN_COL, row);

        sprite_.setTextureRect(rect);
    }

    void PacManView::draw(sf::RenderWindow &window) {
        if (!model_) return;
        if (!model_->active) return;
        if (!camera_) return;
        if (!textureLoaded_) return;

        updateSpriteFrame();

        pacman::logic::Rect r = model_->bounds();
        auto pixelRect = camera_->worldToPixel(r);

        const auto& textRect = sprite_.getTextureRect();
        float scaleX = static_cast<float>(pixelRect.w) / static_cast<float>(textRect.width);
        float scaleY = static_cast<float>(pixelRect.h) / static_cast<float>(textRect.height);

        sprite_.setPosition(static_cast<float>(pixelRect.x), static_cast<float>(pixelRect.y));
        sprite_.setScale(scaleX, scaleY);

        window.draw(sprite_);
    }
}