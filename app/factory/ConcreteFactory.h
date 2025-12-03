#pragma once
#include "../logic/score/Score.h"
#include "../views/ViewRegistry.h"
#include "factory/AbstractFactory.h"
#include <memory>

namespace sf {
class RenderWindow;
} // namespace sf

namespace pacman::app {

class ConcreteFactory final : public logic::AbstractFactory {
public:
  ConcreteFactory() noexcept : window_(nullptr) {}
  explicit ConcreteFactory(sf::RenderWindow &window) noexcept
      : window_(&window) {
  } // Stores reference to render window for view creation
  ~ConcreteFactory() override = default;

  void setWindow(sf::RenderWindow &window) noexcept { window_ = &window; }

  std::shared_ptr<logic::PacMan>
  createPacMan() override; // Creates PacMan model and attaches its view
  std::shared_ptr<logic::Ghost> createGhost(logic::GhostKind kind)
      override; // Creates Ghost model and attaches its view
  std::shared_ptr<logic::Coin>
  createCoin() override; // Creates Coin model and attaches its view
  std::shared_ptr<logic::Fruit>
  createFruit() override; // Creates Fruit model and attaches its view
  std::shared_ptr<logic::Wall>
  createWall() override; // Creates Wall model and attaches its view

  void setScoreObserver(pacman::logic::Score *score) noexcept {
    scoreObserver_ = score;
  }

  ViewRegistry &views() noexcept {
    return views_;
  } // Expose view registry for drawing in the app loop

private:
  sf::RenderWindow *window_; // Pointer to render window
  pacman::logic::Score *scoreObserver_{nullptr};
  ViewRegistry views_; // Central store of all views created by this factory

  void
  attachViewToModel(const std::shared_ptr<logic::PacMan>
                        &); // Links PacMan model to its visual representation
  void
  attachViewToModel(const std::shared_ptr<logic::Ghost>
                        &); // Links Ghost model to its visual representation
  void
  attachViewToModel(const std::shared_ptr<logic::Coin>
                        &); // Links Coin model to its visual representation
  void
  attachViewToModel(const std::shared_ptr<logic::Fruit>
                        &); // Links Fruit model to its visual representation
  void
  attachViewToModel(const std::shared_ptr<logic::Wall>
                        &); // Links Wall model to its visual representation
};
} // namespace pacman::app