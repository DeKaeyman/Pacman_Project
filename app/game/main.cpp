#include "Game.h"

int main() {
  pacman::app::Game game(800, 600, "PacMan");
  game.run();
  return 0;
}