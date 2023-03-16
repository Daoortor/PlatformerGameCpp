#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <memory>
#include <vector>
#include "block.hpp"
#include "player_fwd.hpp"
#include "utilities.hpp"

namespace Platformer {
class Game {
    int height;
    int width;
    std::vector<std::vector<std::unique_ptr<Block>>> field;
    std::unique_ptr<Player> player;
    int timer = 0;
    bool is_paused = false;
    utilities::Vector endPos;

public:
    ~Game();
    Game(
        std::vector<std::vector<std::unique_ptr<Block>>> field_,
        utilities::Vector playerPos,
        utilities::Vector endPos_
    );
    explicit Game(const std::string &filename);

    Player *getPlayer() {
        return player.get();
    }

    std::vector<std::vector<std::unique_ptr<Block>>> &getField() {
        return field;
    }

    const std::unique_ptr<Block> &getBlock(utilities::Vector pos);
    const std::unique_ptr<Block> &getBlockByCoordinates(utilities::Vector pos);
    void act(char command);
    void update();
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_GAME_HPP_
