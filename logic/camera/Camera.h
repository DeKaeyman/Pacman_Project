#pragma once

#include "../entities/Entity.h"

#include <utility>

namespace pacman::logic {

/**
 * @brief Integer pixel-space rectangle.
 */
struct PixelRect {
    int x{};
    int y{};
    int w{};
    int h{};
};

/**
 * @brief Camera that maps world coordinates to pixel coordinates with uniform scaling and letterboxing.
 */
class Camera {
public:
    /**
     * @brief Constructs a camera with a pixel viewport.
     * @param pixelWidth Viewport width in pixels.
     * @param pixelHeight Viewport height in pixels.
     */
    Camera(int pixelWidth, int pixelHeight);

    /**
     * @brief Updates the camera viewport size.
     * @param pixelWidth New viewport width in pixels.
     * @param pixelHeight New viewport height in pixels.
     */
    void setViewport(int pixelWidth, int pixelHeight);

    /**
     * @brief Gets the current viewport width in pixels.
     * @return Viewport width.
     */
    int width() const noexcept { return width_; }

    /**
     * @brief Gets the current viewport height in pixels.
     * @return Viewport height.
     */
    int height() const noexcept { return height_; }

    /**
     * @brief Converts a world-space point to pixel-space coordinates.
     * @param wx World-space x coordinate.
     * @param wy World-space y coordinate.
     * @return Pixel-space coordinates (x, y) as floats.
     */
    std::pair<float, float> worldToPixel(float wx, float wy) const noexcept;

    /**
     * @brief Converts a world-space rectangle to a pixel-space rectangle.
     * @param worldRect The rectangle in world coordinates.
     * @return The rectangle mapped into pixel coordinates.
     */
    PixelRect worldToPixel(const Rect& worldRect) const noexcept;

private:
    int width_{1};
    int height_{1};
};

} // namespace pacman::logic