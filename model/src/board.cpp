#include "board.hpp"
#include "model-constants.hpp"

namespace Platformer {
Board::Board(std::vector<std::vector<std::unique_ptr<Block>>> board_)
    : board(std::move(board_)) {
    if (!board.empty()) {
        height = static_cast<int>(board.size());
        width = static_cast<int>(board[0].size());
        for (const auto &row : board) {
            if (static_cast<int>(row.size()) !=
                width) {  // Reset board if board is non-rectangular
                height = 0;
                width = 0;
                board = std::vector<std::vector<std::unique_ptr<Block>>>();
                return;
            }
        }
    }
}

Board::Board(const std::vector<std::vector<std::string>> &blockMap) {
    for (int row = 0; row < blockMap.size(); row++) {
        board.emplace_back();
        for (const std::string &blockName : blockMap[row]) {
            board[row].push_back(makeBlock(blockName));
        }
    }
    if (!board.empty()) {
        height = static_cast<int>(board.size());
        width = static_cast<int>(board[0].size());
        for (const auto &row : board) {
            if (static_cast<int>(row.size()) !=
                width) {  // Reset board if board is non-rectangular
                height = 0;
                width = 0;
                board = std::vector<std::vector<std::unique_ptr<Block>>>();
                return;
            }
        }
    }
}

const std::unique_ptr<Block> &Board::getBlock(sf::Vector2i pos) {
    if (0 <= pos.x && pos.x < width && 0 <= pos.y && pos.y < height) {
        return board[pos.y][pos.x];
    }
    return AIR_BLOCK;
}

const std::unique_ptr<Block> &Board::getBlockByCoordinates(sf::Vector2i pos) {
    return getBlock(
        {utilities::divide(pos.x, BLOCK_SIZE),
         utilities::divide(pos.y, BLOCK_SIZE)}
    );
}

void Board::addBlock(sf::Vector2u pos, const std::string &name) {
    board.at(pos.y).at(pos.x) = makeBlock(name);
}

std::vector<std::vector<std::string>> Board::getBlockMap() {
    std::vector<std::vector<std::string>> map(
        height, std::vector<std::string>(width)
    );
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            map[row][col] = board[row][col]->name();
        }
    }
    return map;
}
}  // namespace Platformer
