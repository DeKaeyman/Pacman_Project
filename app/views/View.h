#pragma once

#include "camera/Camera.h"
#include "observer/Observer.h"

#include <SFML/Graphics/RenderWindow.hpp>

namespace pacman::app {

    /**
     * @brief Abstract base class for all renderable views.
     *
     * A View observes logic-side models and renders their visual
     * representation using a shared camera.
     */
    class View : public pacman::logic::Observer {
    public:
        /**
         * @brief Virtual destructor for safe polymorphic destruction.
         */
        ~View() override = default;

        /**
         * @brief Draws the view to the given render window.
         * @param window The render window to draw to.
         */
        virtual void draw(sf::RenderWindow& window) = 0;

        /**
         * @brief Receives events from observed models.
         *
         * Default implementation ignores events.
         *
         * @param event The event sent by the observed model.
         */
        void onEvent(const pacman::logic::Event& event) override;

        /**
         * @brief Sets the shared camera used by all views.
         * @param camera Pointer to the active camera (not owned).
         */
        static void setCamera(pacman::logic::Camera* camera) noexcept;

    protected:
        /**
         * @brief Shared camera used for rendering.
         */
        inline static pacman::logic::Camera* camera_ = nullptr;
    };

} // namespace pacman::app