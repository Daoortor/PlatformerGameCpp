#include "windows/level-editor.hpp"

namespace Platformer::gui {
LevelEditor::LevelEditor(
    unsigned int windowHeight,
    control::LevelPerformer *levelPerformerPtr_,
    control::MenuPerformer *menuPerformerPtr_,
    const std::string &backgroundTextureFilepath,
    const std::string &blockFilepath,
    const std::string &miscFilepath,
    const std::string &emptyLevelFilepath,
    const std::string &levelsFilepath,
    const sf::Font &font,
    sf::Vector2f offset
)
    : LevelWindow(
          windowHeight,
          backgroundTextureFilepath,
          miscFilepath,
          emptyLevelFilepath,
          offset
      ),
      levelPerformerPtr(levelPerformerPtr_),
      menuPerformerPtr(menuPerformerPtr_),
      game(std::make_unique<Game>(emptyLevelFilepath)),
      gameFullCopy(std::make_unique<Game>(*game)),
      blockSelectionBar(
          {75, getTopLeftCorner(game).y},
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
          {415, 520},
          {10, 10},
          13
      ),
      loadButton(
          sf::RectangleShape({60, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("Load", font, 20),
          {10, 10},
          {585, 520},
          [this, levelsFilepath, miscFilepath] {
              try {
                  game = std::make_unique<Game>(
                      levelsFilepath + levelNameTextbox.getText() + ".json"
                  );
                  updateAll(game, miscFilepath);
              } catch (const Platformer::FileNotFoundError &) {
                  levelNameTextbox.setText("not found");
              }
          }
      ),
      saveButton(
          sf::RectangleShape({60, 40}),
          colors::BUTTON_COLORS_LIST[0],
          colors::BUTTON_COLORS_LIST[1],
          colors::BUTTON_COLORS_LIST[2],
          colors::BUTTON_COLORS_LIST[3],
          sf::Text("Save", font, 20),
          {10, 10},
          {665, 520},
          [&, levelsFilepath]() {
              game->writeToFile(levelNameTextbox.getText(), levelsFilepath);
              levelNameTextbox.setText("");
          }
      ),
      startPosButton(
          miscFilepath + "level-begin.png",
          sf::RectangleShape({40, 40}),
          {0, 0},
          {85, 348},
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
          {85, 430},
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
          {75, 520},
          [this]() {
              deactivateAll();
              state = EditorState::Idle;
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
          {230, 520},
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
          {315, 520},
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
              auto copySize = gameFullCopy->getBoardObject().getSize();
              if (copySize.y < levelSize.y) {
                  this->gameFullCopy = std::make_unique<Game>(*game);
              } else {
                  this->gameFullCopy->partialCopy(
                      game, (copySize.y - levelSize.y) / 2,
                      (copySize.x - levelSize.x) / 2
                  );
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
          {365, 520},
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
    for (int blockNum = 0; blockNum < levels::BLOCK_NAMES.size(); blockNum++) {
        std::string blockFullPath =
            blockFilepath + levels::BLOCK_NAMES[blockNum] + ".png";
        if (levels::BLOCK_NAMES[blockNum] == "air") {
            blockFullPath = miscFilepath + "eraser.png";
        }
        interface::ButtonWithImage newButton(
            blockFullPath, sf::RectangleShape({40, 40}), {0, 0}, {0, 0},
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
        sf::Vector2f(
            (static_cast<float>(WINDOW_WIDTH) - levelBorder.getSize().x) / 2,
            (static_cast<float>(WINDOW_HEIGHT) - levelBorder.getSize().y) / 2
        ) +
        offset
    );
    levelBorder.setFillColor(sf::Color::Transparent);
    levelBorder.setOutlineThickness(1.f);
    levelBorder.setOutlineColor(sf::Color::White);
}

void LevelEditor::setBlock(
    sf::Vector2<std::size_t> pos,
    const std::string &name
) {
    game->getBoardObject().setBlock(pos, name);
    auto coordinates =
        offset + sf::Vector2f(
                     getTopLeftCorner(game).x +
                         getBlockSize(game) * static_cast<float>(pos.x),
                     getTopLeftCorner(game).y +
                         getBlockSize(game) * static_cast<float>(pos.y)
                 );
    boardSprites.at(pos.y).at(pos.x) =
        makeBlockSprite(blockTextures[name], getBlockSize(game), coordinates);
}

void LevelEditor::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    window.clear();
    loadLevel(window, game);
    blockSelectionBar.loadInWindow(window, event);
    levelNameTextbox.loadInWindow(window, event);
    loadButton.drawInWindow(window);
    loadButton.update(window, event);
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
            window.mapPixelToCoords(sf::Mouse::getPosition(window)) - offset;
        sf::Vector2i inGamePos = static_cast<sf::Vector2i>(
            (mousePos - getTopLeftCorner(game)) / getBlockSize(game) *
            static_cast<float>(BLOCK_SIZE)
        );
        if (state == EditorState::BlockChosen) {
            sf::Vector2i blockPos = getInGameCoordinates(
                window.mapPixelToCoords(
                    sf::Mouse::getPosition(window) -
                    static_cast<sf::Vector2i>(offset)
                ),
                game
            );
            if (blockPos != sf::Vector2i(-1, -1)) {
                setBlock(
                    static_cast<sf::Vector2<std::size_t>>(blockPos), blockChosen
                );
            }
        } else if (state == EditorState::StartPosChosen) {
            if (0 <= inGamePos.x &&
                inGamePos.x <=
                    game->getBoardObject().getSize().x * BLOCK_SIZE &&
                inGamePos.y <=
                    static_cast<int>(
                        game->getBoardObject().getSize().y * BLOCK_SIZE
                    )) {
                setStartPos(inGamePos);
            }
        } else if (state == EditorState::EndPosChosen) {
            if (0 <= inGamePos.x &&
                inGamePos.x <=
                    game->getBoardObject().getSize().x * BLOCK_SIZE &&
                inGamePos.y <=
                    static_cast<int>(
                        game->getBoardObject().getSize().y * BLOCK_SIZE
                    )) {
                setEndPos(inGamePos);
            }
        }
    }
    window.display();
}

void LevelEditor::setStartPos(sf::Vector2i pos) {
    sf::Vector2f circleOffset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    levelBeginSprite.setPosition(
        getCoordinates(pos, game) - circleOffset + offset
    );
    game->setStartPos(pos);
}

void LevelEditor::setEndPos(sf::Vector2i pos) {
    sf::Vector2f circleOffset =
        sf::Vector2f(getBlockSize(game) / 4, getBlockSize(game) / 4);
    levelEndSprite.setPosition(
        getCoordinates(pos, game) - circleOffset + offset
    );
    game->setEndPos(pos);
}

void LevelEditor::deactivateAll() {
    blockSelectionBar.deactivate();
    startPosButton.deactivate();
    levelEndButton.deactivate();
}
}  // namespace Platformer::gui
