#include "game.hpp"
#include <utility>
#include "player.hpp"

namespace Platformer {
Block Game::getBlock(int x, int y) {
    if (0 <= x && x < width && 0 <= y && y < height) {
        return field[x][y];
    }
    return Air();
}

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

Game::Game(std::vector<std::vector<Block>> field_, Vector playerPos)
    : field(std::move(field_)) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    if (field.empty()) {
        height = 0;
        width = 0;
    } else {
        height = static_cast<int>(field.size());
        width = static_cast<int>(field[0].size());
        for (const auto &row : field) {
            if (row.size() !=
                width) {  // Reset field if field is non-rectangular
                height = 0;
                width = 0;
                field = std::vector<std::vector<Block>>();
                return;
            }
        }
    }
}
}  // namespace Platformer
