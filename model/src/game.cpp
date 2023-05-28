#include "game.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>
#include <utility>
#include "../../tools/json.hpp"
#include "player.hpp"

using json = nlohmann::json;

namespace Platformer {
const char *FileNotFoundError::what() const noexcept {
    return message.c_str();
}

Game::~Game() = default;

void Game::update() {
    player->notifyAll();
}

Game &Game::operator=(const Platformer::Game &other) {
    if (this == &other) {
        return *this;
    }
    timer = other.timer;
    startPos = other.startPos;
    endPos = other.endPos;
    board = other.board;
    player = std::make_unique<Player>(this, startPos);
    return *this;
}

Game::Game(const Platformer::Game &other) {
    *this = other;
}

Game::Game(
    std::vector<std::vector<std::unique_ptr<Block>>> board_,
    sf::Vector2i playerPos,
    sf::Vector2i endPos_
)
    : board(std::move(board_)), startPos(playerPos), endPos(endPos_) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
}

Game::Game(const std::string &filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        throw FileNotFoundError(filename);
    }
    json levelData = json::parse(f);
    std::vector<std::vector<std::string>> blockMap = levelData["blockMap"];
    sf::Vector2i playerPos = {
        levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    board = Board(blockMap);
    startPos = {levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
}

void Game::writeToFile(const std::string &name, const std::string &filepath) {
    json levelData;
    levelData["levelName"] = name;
    levelData["width"] = board.getSize().x;
    levelData["height"] = board.getSize().y;
    levelData["blockMap"] = board.getBlockMap();
    levelData["playerStartPos"] = {startPos.x, startPos.y};
    levelData["endPos"] = {endPos.x, endPos.y};
    std::ofstream out(filepath + name + ".json");
    out << std::setw(4) << levelData << std::endl;
}

void Game::enlarge(
    std::size_t deltaTop,
    std::size_t deltaBottom,
    std::size_t deltaLeft,
    std::size_t deltaRight
) {
    std::vector<std::vector<std::unique_ptr<Block>>> newBoard;
    for (std::size_t row = 0; row < board.getSize().y + deltaTop + deltaBottom;
         row++) {
        newBoard.emplace_back();
        for (std::size_t col = 0;
             col < board.getSize().x + deltaLeft + deltaRight; col++) {
            if (deltaTop <= row && row < board.getSize().y + deltaTop &&
                deltaLeft <= col && col < board.getSize().x + deltaLeft) {
                newBoard[row].push_back(std::move(board.getBoard(
                )[row - deltaTop][col - deltaLeft]));
            } else {
                newBoard[row].push_back(makeBlock("air"));
            }
        }
    }
    board.getBoard() = std::move(newBoard);
    board.setSize(
        board.getSize().y + deltaTop + deltaBottom,
        board.getSize().x + deltaLeft + deltaRight
    );
    startPos +=
        static_cast<sf::Vector2i>(sf::Vector2<std::size_t>(deltaLeft, deltaTop)
        ) *
        BLOCK_SIZE;
    endPos +=
        static_cast<sf::Vector2i>(sf::Vector2<std::size_t>(deltaLeft, deltaTop)
        ) *
        BLOCK_SIZE;
}

void Game::enlarge(
    const std::unique_ptr<Game> &gameCopy,
    std::size_t deltaTopCopy,
    std::size_t deltaBottomCopy,
    std::size_t deltaLeftCopy,
    std::size_t deltaRightCopy
) {
    *this = *gameCopy;
    this->crop(deltaTopCopy, deltaBottomCopy, deltaLeftCopy, deltaRightCopy);
}

void Game::crop(
    std::size_t deltaTop,
    std::size_t deltaBottom,
    std::size_t deltaLeft,
    std::size_t deltaRight
) {
    std::vector<std::vector<std::unique_ptr<Block>>> newBoard;
    for (std::size_t row = 0; row < board.getSize().y - deltaTop - deltaBottom;
         row++) {
        newBoard.emplace_back();
        for (std::size_t col = 0;
             col < board.getSize().x - deltaLeft - deltaRight; col++) {
            newBoard[row].push_back(std::move(board.getBoard(
            )[row + deltaTop][col + deltaLeft]));
        }
    }
    board.getBoard() = std::move(newBoard);
    board.setSize(
        board.getSize().y - deltaTop - deltaBottom,
        board.getSize().x - deltaLeft - deltaRight
    );
    startPos -=
        static_cast<sf::Vector2i>(sf::Vector2<std::size_t>(deltaLeft, deltaTop)
        ) *
        BLOCK_SIZE;
    endPos -=
        static_cast<sf::Vector2i>(sf::Vector2<std::size_t>(deltaLeft, deltaTop)
        ) *
        BLOCK_SIZE;
}

void Game::partialCopy(
    const std::unique_ptr<Game> &other,
    std::size_t deltaTop,
    std::size_t deltaLeft
) {
    auto copySize = other->board.getSize();
    for (std::size_t row = deltaTop; row < deltaTop + copySize.y; row++) {
        for (std::size_t col = deltaLeft; col < deltaLeft + copySize.x; col++) {
            board.setBlock(
                {col, row},
                other->board.getBoard()[row - deltaTop][col - deltaLeft]->name()
            );
        }
    }
}
}  // namespace Platformer
