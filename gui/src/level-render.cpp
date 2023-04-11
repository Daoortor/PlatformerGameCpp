#include "../include/level-render.hpp"
#include <iostream>
#include "../include/gui-constants.hpp"

namespace Platformer::gui {

float getBlockSize(std::unique_ptr<Platformer::Game> &game) {
    utilities::Vector boardSize = game->getBoardObject().getSize();
    int boardWidth = boardSize.get_x();
    int boardHeight = boardSize.get_y();
    return std::min(
        static_cast<float>(LEVEL_WIDTH) / static_cast<float>(boardWidth),
        static_cast<float>(LEVEL_HEIGHT) / static_cast<float>(boardHeight)
    );
}

sf::Vector2f getTopLeftCorner(std::unique_ptr<Platformer::Game> &game) {
    utilities::Vector boardSize = game->getBoardObject().getSize();
    int boardWidth = boardSize.get_x();
    int boardHeight = boardSize.get_y();
    float blockSize = std::min(
        static_cast<float>(LEVEL_WIDTH) / static_cast<float>(boardWidth),
        static_cast<float>(LEVEL_HEIGHT) / static_cast<float>(boardHeight)
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
        static_cast<float>(game->getPlayer()->getSize().get_x()) *
            getBlockSize(game) / BLOCK_SIZE,
        static_cast<float>(game->getPlayer()->getSize().get_y()) *
            getBlockSize(game) / BLOCK_SIZE};
}

sf::Vector2f getCoordinates(utilities::Vector pos,
                            std::unique_ptr<Platformer::Game> &game) {
    float scale = getBlockSize(game) / BLOCK_SIZE;
    sf::Vector2f topLeftCorner = getTopLeftCorner(game);
    float xPos =
        topLeftCorner.x +
        static_cast<float>(pos.get_x()) * scale;
    float yPos =
        topLeftCorner.y +
        static_cast<float>(pos.get_y()) * scale;
    return {xPos, yPos};
}

sf::Vector2f getPlayerCoordinates(std::unique_ptr<Platformer::Game> &game) {
    return getCoordinates(game->getPlayer()->getPos(), game) -
    sf::Vector2f(getPlayerSize(game).x / 2, getPlayerSize(game).y / 2);
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
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures
) {
    utilities::Vector boardSize = game->getBoardObject().getSize();
    int boardWidth = boardSize.get_x();
    int boardHeight = boardSize.get_y();
    float blockSize = std::min(
        static_cast<float>(LEVEL_WIDTH) / static_cast<float>(boardWidth),
        static_cast<float>(LEVEL_HEIGHT) / static_cast<float>(boardHeight)
    );
    auto &board = game->getBoard();

    sf::Vector2f topLeftCorner = getTopLeftCorner(game);
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

LevelWindow::LevelWindow(
    unsigned int windowHeight,
    const std::string &backgroundTextureFilepath,
    const std::string &playerFilepath,
    const std::string &miscFilepath,
    const std::string &levelFilepath,
    control::LevelPerformer *levelPerformer
)
    : levelPerformerPtr(levelPerformer) {
    levelPerformerPtr->setLevel(std::make_unique<Game>(levelFilepath));
    backgroundTexture.loadFromFile(backgroundTextureFilepath);
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float backgroundScale =
        static_cast<float>(windowHeight) / static_cast<float>(textureSize.y);
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(backgroundScale, backgroundScale);

    for (const std::string &blockType : Platformer::gui::BLOCK_NAMES) {
        blockTextures[blockType] = Platformer::gui::makeBlockTexture(blockType);
    }
    boardSprites = Platformer::gui::makeBlockSprites(
        levelPerformerPtr->getLevel(), blockTextures
    );
    playerTextures[Platformer::Pose::LOOKING_LEFT] = sf::Texture();
    playerTextures[Platformer::Pose::LOOKING_LEFT].loadFromFile(
        playerFilepath + "/player-left.png"
    );
    playerTextures[Platformer::Pose::LOOKING_RIGHT] = sf::Texture();
    playerTextures[Platformer::Pose::LOOKING_RIGHT].loadFromFile(
        playerFilepath + "/player-right.png"
    );
    playerSprite.setTexture(
        playerTextures[levelPerformerPtr->getLevel()->getPlayer()->getPose()]
    );
    float playerScale =
        getPlayerSize(levelPerformerPtr->getLevel()).y /
        static_cast<float>(playerSprite.getTexture()->getSize().y);
    playerSprite.setScale(playerScale, playerScale);
    sf::Vector2f playerCoordinates =
        getPlayerCoordinates(levelPerformerPtr->getLevel());
    playerSprite.setPosition(playerCoordinates.x, playerCoordinates.y);
    levelEndTexture.loadFromFile(miscFilepath + "/level-end.png");
    levelEndSprite.setTexture(levelEndTexture);
    float levelEndScale = static_cast<float>(
                              getBlockSize(levelPerformer.getLevel())) / 2 /
              static_cast<float>(levelEndSprite.getTexture()->getSize().y);
    levelEndSprite.setScale(levelEndScale, levelEndScale);
    sf::Vector2f offset = sf::Vector2f(getBlockSize(levelPerformer.getLevel()) / 4,
                                       getBlockSize(levelPerformer.getLevel()) / 4);
    sf::Vector2f levelEndPos = getCoordinates(levelPerformer.getLevel()->getEndPos(),
                                      levelPerformer.getLevel());
    levelEndSprite.setPosition(levelEndPos - offset);
}

void LevelWindow::loadInWindow(sf::RenderWindow &window) {
    window.clear();
    if (levelPerformer.getLevel()->getPlayer()->contains(
            levelPerformer.getLevel()->getEndPos())) {
        levelPerformer.setState(control::LevelState::Won);
        return;
    }
    if (levelPerformer.getState() == control::LevelState::Running) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            levelPerformerPtr->jump();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            levelPerformerPtr->moveLeft();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            levelPerformerPtr->moveDown();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            levelPerformerPtr->moveRight();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            levelPerformerPtr->pause();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) {
            levelPerformer.reset();
        }
    } else if (levelPerformer.getState() == control::LevelState::Paused) {
        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            levelPerformerPtr.resume();
            std::cout << "Game resumed\n" << '\n';
        } // dubs as 'Resume' button in Pause menu
         */
        // TODO: some kind of timer to check there is no loop pause-resume,
        //  as is right now; or should actually rewrite button to isKeyReleased
    }
    backgroundSprite.setTexture(backgroundTexture);
    // TODO: resolve texture loss that occurs after removing line above
    window.draw(backgroundSprite);
    for (const auto &row : boardSprites) {
        for (const auto &sprite : row) {
            window.draw(sprite);
        }
    }
    levelPerformerPtr->getLevel()->update();
    sf::Vector2f playerCoordinates =
        getPlayerCoordinates(levelPerformerPtr->getLevel());
    playerSprite.setPosition(playerCoordinates.x, playerCoordinates.y);
    playerSprite.setTexture(
        playerTextures[levelPerformerPtr->getLevel()->getPlayer()->getPose()]
    );
    window.draw(levelEndSprite);
    window.draw(playerSprite);
    window.display();
}
}  // namespace Platformer::gui
