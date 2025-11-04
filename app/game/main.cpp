#include "Game.h"

int main() {
    pacman::app::Game game(800, 600, "Pacman");
    game.run();
    return 0;
}