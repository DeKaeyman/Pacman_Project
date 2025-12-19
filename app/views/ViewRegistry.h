#pragma once

#include "View.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <vector>

namespace pacman::app {

    /**
     * @brief Owns and manages all active views for rendering.
     *
     * The registry batches drawing calls and centralizes ownership
     * of all view instances created during gameplay.
     */
    class ViewRegistry {
    public:
        /**
         * @brief Unique pointer type used for storing views.
         */
        using Ptr = std::unique_ptr<View>;

        /**
         * @brief Registers a new view in the registry.
         * @param view The view to add (ignored if null).
         */
        void add(Ptr view);

        /**
         * @brief Draws all registered views to the given render window.
         * @param window The render window to draw to.
         */
        void drawAll(sf::RenderWindow& window);

        /**
         * @brief Removes all registered views.
         */
        void clear();

        /**
         * @brief Provides direct access to the internal view container.
         * @return Reference to the vector of view pointers.
         */
        std::vector<Ptr>& raw();

    private:
        std::vector<Ptr> views_;
    };

} // namespace pacman::app