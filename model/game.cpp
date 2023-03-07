#include "game.hpp"
#include <utility>
#include "player.hpp"

namespace Platformer {
void Game::act(char command) {
    if (command == 'W') {
        player->jump();
    } else if (command == 'A') {
        player->moveLeft();
    } else if (command == 'S') {
        player->moveDown();
    } else if (command == 'D') {
        player->moveRight();
    }
}

void Game::update() {
    player->notifyAll();
}

Game::Game(int height_, int width_, std::vector<std::vector<Block>> field_)
    : height(height_), width(width_), field(std::move(field_)){};
}  // namespace Platformer
