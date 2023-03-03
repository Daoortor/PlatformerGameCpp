#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include "player_fwd.hpp"
#include "block.hpp"
#include <vector>

namespace Platformer {
    class Game {
        int height;
        int width;
        std::vector<std::vector<Block>> field;
        Player* player;
        int timer = 0;
        bool is_paused = false;
    };
}

#endif  // PLATFORMERGAMECPP_GAME_HPP_
