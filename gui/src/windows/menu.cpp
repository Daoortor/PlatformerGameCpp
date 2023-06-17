#include "windows/menu.hpp"
#include <algorithm>
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
} // TODO: rewrite method to take different child classes of RectangleButton!!!

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
        "Start game",  "Select level", "Level editor", "Connect to server",
        "Author info", "Feedback",     "Quit"};
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
    bindButton("Select level", [&]() { menuPerformer.openLoadLevelMenu(); });
    bindButton("Connect to server", [&]() {
        menuPerformer.setState(control::MenuState::ServerMenu);
        // TODO: add openServerMenu method, same as similar others
    });
    bindButton("Quit", [&]() { menuPerformer.closeWindow(); });
}

LevelSelectionMenu::LevelSelectionMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    const std::string &levelFilePath,
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
           levelFilePath,
           &font,
           fontSize,
           &menuPerformer,
           &levelPerformer,
           &levelWindow,
           windowHeight] {
              update(
                  levelFilePath, font, fontSize, menuPerformer, levelPerformer,
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
    bindButton("Return", [&]() {
        buttonScrollbar.reset();
        menuPerformer.openMainMenu();
    });
    update(
        levelFilePath, font, fontSize, menuPerformer, levelPerformer,
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
    const std::string &levelFilePath,
    const sf::Font &font,
    int fontSize,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    Platformer::gui::LevelGameplayWindow &levelWindow,
    unsigned int windowHeight
) {
    buttonScrollbar.clear();
    auto &colorsList = Platformer::gui::colors::BUTTON_COLORS_LIST;
    auto filenames = Platformer::utilities::getLevelNames(levelFilePath);
    for (auto &levelName : filenames) {
        const std::string levelPath = levelFilePath + levelName + ".json";
        interface::RectangleButton newButton(
            sf::RectangleShape({150, 40}), colorsList[0], colorsList[1],
            colorsList[2], colorsList[3], sf::Text(levelName, font, fontSize),
            {10, 10}, {340, 250},
            [&, levelPath]() {
                menuPerformer.loadLevel(levelPerformer);
                levelWindow = Platformer::gui::LevelGameplayWindow(
                    windowHeight,
                    "../gui/assets/textures/interface/level-background.png",
                    "../gui/assets/textures/player",
                    "../gui/assets/textures/misc/", levelPath, &levelPerformer
                );
                // TODO: implementation above is due to rewrite
            },
            18
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

ServerMenu::ServerMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    const std::string &levelFilePath,
    const std::string &miscFilepath,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    control::ServerPerformer &serverPerformer,
    Platformer::gui::LevelGameplayWindow &levelWindow
) : localLevelsScrollbar(
    {40, static_cast<float>(10 + buttonDistance)},
    {150, 40},
    {10, 20},
    20,
    40,
    6,
    Platformer::gui::colors::BUTTON_COLORS_LIST,
    Platformer::gui::colors::ITEM_CHOSEN_COLOR,
    miscFilepath
    ),
      serverLevelsScrollbar(
          {590, static_cast<float>(10 + buttonDistance)},
          {150, 40},
          {10, 20},
          20,
          40,
          6,
          Platformer::gui::colors::BUTTON_COLORS_LIST,
          Platformer::gui::colors::ITEM_CHOSEN_COLOR,
          miscFilepath
      ),
    refreshLocalButton(
        miscFilepath + "refresh.png",
        sf::RectangleShape({40, 40}),
        {10, 10},
        {215, static_cast<float>(10 + buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this,
         levelFilePath,
         &font,
         fontSize, &serverPerformer] {
              refreshScrollbarButtonUtility(localLevelsScrollbar,
                                            "local",
                                            Platformer::utilities::getLevelNames(levelFilePath),
                                            font,
                                            fontSize,
                                            serverPerformer);
        },
        false
    ),
      refreshServerButton(
          miscFilepath + "refresh.png",
          sf::RectangleShape({40, 40}),
          {10, 10},
          {545, static_cast<float>(10 + buttonDistance)},
          Platformer::gui::colors::BUTTON_COLORS_LIST,
          [this,
           levelFilePath,
           &font,
           fontSize, &serverPerformer] {
              refreshScrollbarButtonUtility(serverLevelsScrollbar,
                                            "global",
                                            serverPerformer.loadAllAvailableLevelNames(),
                                            font,
                                            fontSize,
                                            serverPerformer);
          },
          false
      )
{
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 0, windowWidth, windowHeight
    );
    // TODO: too much code repetition & intersection with LevelSelectionMenu; REDO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    auto colorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
        sf::Color(178, 160, 53, 192), sf::Color(255, 95, 0, 192)
    };
    auto labelColorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(255, 0, 48, 192),
        sf::Color(255, 0, 48, 192), sf::Color(255, 0, 48, 192)
    };
    addNewButton(
        "Return", 0, font, fontSize, colorsList, buttonDistance, {360, 150},
        {10, 10}
    );
    bindButton("Return", [&]() {
        localLevelsScrollbar.reset();
        menuPerformer.openMainMenu();
    });

    addNewButton(
        "--->", 1, font, fontSize, colorsList, buttonDistance, {360, 150},
        {10, 10}
    );
    bindButton("--->", [&]() {
        serverPerformer.sendSelectedToServer();
    });

    addNewButton(
        "<---", 2, font, fontSize, colorsList, buttonDistance, {360, 150},
        {10, 10}
    );
    bindButton("<---", [&]() {
        serverPerformer.getSelectedFromServer();
    });

    addNewButton(
        "Local Levels", 0, font, fontSize, labelColorsList, buttonDistance, {40, 10},
        {10, 10}
    );
    addNewButton(
        "Available Levels", 0, font, fontSize, labelColorsList, buttonDistance, {590, 10},
        {10, 10}
    );
}

void ServerMenu::loadInWindow(
    sf::RenderWindow &window,
    sf::Event event
) {
    Menu::loadInWindow(window, event);
    localLevelsScrollbar.loadInWindow(window, event);
    serverLevelsScrollbar.loadInWindow(window, event);
    refreshLocalButton.drawInWindow(window);
    refreshLocalButton.update(window, event);
    refreshServerButton.drawInWindow(window);
    refreshServerButton.update(window, event);
}

void ServerMenu::refreshScrollbarButtonUtility(
    Scrollbar<interface::RectangleButton> &levelsScrollbar,
    const std::string &scrollbarType,
    const std::vector<std::string> &filenames,
    const sf::Font &font,
    int fontSize,
    control::ServerPerformer &serverPerformer
)  {
    levelsScrollbar.clear();
    auto &colorsList = Platformer::gui::colors::BUTTON_COLORS_LIST;
    // auto filenames = Platformer::utilities::getLevelNames(levelFilePath);
    for (auto &levelName : filenames) {
        interface::RectangleButton newButton(
            sf::RectangleShape({150, 40}), colorsList[0], colorsList[1],
            colorsList[2], colorsList[3], sf::Text(levelName, font, fontSize),
            {10, 10}, {340, 250},
            [&serverPerformer, levelName, scrollbarType]() {
                if (scrollbarType == "local") {
                    serverPerformer.switch_in_local_set(levelName);
                    serverPerformer.debug_local_set();
                } else if (scrollbarType == "global") {
                    serverPerformer.switch_in_global_set(levelName);
                    serverPerformer.debug_global_set();
                }
                // TODO: refactor if-else above
                // TODO: button colors switch
            },
            18
        );
        levelsScrollbar.addItem(newButton);
    }
}
}  // namespace interface
