#ifndef PLATFORMERGAMECPP_GAME_HPP_
#define PLATFORMERGAMECPP_GAME_HPP_

#include <exception>
#include <memory>
#include <vector>
#include "../../tools/utilities.hpp"
#include "block.hpp"
#include "board.hpp"
#include "player_fwd.hpp"

namespace Platformer {

class FileNotFoundError : public std::exception {
    std::string message;

public:
    explicit FileNotFoundError(std::string filename)
        : message("Level " + std::move(filename) + " not found"){};
    [[nodiscard]] const char *what() const noexcept override;
};

class Game {
    Board board;
    std::unique_ptr<Player> player;
    int timer = 0;
    sf::Vector2i startPos;
    sf::Vector2i endPos;

public:
    ~Game();
    Game(Game &&other) = default;
    Game &operator=(Game &&other) = default;
    Game(const Game &other);
    Game &operator=(const Game &other);
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
    void enlarge(
        std::size_t deltaTop,
        std::size_t deltaBottom,
        std::size_t deltaLeft,
        std::size_t deltaRight
    );
    void enlarge(
        const std::unique_ptr<Game> &gameCopy,
        std::size_t deltaTopCopy,
        std::size_t deltaBottomCopy,
        std::size_t deltaLeftCopy,
        std::size_t deltaRightCopy
    );
    void crop(
        std::size_t deltaTop,
        std::size_t deltaBottom,
        std::size_t deltaLeft,
        std::size_t deltaRight
    );
    void partialCopy(
        const std::unique_ptr<Game> &other,
        std::size_t deltaTop,
        std::size_t deltaLeft
    );
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_GAME_HPP_
