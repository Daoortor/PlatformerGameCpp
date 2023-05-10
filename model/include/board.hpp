#ifndef PLATFORMERGAMECPP_BOARD_HPP_
#define PLATFORMERGAMECPP_BOARD_HPP_

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "../../tools/utilities.hpp"
#include "block.hpp"

namespace Platformer {

class Board {
    std::size_t height = 0;
    std::size_t width = 0;
    std::vector<std::vector<std::unique_ptr<Block>>> board;

public:
    Board() = default;
    Board &operator=(const Board &other);
    Board(const Board &other);
    explicit Board(std::vector<std::vector<std::unique_ptr<Block>>> board_);
    explicit Board(const std::vector<std::vector<std::string>> &blockMap);

    std::vector<std::vector<std::unique_ptr<Block>>> &getBoard() {
        return board;
    }

    [[nodiscard]] sf::Vector2<std::size_t> getSize() const {
        return {width, height};
    }

    void setSize(std::size_t newHeight, std::size_t newWidth) {
        height = newHeight;
        width = newWidth;
    }

    const std::unique_ptr<Block> &getBlock(sf::Vector2<std::size_t> pos);
    const std::unique_ptr<Block> &getBlockByCoordinates(sf::Vector2i pos);
    void setBlock(sf::Vector2<std::size_t> pos, const std::string &name);
    std::vector<std::vector<std::string>> getBlockMap();
};

}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BOARD_HPP_
