#include "level-render.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include "gui-constants.hpp"
#include "model-constants.hpp"

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
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures
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
    const std::string &miscFilepath,
    const std::string &levelFilepath = ""
) {
    backgroundTexture.loadFromFile(backgroundTextureFilepath);
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float backgroundScale =
        static_cast<float>(windowHeight) / static_cast<float>(textureSize.y);
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(backgroundScale, backgroundScale);

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
    // TODO: resolve texture loss that occurs after removing line above
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
    sf::Vector2f offset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    boardSprites = Platformer::gui::makeBlockSprites(game, blockTextures);
    levelBeginTexture.loadFromFile(miscFilepath + "level-begin.png");
    float levelBeginEndScale =
        static_cast<float>(getBlockSize(game)) / 2 /
        static_cast<float>(levelBeginTexture.getSize().y);
    levelBeginSprite.setTexture(levelBeginTexture);
    levelBeginSprite.setScale(levelBeginEndScale, levelBeginEndScale);
    sf::Vector2f levelBeginPos = getCoordinates(game->getStartPos(), game);
    levelBeginSprite.setPosition(levelBeginPos - offset);

    levelEndTexture.loadFromFile(miscFilepath + "level-end.png");
    levelEndSprite.setTexture(levelEndTexture);
    levelEndSprite.setScale(levelBeginEndScale, levelBeginEndScale);
    sf::Vector2f levelEndPos = getCoordinates(game->getEndPos(), game);
    levelEndSprite.setPosition(levelEndPos - offset);
}

LevelGameplayWindow::LevelGameplayWindow(
    unsigned int windowHeight,
    const std::string &backgroundTextureFilepath,
    const std::string &playerFilepath,
    const std::string &miscFilepath,
    const std::string &levelFilepath,
    control::LevelPerformer *levelPerformerPtr_
)
    : levelPerformerPtr(levelPerformerPtr_),
      LevelWindow(
          windowHeight,
          backgroundTextureFilepath,
          miscFilepath,
          levelFilepath
      ) {
    levelPerformerPtr->setLevel(std::make_unique<Game>(levelFilepath));
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
}

void LevelGameplayWindow::loadInWindow(sf::RenderWindow &window) {
    window.clear();
    if (levelPerformerPtr->getLevel()->getPlayer()->contains(
            levelPerformerPtr->getLevel()->getEndPos()
        )) {
        levelPerformerPtr->setState(control::LevelState::Won);
        return;
    }
    if (levelPerformerPtr->getState() == control::LevelState::Running) {
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
            levelPerformerPtr->reset();
        }
    } else if (levelPerformerPtr->getState() == control::LevelState::Paused) {
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
    loadLevel(window, levelPerformerPtr->getLevel());
    levelPerformerPtr->getLevel()->update();
    sf::Vector2f playerCoordinates =
        getPlayerCoordinates(levelPerformerPtr->getLevel());
    playerSprite.setPosition(playerCoordinates.x, playerCoordinates.y);
    playerSprite.setTexture(
        playerTextures[levelPerformerPtr->getLevel()->getPlayer()->getPose()]
    );
    levelEndSprite.setTexture(levelEndTexture);
    window.draw(levelEndSprite);
    // TODO: resolve texture loss that occurs after removing line above
    window.draw(playerSprite);
    window.display();
}

