#include "View.h"

namespace pacman::app {

/**
 * @brief Default event handler for views (does nothing).
 * @param event Incoming logic event.
 */
void View::onEvent(const pacman::logic::Event& event) { (void)event; }

/**
 * @brief Assigns the shared camera to all views.
 * @param camera Pointer to the active camera.
 */
void View::setCamera(pacman::logic::Camera* camera) noexcept { camera_ = camera; }

} // namespace pacman::app