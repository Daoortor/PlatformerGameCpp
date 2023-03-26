#include "../include/level-render.hpp"
#include "../include/gui-constants.hpp"

namespace Platformer::gui {
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
    Game &game,
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures
) {
    utilities::Vector boardSize = game.getBoardObject().getSize();
    int boardWidth = boardSize.get_x();
    int boardHeight = boardSize.get_y();
    float blockSize = std::min(
        static_cast<float>(LEVEL_WIDTH) / static_cast<float>(boardWidth),
        static_cast<float>(LEVEL_HEIGHT) / static_cast<float>(boardHeight)
    );
    auto &board = game.getBoard();

    sf::Vector2f windowCenter{
        static_cast<float>(WINDOW_WIDTH) / 2,
        static_cast<float>(WINDOW_HEIGHT) / 2};
    sf::Vector2f topLeftCorner{
        windowCenter.x - (blockSize * static_cast<float>(boardWidth)) / 2,
        windowCenter.y - (blockSize * static_cast<float>(boardHeight)) / 2};

    std::vector<std::vector<sf::Sprite>> spriteMatrix(
        boardHeight, std::vector<sf::Sprite>(boardWidth)
    );
    for (int row = 0; row < boardHeight; row++) {
        for (int col = 0; col < boardWidth; col++) {
            std::string blockName = board[row][col]->name();
            sf::Vector2f pos = {
                topLeftCorner.x + blockSize * static_cast<float>(col),
                topLeftCorner.y + blockSize * static_cast<float>(row)};
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

levelWindow::levelWindow(
    unsigned int windowHeight,
    const std::string &backgroundTextureFilepath,
    const std::string &levelFilepath,
    control::LevelOverlord &overlord
)
    : overlord(overlord) {
    overlord.setLevel(Game(levelFilepath));
    backgroundTexture.loadFromFile(backgroundTextureFilepath);
    sf::Vector2u TextureSize = backgroundTexture.getSize();
    float BackgroundScale =
        static_cast<float>(windowHeight) / static_cast<float>(TextureSize.y);
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(BackgroundScale, BackgroundScale);

    for (const std::string &blockType : Platformer::gui::BLOCK_NAMES) {
        blockTextures[blockType] = Platformer::gui::makeBlockTexture(blockType);
    }
    boardSprites =
        Platformer::gui::makeBlockSprites(overlord.getLevel(), blockTextures);
}

void levelWindow::loadInWindow(sf::RenderWindow &window) {
    window.clear();
    window.draw(backgroundSprite);
    for (const auto &row : boardSprites) {
        for (const auto &sprite : row) {
            window.draw(sprite);
        }
    }
    window.display();
}
}  // namespace Platformer::gui