LevelEditor::LevelEditor(
    unsigned int windowHeight,
    control::LevelPerformer *levelPerformerPtr_,
    control::MenuPerformer *menuPerformerPtr_,
    const std::string &backgroundTextureFilepath,
    const std::string &blockFilepath,
    const std::string &miscFilepath,
    const std::string &emptyLevelFilepath,
    const std::string &levelsFilepath,
    const sf::Font &font
)
    : LevelWindow(
          windowHeight,
          backgroundTextureFilepath,
          miscFilepath,
          emptyLevelFilepath
      ),
      levelPerformerPtr(levelPerformerPtr_),
      menuPerformerPtr(menuPerformerPtr_),
      game(std::make_unique<Game>(emptyLevelFilepath)),
      gameFullCopy(std::make_unique<Game>(*game)),
      blockSelectionBar(
          {30, getTopLeftCorner(game).y},
          {40, 40},
          {10, 20},
          20,
          40,
          2,
          colors::BUTTON_COLORS_LIST,
          colors::ITEM_CHOSEN_COLOR,
          miscFilepath
      ),
      state(EditorState::Idle),
      levelNameTextbox(
          sf::RectangleShape({150, 40}),
          "Level name...",
          font,
          20,
          colors::TEXTBOX_COLORS_LIST[0],
          colors::TEXTBOX_COLORS_LIST[1],
          colors::TEXTBOX_COLORS_LIST[2],
          colors::TEXTBOX_COLORS_LIST[3],
          colors::TEXTBOX_COLORS_LIST[4],
          colors::TEXTBOX_COLORS_LIST[5],
          {370, 520},
          {10, 10},
          13
      ),
      saveButton(
          sf::RectangleShape({100, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("Save", font, 20),
          {10, 10},
          {540, 520},
          [&, levelsFilepath]() {
              game->writeToFile(levelNameTextbox.getText(), levelsFilepath);
          }
      ),
      startPosButton(
          miscFilepath + "level-begin.png",
          sf::RectangleShape({40, 40}),
          {0, 0},
          {40, 348},
          std::vector<sf::Color>(4, sf::Color::Transparent),
          [this]() {
              this->state = EditorState::StartPosChosen;
              this->deactivateAll();
              this->startPosButton.activate();
          },
          true,
          10,
          colors::CHOSEN_COLOR
      ),
      levelEndButton(
          miscFilepath + "level-end.png",
          sf::RectangleShape({40, 40}),
          {0, 0},
          {40, 430},
          std::vector<sf::Color>(4, sf::Color::Transparent),
          [this]() {
              this->state = EditorState::EndPosChosen;
              this->deactivateAll();
              this->levelEndButton.activate();
          },
          true,
          10,
          colors::CHOSEN_COLOR
      ),
      backToMenuButton(
          sf::RectangleShape({135, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("Back to menu", font, 20),
          {10, 10},
          {30, 520},
          [this]() {
              levelPerformerPtr->exit();
              menuPerformerPtr->openMainMenu();
          }
      ),
      resetButton(
          sf::RectangleShape({65, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("Reset", font, 20),
          {10, 10},
          {185, 520},
          [this, emptyLevelFilepath, miscFilepath]() {
              this->game = std::make_unique<Game>(emptyLevelFilepath);
              this->updateAll(this->game, miscFilepath);
          }
      ),
      plusButton(
          sf::RectangleShape({30, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("+", font, 20),
          {10, 10},
          {270, 520},
          [this, miscFilepath]() {
              auto levelSize = this->game->getBoardObject().getSize();
              if (levelSize.y == levels::INITIAL_LEVEL_BLOCK_HEIGHT) {
                  return;
              }
              auto newHeight = levelSize.y - 1;
              auto newWidth = static_cast<std::size_t>(
                  static_cast<float>(newHeight) *
                  (static_cast<float>(levels::LEVEL_WIDTH) /
                   static_cast<float>(levels::LEVEL_HEIGHT))
              );
              if (gameFullCopy->getBoardObject().getSize().y < levelSize.y) {
                  this->gameFullCopy = std::make_unique<Game>(*game);
              }
              this->game->crop(
                  levelSize.y - newHeight, levelSize.y - newHeight,
                  levelSize.x - newWidth, levelSize.x - newWidth
              );
              this->updateAll(this->game, miscFilepath);
          }
      ),
      minusButton(
          sf::RectangleShape({30, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("-", font, 20),
          {10, 10},
          {320, 520},
          [this, miscFilepath]() {
              auto levelSize = this->game->getBoardObject().getSize();
              auto deltaHeight = 1;
              auto deltaWidth = static_cast<std::size_t>(
                                    static_cast<float>(levelSize.y + 1) *
                                    (static_cast<float>(levels::LEVEL_WIDTH) /
                                     static_cast<float>(levels::LEVEL_HEIGHT))
                                ) -
                                levelSize.x;
              auto copySize = this->gameFullCopy->getBoardObject().getSize();
              if (levelSize.y + deltaHeight <= copySize.y &&
                  levelSize.x + deltaWidth <= copySize.x) {
                  auto deltaHeightCopy =
                      (copySize.y - levelSize.y) / 2 - deltaHeight;
                  auto deltaWidthCopy =
                      (copySize.x - levelSize.x) / 2 - deltaWidth;
                  this->game->enlarge(
                      this->gameFullCopy, deltaHeightCopy, deltaHeightCopy,
                      deltaWidthCopy, deltaWidthCopy
                  );
              } else {
                  this->game->enlarge(
                      deltaHeight, deltaHeight, deltaWidth, deltaWidth
                  );
              }
              this->updateAll(this->game, miscFilepath);
          }
      ) {
    for (std::size_t blockNum = 0; blockNum < levels::BLOCK_NAMES.size();
         blockNum++) {
        interface::ButtonWithImage newButton(
            blockFilepath + levels::BLOCK_NAMES[blockNum] + ".png",
            sf::RectangleShape({40, 40}), {0, 0}, {0, 0},
            std::vector<sf::Color>(4, sf::Color::Transparent),
            [&, blockNum]() {
                this->deactivateAll();
                this->blockChosen = levels::BLOCK_NAMES[blockNum];
                blockSelectionBar.chooseItem(blockNum);
                this->state = EditorState::BlockChosen;
            },
            true, 10, colors::CHOSEN_COLOR
        );
        blockSelectionBar.addItem(newButton);
    }
    levelBorder = sf::RectangleShape(
        {static_cast<float>(levels::LEVEL_WIDTH),
         static_cast<float>(levels::LEVEL_HEIGHT)}
    );
    levelBorder.setPosition(
        {(static_cast<float>(WINDOW_WIDTH) - levelBorder.getSize().x) / 2,
         (static_cast<float>(WINDOW_HEIGHT) - levelBorder.getSize().y) / 2}
    );
    levelBorder.setFillColor(sf::Color::Transparent);
    levelBorder.setOutlineThickness(1.f);
    levelBorder.setOutlineColor(sf::Color::White);
}

void LevelEditor::addBlock(sf::Vector2u pos, const std::string &name) {
    game->getBoardObject().addBlock(pos, name);
    sf::Vector2f coordinates = {
        getTopLeftCorner(game).x +
            getBlockSize(game) * static_cast<float>(pos.x),
        getTopLeftCorner(game).y +
            getBlockSize(game) * static_cast<float>(pos.y)};
    boardSprites.at(pos.y).at(pos.x) =
        makeBlockSprite(blockTextures[name], getBlockSize(game), coordinates);
}

void LevelEditor::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    window.clear();
    loadLevel(window, game);
    blockSelectionBar.loadInWindow(window, event);
    levelNameTextbox.loadInWindow(window, event);
    saveButton.drawInWindow(window);
    saveButton.update(window, event);
    window.draw(levelBorder);
    startPosButton.drawInWindow(window);
    startPosButton.update(window, event);
    levelEndButton.drawInWindow(window);
    levelEndButton.update(window, event);
    backToMenuButton.drawInWindow(window);
    backToMenuButton.update(window, event);
    resetButton.drawInWindow(window);
    resetButton.update(window, event);
    plusButton.drawInWindow(window);
    plusButton.update(window, event);
    minusButton.drawInWindow(window);
    minusButton.update(window, event);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2f mousePos =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Vector2i inGamePos = static_cast<sf::Vector2i>(
            (mousePos - getTopLeftCorner(game)) / getBlockSize(game) *
            static_cast<float>(BLOCK_SIZE)
        );
        if (state == EditorState::BlockChosen) {
            sf::Vector2i blockPos = getInGameCoordinates(
                window.mapPixelToCoords(sf::Mouse::getPosition(window)), game
            );
            if (blockPos != sf::Vector2i(-1, -1)) {
                addBlock(static_cast<sf::Vector2u>(blockPos), blockChosen);
            }
        } else if (state == EditorState::StartPosChosen) {
            if (inGamePos.x >= 0 &&
                inGamePos.y < game->getBoardObject().getSize().y * BLOCK_SIZE) {
                setStartPos(inGamePos);
            }
        } else if (state == EditorState::EndPosChosen) {
            if (inGamePos.x >= 0 &&
                inGamePos.y < game->getBoardObject().getSize().y * BLOCK_SIZE) {
                setEndPos(inGamePos);
            }
        }
    }
    window.display();
}

void LevelEditor::setStartPos(sf::Vector2i pos) {
    sf::Vector2f offset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    levelBeginSprite.setPosition(getCoordinates(pos, game) - offset);
    game->setStartPos(pos);
}

void LevelEditor::setEndPos(sf::Vector2i pos) {
    sf::Vector2f offset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    levelEndSprite.setPosition(getCoordinates(pos, game) - offset);
    game->setEndPos(pos);
}

void LevelEditor::deactivateAll() {
    blockSelectionBar.deactivate();
    startPosButton.deactivate();
    levelEndButton.deactivate();
}
}  // namespace Platformer::gui
