#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <memory>
#include <vector>
#include <chrono>
#include <iostream>
#include "../../tools/utilities.hpp"
#include "block.hpp"
#include "board.hpp"
#include "player_fwd.hpp"

namespace Platformer {

class Statistics {
    Player *player = nullptr;
    std::chrono::steady_clock::time_point sessionStartTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastRespawnTime;
    std::chrono::steady_clock::time_point endTime;
    bool isEnded = false;
    int deathCount = 0;
    double averageDeathTime = 0;

public:
    explicit Statistics() = default;

    explicit Statistics(Player* player_) : player(player_) {
        sessionStartTime = std::chrono::steady_clock::now();
    }

    void startingGame();
    void playerDeath();
    void endingGame();

    [[nodiscard]] std::chrono::steady_clock::time_point getStartTime() const;
    [[nodiscard]] std::chrono::steady_clock::time_point getEndTime() const;
    [[nodiscard]] double getAverageDeathTime() const;
    [[nodiscard]] int getDeathCount() const;
};

class Game {
    Statistics statistics;
    Board board;
    std::unique_ptr<Player> player;
    int timer = 0;
    bool is_paused = false;
    utilities::Vector endPos;

public:
    ~Game();
    Game(Game &&other) = default;
    Game &operator=(Game &&other) = default;
    Game(
        std::vector<std::vector<std::unique_ptr<Block>>> board_,
        utilities::Vector playerPos,
        utilities::Vector endPos_
    );
    explicit Game(const std::string &filename);

    [[nodiscard]] Player *getPlayer() const {
        return player.get();
    }

    std::vector<std::vector<std::unique_ptr<Block>>> &getBoard() {
        return board.getBoard();
    }

    Board &getBoardObject() {
        return board;
    }

    void update();
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_GAME_HPP_
