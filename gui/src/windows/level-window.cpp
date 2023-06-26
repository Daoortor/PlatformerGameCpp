#include "windows/level-window.hpp"
#include <cmath>
#include <iostream>
#include <memory>

namespace Platformer::gui {

float getBlockSize(std::unique_ptr<Platformer::Game> &game) {
    auto boardSize = game->getBoardObject().getSize();
    std::size_t boardWidth = boardSize.x;
    std::size_t boardHeight = boardSize.y;
    return std::min(
        static_cast<float>(levels::LEVEL_WIDTH) /
            static_cast<float>(boardWidth),
        static_cast<float>(levels::LEVEL_HEIGHT) /
            static_cast<float>(boardHeight)
    );
}

sf::Vector2f getGameFrameSize(std::unique_ptr<Platformer::Game> &game) {
    return {
        static_cast<float>(game->getBoardObject().getSize().x) *
            getBlockSize(game),
        static_cast<float>(game->getBoardObject().getSize().y) *
            getBlockSize(game)};
}

sf::Vector2f getTopLeftCorner(std::unique_ptr<Platformer::Game> &game) {
    auto boardSize = game->getBoardObject().getSize();
    std::size_t boardWidth = boardSize.x;
    std::size_t boardHeight = boardSize.y;
    float blockSize = std::min(
        static_cast<float>(levels::LEVEL_WIDTH) /
            static_cast<float>(boardWidth),
        static_cast<float>(levels::LEVEL_HEIGHT) /
            static_cast<float>(boardHeight)
    );
    sf::Vector2f windowCenter{
        static_cast<float>(WINDOW_WIDTH) / 2,
        static_cast<float>(WINDOW_HEIGHT) / 2};
    return {
        windowCenter.x - (blockSize * static_cast<float>(boardWidth)) / 2,
        windowCenter.y - (blockSize * static_cast<float>(boardHeight)) / 2};
}

sf::Vector2f getPlayerSize(std::unique_ptr<Platformer::Game> &game) {
    return {
        static_cast<float>(game->getPlayer()->getSize().x) *
            getBlockSize(game) / BLOCK_SIZE,
        static_cast<float>(game->getPlayer()->getSize().y) *
            getBlockSize(game) / BLOCK_SIZE};
}

sf::Vector2f
getCoordinates(sf::Vector2i pos, std::unique_ptr<Platformer::Game> &game) {
    float scale = getBlockSize(game) / BLOCK_SIZE;
    sf::Vector2f topLeftCorner = getTopLeftCorner(game);
    float xPos = topLeftCorner.x + static_cast<float>(pos.x) * scale;
    float yPos = topLeftCorner.y + static_cast<float>(pos.y) * scale;
    return {xPos, yPos};
}

sf::Vector2f getPlayerCoordinates(std::unique_ptr<Platformer::Game> &game) {
    return getCoordinates(game->getPlayer()->getPos(), game) -
           sf::Vector2f(getPlayerSize(game).x / 2, getPlayerSize(game).y / 2);
}

sf::Vector2i getInGameCoordinates(
    sf::Vector2f pos,
    std::unique_ptr<Platformer::Game> &game
) {
    sf::Vector2f convertedFloatCoordinates =
        (pos - getTopLeftCorner(game)) / getBlockSize(game);
    sf::Vector2i convertedCoordinates = {
        static_cast<int>(std::floor(convertedFloatCoordinates.x)),
        static_cast<int>(std::floor(convertedFloatCoordinates.y))};
    auto boardSize = game->getBoardObject().getSize();
    if (0 <= convertedCoordinates.x && convertedCoordinates.x < boardSize.x &&
        0 <= convertedCoordinates.y && convertedCoordinates.y < boardSize.y) {
        return convertedCoordinates;
    }
    return {-1, -1};
}

sf::Sprite makeBlockSprite(
    const std::unique_ptr<sf::Texture> &blockTexture,
    float size,
    sf::Vector2f pos
) {
    sf::Vector2u textureSize = blockTexture->getSize();
    float blockScale = size / static_cast<float>(textureSize.y);
    sf::Sprite blockSprite;
    blockSprite.setTexture(*blockTexture);
    blockSprite.setScale(blockScale, blockScale);
    blockSprite.setPosition(pos.x, pos.y);
    return blockSprite;
}

std::vector<std::vector<sf::Sprite>> makeBlockSprites(
    std::unique_ptr<Platformer::Game> &game,
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures,
    sf::Vector2f offset = {0, 0}
) {
    auto boardSize = game->getBoardObject().getSize();
    std::size_t boardWidth = boardSize.x;
    std::size_t boardHeight = boardSize.y;
    float blockSize = std::min(
        static_cast<float>(levels::LEVEL_WIDTH) /
            static_cast<float>(boardWidth),
        static_cast<float>(levels::LEVEL_HEIGHT) /
            static_cast<float>(boardHeight)
    );
    auto &board = game->getBoard();

    sf::Vector2f topLeftCorner = getTopLeftCorner(game);
    std::vector<std::vector<sf::Sprite>> spriteMatrix(
        boardHeight, std::vector<sf::Sprite>(boardWidth)
    );
    for (std::size_t row = 0; row < boardHeight; row++) {
        for (std::size_t col = 0; col < boardWidth; col++) {
            std::string blockName = board[row][col]->name();
            sf::Vector2f pos =
                offset +
                sf::Vector2f(
                    topLeftCorner.x + blockSize * static_cast<float>(col),
                    topLeftCorner.y + blockSize * static_cast<float>(row)
                );
            spriteMatrix[row][col] =
                makeBlockSprite(blockTextures[blockName], blockSize, pos);
        }
    }
    return spriteMatrix;
}

std::unique_ptr<sf::Texture> makeBlockTexture(const std::string &type) {
    std::unique_ptr<sf::Texture> blockTexture = std::make_unique<sf::Texture>();
    blockTexture->loadFromFile(
        "../gui/assets/textures/blocks/" + type + ".png"
    );
    return blockTexture;
}

LevelWindow::LevelWindow(
    unsigned int windowHeight,
    const std::string &backgroundTextureFilepath,
    const std::string &miscFilepath,
    const std::string &levelFilepath = "",
    sf::Vector2f offset_
)
    : offset(offset_) {
    backgroundTexture.loadFromFile(backgroundTextureFilepath);
    backgroundSprite.setTexture(backgroundTexture);
    for (const std::string &blockType : levels::BLOCK_NAMES) {
        blockTextures[blockType] = Platformer::gui::makeBlockTexture(blockType);
    }
    auto game = std::make_unique<Game>(levelFilepath);
    updateAll(game, miscFilepath);
}

void LevelWindow::loadLevel(
    sf::RenderWindow &window,
    std::unique_ptr<Platformer::Game> &game
) {
    backgroundSprite.setTexture(backgroundTexture);
    // line above ensures no texture loss
    window.draw(backgroundSprite);
    for (const auto &row : boardSprites) {
        for (const auto &sprite : row) {
            window.draw(sprite);
        }
    }
    levelBeginSprite.setTexture(levelBeginTexture);
    window.draw(levelBeginSprite);
    levelEndSprite.setTexture(levelEndTexture);
    window.draw(levelEndSprite);
}

void LevelWindow::updateAll(
    std::unique_ptr<Platformer::Game> &game,
    const std::string &miscFilepath
) {
    sf::Vector2f startEndOffset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    boardSprites =
        Platformer::gui::makeBlockSprites(game, blockTextures, offset);
    levelBeginTexture.loadFromFile(miscFilepath + "level-begin.png");
    float levelBeginEndScale =
        static_cast<float>(getBlockSize(game)) / 2 /
        static_cast<float>(levelBeginTexture.getSize().y);
    levelBeginSprite.setTexture(levelBeginTexture);
    levelBeginSprite.setScale(levelBeginEndScale, levelBeginEndScale);
    sf::Vector2f levelBeginPos = getCoordinates(game->getStartPos(), game);
    levelBeginSprite.setPosition(levelBeginPos + offset - startEndOffset);

    levelEndTexture.loadFromFile(miscFilepath + "level-end.png");
    levelEndSprite.setTexture(levelEndTexture);
    levelEndSprite.setScale(levelBeginEndScale, levelBeginEndScale);
    sf::Vector2f levelEndPos = getCoordinates(game->getEndPos(), game);
    levelEndSprite.setPosition(levelEndPos + offset - startEndOffset);
}
}  // namespace Platformer::gui
