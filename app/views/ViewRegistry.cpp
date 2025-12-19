#include "ViewRegistry.h"

namespace pacman::app {

    /**
     * @brief Registers a non-null view into the registry.
     * @param view The view to store.
     */
    void ViewRegistry::add(Ptr view) {
        if (view) {
            views_.push_back(std::move(view));
        }
    }

    /**
     * @brief Draws all registered views.
     * @param window The render window to draw to.
     */
    void ViewRegistry::drawAll(sf::RenderWindow& window) {
        for (auto& view : views_) {
            view->draw(window);
        }
    }

    /**
     * @brief Clears all views from the registry.
     */
    void ViewRegistry::clear() {
        views_.clear();
    }

    /**
     * @brief Returns the internal container of views.
     * @return Reference to the view vector.
     */
    std::vector<ViewRegistry::Ptr>& ViewRegistry::raw() {
        return views_;
    }

} // namespace pacman::app