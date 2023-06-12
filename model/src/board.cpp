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

Board &Board::operator=(const Platformer::Board &other) {
    if (this == &other) {
        return *this;
    }
    height = other.height;
    width = other.width;
    board = std::vector<std::vector<std::unique_ptr<Block>>>();
    for (std::size_t row = 0; row < height; row++) {
        board.emplace_back();
        for (std::size_t col = 0; col < width; col++) {
            board[row].push_back(makeBlock(other.board[row][col]->name()));
        }
    }
    return *this;
}

Board::Board(const Platformer::Board &other) {
    *this = other;
}

const std::unique_ptr<Block> &Board::getBlock(sf::Vector2<std::size_t> pos) {
    if (0 <= pos.x && pos.x < width && 0 <= pos.y && pos.y < height) {
        return board[pos.y][pos.x];
    }
    return AIR_BLOCK;
}

const std::unique_ptr<Block> &Board::getBlockByCoordinates(sf::Vector2i pos) {
    return getBlock(
        {static_cast<std::size_t>(
             utilities::divide(static_cast<int>(pos.x), BLOCK_SIZE)
         ),
         static_cast<std::size_t>(
             utilities::divide(static_cast<int>(pos.y), BLOCK_SIZE)
         )}
    );
}

void Board::setBlock(sf::Vector2<std::size_t> pos, const std::string &name) {
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
