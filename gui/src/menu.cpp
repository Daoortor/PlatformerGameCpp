#include "../include/menu.hpp"
#include <filesystem>
#include <utility>
#include "gui-constants.hpp"

namespace interface {
std::unique_ptr<RectangleButton> &interface::Menu::addRectangleButton(
    interface::RectangleButton buttonSample
) {
    rectangleButtons.emplace_back(
        std::make_unique<RectangleButton>(std::move(buttonSample))
    );
    buttonLabelToNum[rectangleButtons[rectangleButtons.size() - 1]
                         ->getLabel()
                         .getString()] =
        static_cast<int>(rectangleButtons.size() - 1);
    return rectangleButtons[rectangleButtons.size() - 1];
}

void Menu::loadBackgroundSpriteFromTextureFile(
    const std::string &texturePath,
    int r,
    int g,
    int b,
    int a,
    unsigned int width,
    unsigned int height
) {
    if (!backgroundTexture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load background texture: " << texturePath
                  << " invalid\n";
    } else {
        auto textureSize = backgroundTexture.getSize();

        float scaleWidth =
            static_cast<float>(width) / static_cast<float>(textureSize.x);
        float scaleHeight =
            static_cast<float>(height) / static_cast<float>(textureSize.y);

        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setColor(sf::Color(r, g, b, a));
        backgroundSprite.setScale(scaleWidth, scaleHeight);
    }
}

void Menu::loadBackgroundInWindow(sf::RenderWindow &window) {
    window.draw(backgroundSprite);
}

void Menu::loadRectangleButtonsInWindow(sf::RenderWindow &window) {
    for (auto &rectangleButton : rectangleButtons) {
        rectangleButton->drawInWindow(window);
    }
}

void Menu::updateButtons(sf::RenderWindow &window, sf::Event event) {
    for (auto &button : rectangleButtons) {
        button->update(window, event);
    }
}

void Menu::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    loadBackgroundInWindow(window);
    updateButtons(window, event);
    loadRectangleButtonsInWindow(window);
}

void Menu::bindButton(
    const std::string &label_string,
    std::function<void()> func
) {
    // TODO: better exception handling
    if (buttonLabelToNum.count(label_string) == 0) {
        std::cerr << "No available button named " << label_string
                  << "; binding halted\n";
    } else {
        int i = buttonLabelToNum[label_string];
        rectangleButtons[i]->bind(std::move(func));
    }
}

void Menu::addNewButton(
    const std::string &label_string,
    int number,
    const sf::Font &font,
    int fontSize,
    const std::vector<sf::Color> &buttonColorsList,
    int buttonDistance,
    sf::Vector2f startingButtonPosition,
    sf::Vector2f buttonIndent
) {
    sf::Text buttonText = sf::Text(label_string, font, fontSize);
    auto buttonWidth = buttonText.getGlobalBounds().width + 2 * buttonIndent.x;
    auto buttonHeight =
        buttonText.getGlobalBounds().height + 2 * buttonIndent.y;
    sf::RectangleShape buttonShape({buttonWidth, buttonHeight});
    std::function<void()> action = [label_string]() {
        std::cout << "<" << label_string << "> was pressed\n";
    };  // Placeholders for still not implemented buttonScrollbar
    interface::RectangleButton button(
        buttonShape, buttonColorsList[0], buttonColorsList[1],
        buttonColorsList[2], buttonColorsList[3], buttonText, buttonIndent,
        startingButtonPosition +
            sf::Vector2f(0, static_cast<float>(number * buttonDistance)),
        action
    );
    addRectangleButton(button);
}

MainMenu::MainMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    Platformer::gui::LevelGameplayWindow &levelWindow
) {
    // TODO: window Width & Height dependency
    sf::Vector2f startingButtonPosition = {340, 150};
    sf::Vector2f buttonIndent = {10, 5};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 255, windowWidth, windowHeight
    );

    std::vector<std::string> buttonStringLabels = {
        "Start game",  "Load game", "Level editor", "Settings",
        "Author info", "Feedback",  "Quit"};
    auto colorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
        sf::Color(0, 209, 255, 192), sf::Color(255, 95, 0, 192)};
    for (int i = 0; i < buttonStringLabels.size(); i++) {
        addNewButton(
            buttonStringLabels[i], i, font, fontSize, colorsList,
            buttonDistance, startingButtonPosition, buttonIndent
        );
    }
    bindButton("Start game", [&]() {
        menuPerformer.loadLevel(levelPerformer);
        levelWindow = Platformer::gui::LevelGameplayWindow(
            windowHeight,
            "../gui/assets/textures/interface/level-background.png",
            "../gui/assets/textures/player", "../gui/assets/textures/misc/",
            menuPerformer.getLevelFilePath(0), &levelPerformer
        );
        // TODO: big rewrite of levelPerformer-levelWindow relationship
    });
    bindButton("Level editor", [&]() {
        menuPerformer.setState(control::MenuState::Empty);
        levelPerformer.setState(control::LevelState::Editor);
    });
    bindButton("Load game", [&]() { menuPerformer.openLoadLevelMenu(); });
    bindButton("Quit", [&]() { menuPerformer.closeWindow(); });
}

