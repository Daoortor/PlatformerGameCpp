#include "../include/game.hpp"
#include <fstream>
#include <utility>
#include <chrono>
#include "../../tools/json.hpp"
#include "../include/player.hpp"

using json = nlohmann::json;

namespace Platformer {
void Statistics::startingGame() {
    startTime = std::chrono::steady_clock::now();
}

void Statistics::playerDeath() {
    lastRespawnTime = std::chrono::steady_clock::now();
    deathCount++;
}

void Statistics::endingGame() {
    isEnded = true;
    endTime = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::time_point Statistics::getStartTime() const {
    return startTime;
}

std::chrono::steady_clock::time_point Statistics::getEndTime() const {
    return endTime;
}

double Statistics::getAverageDeathTime() const {
    std::chrono::duration<double> result = (endTime - startTime) / deathCount;
    return result.count();
}

int Statistics::getDeathCount() const {
    return deathCount;
}

Game::~Game() = default;

void Game::update() {
    player->notifyAll();
}

Game::Game(
    std::vector<std::vector<std::unique_ptr<Block>>> board_,
    utilities::Vector playerPos,
    utilities::Vector endPos_
)
    : board(std::move(board_)), endPos(endPos_) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    statistics = Statistics(&(*player));
}

Game::Game(const std::string &filename) {
    std::ifstream f(filename);
    json levelData = json::parse(f);
    std::vector<std::vector<std::string>> blockMap = levelData["blockMap"];
    utilities::Vector playerPos = {
        levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    board = Board(blockMap);
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
    statistics = Statistics(&(*player));
}
}  // namespace Platformer
