#include "GhostView.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <cmath>

namespace pacman::app {

    sf::Texture GhostView::texture_{};
    bool GhostView::textureLoaded_{false};

    namespace {
        constexpr const char* kSpriteSheetPath = "assets/sprites/sprite.png";

        constexpr unsigned int kSheetCols = 19;
        constexpr unsigned int kSheetRows = 19;

        /**
         * @brief Returns the sprite sheet column for a given ghost kind.
         * @param kind The ghost kind.
         * @return Sprite sheet column index for that kind.
         */
        unsigned int columnForKind(pacman::logic::GhostKind kind) {
            using pacman::logic::GhostKind;

            switch (kind) {
                case GhostKind::A:
                    return 13;
                case GhostKind::B:
                    return 14;
                case GhostKind::C:
                    return 15;
                case GhostKind::D:
                    return 16;
            }

            return 13;
        }

        constexpr unsigned int kRowRight1 = 0;
        constexpr unsigned int kRowRight2 = 1;
        constexpr unsigned int kRowDown1 = 2;
        constexpr unsigned int kRowDown2 = 3;
        constexpr unsigned int kRowLeft1 = 4;
        constexpr unsigned int kRowLeft2 = 5;
        constexpr unsigned int kRowUp1 = 6;
        constexpr unsigned int kRowUp2 = 7;

        constexpr unsigned int kFearCol1 = 0;
        constexpr unsigned int kFearCol2 = 1;
        constexpr unsigned int kFearRow1 = 11;
        constexpr unsigned int kFearRow2 = 12;

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
         * @brief Selects an animation row based on direction and elapsed time.
         * @param dir Current ghost direction.
         * @param t Elapsed time used for animation.
         * @return Sprite sheet row index.
         */
        unsigned int pickRowFor(logic::Direction dir, double t) {
            const double speed = 8.0;
            const double phase = std::fmod(t * speed, 2.0);
            const bool firstFrame = (phase < 1.0);

            switch (dir) {
                case logic::Direction::Right:
                    return firstFrame ? kRowRight1 : kRowRight2;
                case logic::Direction::Down:
                    return firstFrame ? kRowDown1 : kRowDown2;
                case logic::Direction::Left:
                    return firstFrame ? kRowLeft1 : kRowLeft2;
                case logic::Direction::Up:
                    return firstFrame ? kRowUp1 : kRowUp2;
                case logic::Direction::None:
                default:
                    return kRowRight1;
            }
        }

        /**
         * @brief Selects the fear-mode sprite column based on elapsed time (flashing effect).
         * @param t Elapsed time used for animation.
         * @return Sprite sheet column index.
         */
        unsigned int pickFearColumn(double t) {
            const double flashSpeed = 2.0;
            const double phase = std::fmod(t * flashSpeed, 2.0);
            return (phase < 1.0) ? kFearCol1 : kFearCol2;
        }

        /**
         * @brief Selects the fear-mode sprite row based on elapsed time (mouth animation).
         * @param t Elapsed time used for animation.
         * @return Sprite sheet row index.
         */
        unsigned int pickFearRow(double t) {
            const double animSpeed = 8.0;
            const double phase = std::fmod(t * animSpeed, 2.0);
            return (phase < 1.0) ? kFearRow1 : kFearRow2;
        }
    } // namespace

    /**
     * @brief Loads the shared sprite sheet texture on first use.
     *
     * If loading fails, rendering will be skipped (matching current behavior).
     */
    void GhostView::ensureTextureLoaded() {
        if (textureLoaded_) {
            return;
        }

        if (!texture_.loadFromFile(kSpriteSheetPath)) {
            return;
        }

        textureLoaded_ = true;
    }

    /**
     * @brief Constructs a GhostView bound to the given ghost model and prepares its sprite.
     * @param model Shared pointer to the ghost logic entity.
     */
    GhostView::GhostView(const std::shared_ptr<pacman::logic::Ghost>& model)
            : model_(model) {
        ensureTextureLoaded();

        if (textureLoaded_) {
            sprite_.setTexture(texture_);
            updateSpriteFrame();
        }
    }

    /**
     * @brief Reacts to ghost state-change events (direction changes and fear mode toggles).
     * @param event Incoming logic event.
     */
    void GhostView::onEvent(const pacman::logic::Event& event) {
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
            case 100:
                fearMode_ = true;
                break;
            case 101:
                fearMode_ = false;
                break;
            default:
                break;
        }
    }

    /**
     * @brief Updates the sprite frame selection based on direction, fear mode, and elapsed time.
     */
    void GhostView::updateSpriteFrame() {
        if (!textureLoaded_ || !model_) {
            return;
        }

        auto& sw = pacman::logic::Stopwatch::getInstance();
        const double t = sw.elapsed();

        unsigned int col = 0;
        unsigned int row = 0;

        if (fearMode_) {
            col = pickFearColumn(t);
            row = pickFearRow(t);
        } else {
            row = pickRowFor(direction_, t);
            col = columnForKind(model_->kind());
        }

        sprite_.setTextureRect(spriteRectFromGrid(texture_, col, row));
        sprite_.setColor(sf::Color::White);
    }

    /**
     * @brief Draws the ghost sprite centered inside its tile.
     * @param window The render window to draw to.
     */
    void GhostView::draw(sf::RenderWindow& window) {
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

        sprite_.setPosition(posX, posY);
        sprite_.setScale(scale, scale);

        window.draw(sprite_);
    }

} // namespace pacman::app