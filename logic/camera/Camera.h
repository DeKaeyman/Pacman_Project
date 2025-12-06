// logic/camera/Camera.h
#pragma once
#include "../entities/Entity.h"
#include <utility>

namespace pacman::logic {

struct PixelRect {
    int x{}; // Pixel X-coordinate (top left)
    int y{}; // Pixel Y-coordinate (top left)
    int w{}; // Width of the rectangle in pixels
    int h{}; // Height of the rectangle in pixels
};

class Camera {
public:
    Camera(int pixelWidth, int pixelHeight);

    void setViewport(int pixelWidth, int pixelHeight); // Update the viewport size

    int width() const noexcept { return width_; };   // Current viewport width in pixels
    int height() const noexcept { return height_; }; // Current viewport height in pixels

    std::pair<float, float> worldToPixel(float wx,
                                         float wy) const noexcept; // Convert a world space point to pixel coordinates

    PixelRect worldToPixel(
        const Rect& worldRect) const noexcept; // Convert a world space rectangle to a pixel space rectangle

private:
    int width_{1};  // Current viewport width in pixel
    int height_{1}; // Current viewport height in pixel
};
} // namespace pacman::logic