#pragma once
#include <cstdint>
#include <memory>

namespace pacman::logic {

// Forward declare models
class PacMan;
class Ghost;
class Coin;
class Fruit;
class Wall;

enum class GhostKind : std::uint8_t {
  A = 0,
  B,
  C,
  D
}; // Enum defining different ghost types

class AbstractFactory { // Interface for all factories that create game entities
public:
  virtual ~AbstractFactory() = default;

  virtual std::shared_ptr<PacMan> createPacMan() = 0; // Builds a PacMan model
  virtual std::shared_ptr<Ghost>
  createGhost(GhostKind kind) = 0;                  // Builds a Ghost model
  virtual std::shared_ptr<Coin> createCoin() = 0;   // Builds a Coin model
  virtual std::shared_ptr<Fruit> createFruit() = 0; // Builds a Fruit model
  virtual std::shared_ptr<Wall> createWall() = 0;   // Builds a Wall model
};
} // namespace pacman::logic