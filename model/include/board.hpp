#ifndef PLATFORMERGAMECPP_BOARD_HPP_
#define PLATFORMERGAMECPP_BOARD_HPP_

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "../../tools/utilities.hpp"
#include "block.hpp"

namespace Platformer {

class Board {
    int height = 0;
    int width = 0;
    std::vector<std::vector<std::unique_ptr<Block>>> board;

public:
    Board() = default;
    explicit Board(std::vector<std::vector<std::unique_ptr<Block>>> board_);
    explicit Board(const std::vector<std::vector<std::string>> &blockMap);

    std::vector<std::vector<std::unique_ptr<Block>>> &getBoard() {
        return board;
    }

    [[nodiscard]] sf::Vector2i getSize() const {
        return {width, height};
    }

    const std::unique_ptr<Block> &getBlock(sf::Vector2i pos);
    const std::unique_ptr<Block> &getBlockByCoordinates(sf::Vector2i pos);
    void addBlock(sf::Vector2u pos, const std::string &name);
    std::vector<std::vector<std::string>> getBlockMap();
};

}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BOARD_HPP_
