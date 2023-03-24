#ifndef PLATFORMERGAMECPP_BOARD_HPP_
#define PLATFORMERGAMECPP_BOARD_HPP_

#include <memory>
#include <vector>
#include "block.hpp"
#include "utilities.hpp"

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
    [[nodiscard]] utilities::Vector getSize() const {
        return {width, height};
    }

    const std::unique_ptr<Block> &getBlock(utilities::Vector pos);
    const std::unique_ptr<Block> &getBlockByCoordinates(utilities::Vector pos);
};

}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BOARD_HPP_