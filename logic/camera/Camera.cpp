// logic/camera/Camera.cpp
#include "Camera.h"
#include <algorithm>
#include <cmath>

namespace pacman::logic {

static constexpr float WorldExtent = 2.0f;

Camera::Camera(int pixelWidth, int pixelHeight)
    : width_{pixelWidth > 0 ? pixelWidth : 1}, height_{pixelHeight > 0 ? pixelHeight : 1} {
} // Initializes the camera with a given viewport size

void Camera::setViewport(int pixelWidth,
                         int pixelHeight) { // Updates the viewport size. Gets
                                            // called when the window resizes
    width_ = pixelWidth > 0 ? pixelWidth : 1;
    height_ = pixelHeight > 0 ? pixelHeight : 1;
}

std::pair<float, float> Camera::worldToPixel(float wx, float wy) const noexcept {
    // Uniform scale: same for X and Y so world units stay square
    const float scale = static_cast<float>(std::min(width_, height_)) / WorldExtent;

    // Size of the world area in pixels after uniform scaling
    const float worldPixelW = WorldExtent * scale;
    const float worldPixelH = WorldExtent * scale;

    // Letterbox: center the world in the window
    const float offsetX = (static_cast<float>(width_) - worldPixelW) * 0.5f;
    const float offsetY = (static_cast<float>(height_) - worldPixelH) * 0.5f;

    // Map world [-1,1] -> [0, WorldExtent] in both axes
    const float nx = (wx + 1.0f) * 0.5f * WorldExtent;
    const float ny = (wy + 1.0f) * 0.5f * WorldExtent;

    const float px = offsetX + nx * scale;
    const float py = offsetY + ny * scale;

    return {px, py};
}

PixelRect Camera::worldToPixel(const pacman::logic::Rect& worldRect)
    const noexcept { // Converts a world space rectangle into a pixel space rectangle
    auto [px, py] = worldToPixel(worldRect.x, worldRect.y); // Convert the top left corner

    const float scale = static_cast<float>(std::min(width_, height_)) / WorldExtent;

    // Convert width/height from world units to pixels
    const float pw = worldRect.w * scale;
    const float ph = worldRect.h * scale;

    // Build and return pixel rectangle
    PixelRect pr{};
    pr.x = static_cast<int>(std::lround(px));
    pr.y = static_cast<int>(std::lround(py));
    pr.w = static_cast<int>(std::lround(pw));
    pr.h = static_cast<int>(std::lround(ph));
    return pr;
}
} // namespace pacman::logic