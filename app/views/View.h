#pragma once
#include "camera/Camera.h"
#include "observer/Observer.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

namespace pacman::app {

class View
    : public pacman::logic::Observer { // Base UI element that listens to model
                                       // events and can draw itself
public:
  ~View() override = default;

  virtual void draw(sf::RenderWindow &window) = 0; // Render this view into the
                                                   // given SFML window

  void onEvent(const pacman::logic::Event & /*e*/) override {}

  static void setCamera(pacman::logic::Camera *cam) noexcept {
    camera_ = cam;
  } // Hook to give the shared Camera instance to all views

protected:
  inline static pacman::logic::Camera *camera_ = nullptr;
};
} // namespace pacman::app
