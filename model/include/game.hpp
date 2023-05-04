#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <memory>
#include <vector>
#include "../../tools/utilities.hpp"
#include "block.hpp"
#include "board.hpp"
#include "player_fwd.hpp"

namespace Platformer {

class Game {
    Board board;
    std::unique_ptr<Player> player;
    int timer = 0;
    bool is_paused = false;
    sf::Vector2i startPos;
    sf::Vector2i endPos;

public:
    ~Game();
    Game(Game &&other) = default;
    Game &operator=(Game &&other) = default;
    Game(
        std::vector<std::vector<std::unique_ptr<Block>>> board_,
        sf::Vector2i playerPos,
        sf::Vector2i endPos_
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

    sf::Vector2i getStartPos() {
        return startPos;
    }

    sf::Vector2i getEndPos() {
        return endPos;
    }

    void setStartPos(sf::Vector2i pos) {
        startPos = pos;
    }

    void setEndPos(sf::Vector2i pos) {
        endPos = pos;
    }

    void update();

    void writeToFile(const std::string &name, const std::string &filepath);
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_GAME_HPP_
