#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <memory>
#include <vector>
#include "block.hpp"
#include "board.hpp"
#include "player_fwd.hpp"
#include "../../tools/utilities.hpp"

namespace Platformer {
class Game {
    Board board;
    std::unique_ptr<Player> player;
    int timer = 0;
    bool is_paused = false;
    utilities::Vector endPos;

public:
    ~Game();
    Game(Game&& other) = default;
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
