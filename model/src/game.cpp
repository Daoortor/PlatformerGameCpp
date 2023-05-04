#include "game.hpp"
#include <fstream>
#include <utility>
#include "../../tools/json.hpp"
#include "player.hpp"

using json = nlohmann::json;

namespace Platformer {
Game::~Game() = default;

void Game::update() {
    player->notifyAll();
}

Game::Game(
    std::vector<std::vector<std::unique_ptr<Block>>> board_,
    sf::Vector2i playerPos,
    sf::Vector2i endPos_
)
    : board(std::move(board_)), startPos(playerPos), endPos(endPos_) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
}

Game::Game(const std::string &filename) {
    std::ifstream f(filename);
    json levelData = json::parse(f);
    std::vector<std::vector<std::string>> blockMap = levelData["blockMap"];
    sf::Vector2i playerPos = {
        levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    board = Board(blockMap);
    startPos = {levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
}

void Game::writeToFile(const std::string &name, const std::string &filepath) {
    json levelData;
    levelData["levelName"] = name;
    levelData["width"] = board.getSize().x;
    levelData["height"] = board.getSize().y;
    levelData["blockMap"] = board.getBlockMap();
    levelData["playerStartPos"] = {startPos.x, startPos.y};
    levelData["endPos"] = {endPos.x, endPos.y};
    std::ofstream out(filepath + name + ".json");
    out << std::setw(4) << levelData << std::endl;
}
}  // namespace Platformer
