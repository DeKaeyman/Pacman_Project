// logic/camera/Camera.cpp
#include "Camera.h"
#include <algorithm>

namespace pacman::logic {

    Camera::Camera(int pixelWidth, int pixelHeight) : width_{pixelWidth > 0 ? pixelWidth : 1}, height_{pixelHeight > 0 ? pixelHeight : 1} {} // Initializes the camera with a given viewport size

    void Camera::setViewport(int pixelWidth, int pixelHeight) { // Updates the viewport size. Gets called when the window resizes
        width_ = pixelWidth > 0 ? pixelWidth : 1;
        height_ = pixelHeight > 0 ? pixelHeight : 1;
    }

    std::pair<float, float> Camera::worldToPixel(float wx, float wy) const noexcept { // Converts world coordinates in [-1,1] to pixel space.
        float nx = (wx + 1.0f) * 0.5f; // Normalize X from [-1,1] to [0,1]
        float ny = (wy + 1.0f) * 0.5f; // Normalize Y from [-1,1] to [0,1]

        ny = 1.0f - ny; // Flip Y so that 0 = top

        // Scale normalized coordinates to the viewport size
        float px = nx * static_cast<float>(width_);
        float py = ny * static_cast<float>(height_);

        return {px, py};
    }

    PixelRect Camera::worldToPixel(const pacman::logic::Rect &worldRect) const noexcept { // Converts a world space rectangle into a pixel space rectangle
        auto [px, py] = worldToPixel(worldRect.x, worldRect.y); // Convert the top left corner

        // World coordinates range is fixed as [-1,1], size = 2.0
        float scaleX = static_cast<float>(width_) / 2.0f;
        float scaleY = static_cast<float>(height_) / 2.0f;

        // Convert width/height from world units to pixels
        float pw = worldRect.w * scaleX;
        float ph = worldRect.h * scaleY;


        // Build and return pixel rectangle
        PixelRect pr{};
        pr.x = static_cast<int>(px);
        pr.y = static_cast<int>(py);
        pr.w = static_cast<int>(pw);
        pr.h = static_cast<int>(ph);

        return pr;
    }
}