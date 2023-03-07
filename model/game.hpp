#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <memory>
#include <vector>
#include "block.hpp"
#include "player_fwd.hpp"

namespace Platformer {
class Game {
    int height;
    int width;
    std::vector<std::vector<Block>> field;
    std::unique_ptr<Player> player;
    int timer = 0;
    bool is_paused = false;

public:
    Game(int height_, int width_, std::vector<std::vector<Block>> field_);

    Player *getPlayer() {
        return player.get();
    }

    std::vector<std::vector<Block>> getField() {
        return field;
    }

    void act(char command);
    void update();
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_GAME_HPP_
