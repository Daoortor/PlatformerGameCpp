#include "../include/game.hpp"
#include <utility>
#include <fstream>
#include "../include/player.hpp"
#include "../../tools/json.hpp"

using json = nlohmann::json;

namespace Platformer {
Game::~Game() = default;

const std::unique_ptr<Block>& Game::getBlock(utilities::Vector pos) {
    if (0 <= pos.get_x() && pos.get_x() < width && 0 <= pos.get_y() && pos.get_y() < height) {
        return field[pos.get_y()][pos.get_x()];
    }
    return AIR_BLOCK;
}

const std::unique_ptr<Block> &Game::getBlockByCoordinates(utilities::Vector pos) {
    return getBlock({utilities::divide(pos.get_x(), BLOCK_SIZE), utilities::divide(pos.get_y(), BLOCK_SIZE)});
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

Game::Game(std::vector<std::vector<std::unique_ptr<Block>>> field_, utilities::Vector playerPos, utilities::Vector endPos_)
    : field(std::move(field_)), endPos(endPos_) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    if (field.empty()) {
        height = 0;
        width = 0;
    } else {
        height = static_cast<int>(field.size());
        width = static_cast<int>(field[0].size());
        for (const auto &row : field) {
            if (static_cast<int>(row.size()) !=
                width) {  // Reset field if field is non-rectangular
                height = 0;
                width = 0;
                field = std::vector<std::vector<std::unique_ptr<Block>>>();
                return;
            }
        }
    }
}

Game::Game(const std::string &filename) {
    std::ifstream f(filename);
    json levelData = json::parse(f);
    std::vector<std::vector<std::string>> blockMap = levelData["blockMap"];
    utilities::Vector playerPos = {levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    for (int row=0; row<blockMap.size(); row++) {
        field.emplace_back();
        for (const std::string& blockName : blockMap[row]) {
            field[row].push_back(makeBlock(blockName));
        }
    }
    if (field.empty()) {
        height = 0;
        width = 0;
    } else {
        height = static_cast<int>(field.size());
        width = static_cast<int>(field[0].size());
        for (const auto &row : field) {
            if (static_cast<int>(row.size()) !=
                width) {  // Reset field if field is non-rectangular
                height = 0;
                width = 0;
                field = std::vector<std::vector<std::unique_ptr<Block>>>();
                return;
            }
        }
    }
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
}
}  // namespace Platformer