LevelSelectionMenu::LevelSelectionMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    const std::string &LevelFilePath,
    const std::string &miscFilepath,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    Platformer::gui::LevelGameplayWindow &levelWindow
)
    : buttonScrollbar(
          {340, static_cast<float>(150 + buttonDistance)},
          {150, 40},
          {10, 20},
          20,
          40,
          3,
          Platformer::gui::colors::BUTTON_COLORS_LIST,
          Platformer::gui::colors::ITEM_CHOSEN_COLOR,
          miscFilepath
      ),
      refreshButton(
          miscFilepath + "refresh.png",
          sf::RectangleShape({40, 40}),
          {10, 10},
          {470, 150},
          Platformer::gui::colors::BUTTON_COLORS_LIST,
          [this,
           LevelFilePath,
           &font,
           fontSize,
           &menuPerformer,
           &levelPerformer,
           &levelWindow,
           windowHeight] {
              update(
                  LevelFilePath, font, fontSize, menuPerformer, levelPerformer,
                  levelWindow, windowHeight
              );
          },
          false
      ) {
    auto colorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
        sf::Color(178, 160, 53, 192), sf::Color(255, 95, 0, 192)};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 128, windowWidth, windowHeight
    );
    addNewButton(
        "Return", 0, font, fontSize, colorsList, buttonDistance, {340, 150},
        {10, 10}
    );
    bindButton("Return", [&]() { menuPerformer.openMainMenu(); });
    update(
        LevelFilePath, font, fontSize, menuPerformer, levelPerformer,
        levelWindow, windowHeight
    );
}

void LevelSelectionMenu::loadInWindow(
    sf::RenderWindow &window,
    sf::Event event
) {
    Menu::loadInWindow(window, event);
    buttonScrollbar.loadInWindow(window, event);
    refreshButton.drawInWindow(window);
    refreshButton.update(window, event);
}

void LevelSelectionMenu::update(
    const std::string &LevelFilePath,
    const sf::Font &font,
    int fontSize,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    Platformer::gui::LevelGameplayWindow &levelWindow,
    unsigned int windowHeight
) {
    buttonScrollbar.clear();
    auto &colorsList = Platformer::gui::colors::BUTTON_COLORS_LIST;
    for (auto &p : std::filesystem::directory_iterator(std::filesystem::path{
             LevelFilePath})) {
        const std::string levelPath = p.path();
        std::string levelName = levelPath.substr(LevelFilePath.size());
        const int capacity = 16;
        if (levelName.size() > capacity) {
            levelName = levelName.substr(0, capacity - 3) + "...";
        }
        interface::RectangleButton newButton(
            sf::RectangleShape({150, 40}), colorsList[0], colorsList[1],
            colorsList[2], colorsList[3], sf::Text(levelName, font, fontSize),
            {10, 5}, {340, 250},
            [&, levelPath]() {
                menuPerformer.loadLevel(levelPerformer);
                levelWindow = Platformer::gui::LevelGameplayWindow(
                    windowHeight,
                    "../gui/assets/textures/interface/level-background.png",
                    "../gui/assets/textures/player",
                    "../gui/assets/textures/misc/", levelPath, &levelPerformer
                );
                // TODO: implementation above is due to rewrite
            }
        );
        buttonScrollbar.addItem(newButton);
    }
}

PauseMenu::PauseMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer
) {
    sf::Vector2f startingButtonPosition = {340, 250};
    sf::Vector2f buttonIndent = {10, 5};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 0, windowWidth, windowHeight
    );
    std::vector<std::string> buttonStringLabels = {
        "Resume", "Back to title screen"};
    for (int number = 0; number < buttonStringLabels.size(); number++) {
        addNewButton(
            buttonStringLabels[number], number, font, fontSize,
            Platformer::gui::colors::BUTTON_COLORS_LIST, buttonDistance,
            startingButtonPosition, buttonIndent
        );
    }
    bindButton("Resume", [&]() {
        menuPerformer.closeCurrentMenu();
        levelPerformer.resume();
    });
    bindButton("Back to title screen", [&]() {
        levelPerformer.exit();
        menuPerformer.openMainMenu();
    });
}
}  // namespace interface
