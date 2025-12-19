#include "Camera.h"

#include <algorithm>
#include <cmath>

namespace pacman::logic {

    namespace {
        constexpr float kWorldExtent = 2.0f;
    } // namespace

    /**
     * @brief Constructs a camera with a pixel viewport.
     *
     * The viewport dimensions are clamped to at least 1 to avoid division by zero.
     *
     * @param pixelWidth Viewport width in pixels.
     * @param pixelHeight Viewport height in pixels.
     */
    Camera::Camera(int pixelWidth, int pixelHeight)
            : width_{pixelWidth > 0 ? pixelWidth : 1},
              height_{pixelHeight > 0 ? pixelHeight : 1} {}

    /**
     * @brief Updates the camera viewport size.
     *
     * Values are clamped to at least 1 to avoid invalid dimensions.
     *
     * @param pixelWidth New viewport width in pixels.
     * @param pixelHeight New viewport height in pixels.
     */
    void Camera::setViewport(int pixelWidth, int pixelHeight) {
        width_ = (pixelWidth > 0) ? pixelWidth : 1;
        height_ = (pixelHeight > 0) ? pixelHeight : 1;
    }

    /**
     * @brief Converts a world-space point to pixel coordinates using uniform scaling and letterboxing.
     *
     * World coordinates are expected to be in the range [-1, 1] for both axes,
     * which is mapped to a square world extent.
     *
     * @param wx World-space x coordinate.
     * @param wy World-space y coordinate.
     * @return Pixel-space coordinates (x, y) as floats.
     */
    std::pair<float, float> Camera::worldToPixel(float wx, float wy) const noexcept {
        const float scale = static_cast<float>(std::min(width_, height_)) / kWorldExtent;

        const float worldPixelW = kWorldExtent * scale;
        const float worldPixelH = kWorldExtent * scale;

        const float offsetX = (static_cast<float>(width_) - worldPixelW) * 0.5f;
        const float offsetY = (static_cast<float>(height_) - worldPixelH) * 0.5f;

        const float nx = (wx + 1.0f) * 0.5f * kWorldExtent;
        const float ny = (wy + 1.0f) * 0.5f * kWorldExtent;

        const float px = offsetX + nx * scale;
        const float py = offsetY + ny * scale;

        return {px, py};
    }

    /**
     * @brief Converts a world-space rectangle to a pixel-space rectangle.
     * @param worldRect The rectangle in world coordinates.
     * @return The rectangle mapped into pixel coordinates.
     */
    PixelRect Camera::worldToPixel(const pacman::logic::Rect& worldRect) const noexcept {
        const auto [px, py] = worldToPixel(worldRect.x, worldRect.y);
        const float scale = static_cast<float>(std::min(width_, height_)) / kWorldExtent;

        const float pw = worldRect.w * scale;
        const float ph = worldRect.h * scale;

        PixelRect pr{};
        pr.x = static_cast<int>(std::lround(px));
        pr.y = static_cast<int>(std::lround(py));
        pr.w = static_cast<int>(std::lround(pw));
        pr.h = static_cast<int>(std::lround(ph));
        return pr;
    }

} // namespace pacman::logic