#include "PacManView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

namespace pacman::app {

    sf::Texture PacManView::texture_{};
    bool PacManView::textureLoaded_{false};

    namespace {
        constexpr const char* SPRITE_SHEET_PATH = "assets/sprites/sprite.png";

        constexpr int FRAME_W = 16;
        constexpr int FRAME_H = 16;

        constexpr int ROW_RIGHT = 0;
        constexpr int ROW_LEFT  = 1;
        constexpr int ROW_UP    = 2;
        constexpr int ROW_DOWN  = 3;

        constexpr int COL_OPEN  = 0;
        constexpr int COL_CLOSED = 1;
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
                        direction_ = Direction::Right;
                        break;
                    case 1:
                        direction_ = Direction::Left;
                        break;
                    case 2:
                        direction_ = Direction::Up;
                        break;
                    case 3:
                        direction_ = Direction::Down;
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
        double t = sw.elapsed();

        double phase = std::fmod(t * 4.0, 1.0);
        bool moutOpen = phase < 0.5;

        int row = 0;
        switch (direction_) {
            case Direction::Right:
                row = ROW_RIGHT;
                break;
            case Direction::Left:
                row = ROW_LEFT;
                break;
            case Direction::Up:
                row = ROW_UP;
                break;
            case Direction::Down:
                row = ROW_DOWN;
                break;
        }

        int col = moutOpen ? COL_OPEN : COL_CLOSED;

        int x = col * FRAME_W;
        int y = col * FRAME_H;

        sprite_.setTextureRect(sf::IntRect(x, y, FRAME_W, FRAME_H));
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