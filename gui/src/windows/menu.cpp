#include "windows/menu.hpp"
#include <algorithm>
#include <filesystem>
#include <utility>
#include "gui-constants.hpp"

namespace interface {
template <typename RectangleButtonDerivedClass>
std::unique_ptr<RectangleButton> &Menu::addCreatedRectangleDescendantButton(
    RectangleButtonDerivedClass &buttonSample
) {
    rectangleButtons.emplace_back(
        std::make_unique<RectangleButtonDerivedClass>(buttonSample)
    );
    buttonLabelToNum[rectangleButtons.back()->getLabel().getString()] =
        static_cast<int>(rectangleButtons.size() - 1);
    return rectangleButtons.back();
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

void Menu::createAndAddNewRectangleButton(
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
    };

    RectangleButton button(
        buttonShape, buttonColorsList[0], buttonColorsList[1],
        buttonColorsList[2], buttonColorsList[3], buttonText, buttonIndent,
        startingButtonPosition +
            sf::Vector2f(0, static_cast<float>(number * buttonDistance)),
        action
    );
    addCreatedRectangleDescendantButton(button);
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
        createAndAddNewRectangleButton(
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
    });
    bindButton("Level editor", [&]() {
        menuPerformer.setState(control::MenuState::Empty);
        levelPerformer.setState(control::LevelState::Editor);
    });
    bindButton("Select level", [&]() { menuPerformer.openLoadLevelMenu(); });
    bindButton("Connect to server", [&]() {
        menuPerformer.setState(control::MenuState::ServerMenu);
        // TODO: add openServerMenu method for consistency
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
      ) {
    auto colorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
        sf::Color(178, 160, 53, 192), sf::Color(255, 95, 0, 192)};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 128, windowWidth, windowHeight
    );
    createAndAddNewRectangleButton(
        "Return", 0, font, fontSize, colorsList, buttonDistance, {340, 150},
        {10, 10}
    );
    bindButton("Return", [&]() {
        buttonScrollbar.reset();
        menuPerformer.openMainMenu();
    });

    ButtonWithImage refreshButton(
        miscFilepath + "refresh.png", sf::RectangleShape({40, 40}), {10, 10},
        {470, 150}, Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &menuPerformer, &levelPerformer,
         &levelWindow, windowHeight] {
            update(
                levelFilePath, font, fontSize, menuPerformer, levelPerformer,
                levelWindow, windowHeight
            );
        },
        false
    );
    addCreatedRectangleDescendantButton(refreshButton);

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
        createAndAddNewRectangleButton(
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
)
    : localLevelsScrollbar(
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
      ) {
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 0, windowWidth, windowHeight
    );
    auto colorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
        sf::Color(178, 160, 53, 192), sf::Color(255, 95, 0, 192)};
    auto labelColorsList = std::vector{
        sf::Color(255, 0, 48, 192), sf::Color(255, 0, 48, 192),
        sf::Color(255, 0, 48, 192), sf::Color(255, 0, 48, 192)};
    createAndAddNewRectangleButton(
        "Return", 0, font, fontSize, colorsList, buttonDistance, {360, 150},
        {10, 10}
    );
    bindButton("Return", [&]() {
        localLevelsScrollbar.reset();
        serverPerformer.clear_local_levels_selection();
        serverPerformer.clear_global_levels_selection();
        menuPerformer.openMainMenu();
    });

    ButtonWithImage refreshServerButton(
        miscFilepath + "refresh.png", sf::RectangleShape({40, 40}), {10, 10},
        {545, static_cast<float>(10 + buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &serverPerformer] {
            refreshGlobalLevels(font, fontSize, serverPerformer);
        },
        false
    );
    addCreatedRectangleDescendantButton(refreshServerButton);

    ButtonWithImage refreshLocalButton(
        miscFilepath + "refresh.png", sf::RectangleShape({40, 40}), {10, 10},
        {215, static_cast<float>(10 + buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &serverPerformer] {
            refreshLocalLevels(levelFilePath, font, fontSize, serverPerformer);
        },
        false
    );
    addCreatedRectangleDescendantButton(refreshLocalButton);

    ButtonWithImage sendButton(
        miscFilepath + "right-arrow.png", sf::RectangleShape({40, 40}),
        {10, 10}, {375, static_cast<float>(150 + 1 * buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &serverPerformer] {
            serverPerformer.sendSelectedToServer();
            refreshUI(levelFilePath, font, fontSize, serverPerformer);
        },
        false
    );
    addCreatedRectangleDescendantButton(sendButton);

    ButtonWithImage receiveButton(
        miscFilepath + "left-arrow.png", sf::RectangleShape({40, 40}), {10, 10},
        {375, static_cast<float>(150 + 2 * buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &serverPerformer] {
            serverPerformer.getSelectedFromServer();
            refreshUI(levelFilePath, font, fontSize, serverPerformer);
        },
        false
    );
    addCreatedRectangleDescendantButton(receiveButton);

    ButtonWithImage binButton(
        miscFilepath + "bin.png", sf::RectangleShape({40, 40}), {10, 10},
        {375, static_cast<float>(150 + 3 * buttonDistance)},
        Platformer::gui::colors::BUTTON_COLORS_LIST,
        [this, levelFilePath, &font, fontSize, &serverPerformer] {
            serverPerformer.deleteSelectedFromServer();
            refreshGlobalLevels(font, fontSize, serverPerformer);
        },
        false
    );
    addCreatedRectangleDescendantButton(binButton);

    createAndAddNewRectangleButton(
        "Local Levels", 0, font, fontSize, labelColorsList, buttonDistance,
        {40, 10}, {10, 10}
    );
    createAndAddNewRectangleButton(
        "Available Levels", 0, font, fontSize, labelColorsList, buttonDistance,
        {590, 10}, {10, 10}
    );
}

void ServerMenu::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    Menu::loadInWindow(window, event);
    localLevelsScrollbar.loadInWindow(window, event);
    serverLevelsScrollbar.loadInWindow(window, event);
}

void ServerMenu::refreshScrollbarButtonUtility(
    Scrollbar<interface::SwitchRectangleButton> &levelsScrollbar,
    const std::string &scrollbarType,
    const std::vector<std::string> &filenames,
    const sf::Font &font,
    int fontSize,
    control::ServerPerformer &serverPerformer
) {
    levelsScrollbar.clear();
    auto &colorsList = Platformer::gui::colors::BUTTON_COLORS_LIST;
    for (auto &levelName : filenames) {
        interface::SwitchRectangleButton newButton(
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
            },
            18
        );
        levelsScrollbar.addItem(newButton);
    }
}

void ServerMenu::refreshLocalLevels(
    const std::string &levelFilePath,
    const sf::Font &font,
    int fontSize,
    control::ServerPerformer &serverPerformer
) {
    refreshScrollbarButtonUtility(
        localLevelsScrollbar, "local",
        Platformer::utilities::getLevelNames(levelFilePath), font, fontSize,
        serverPerformer
    );
    serverPerformer.clear_local_levels_selection();
}

void ServerMenu::refreshGlobalLevels(
    const sf::Font &font,
    int fontSize,
    control::ServerPerformer &serverPerformer
) {
    refreshScrollbarButtonUtility(
        serverLevelsScrollbar, "global",
        serverPerformer.loadAllAvailableLevelNames(), font, fontSize,
        serverPerformer
    );
    serverPerformer.clear_global_levels_selection();
}

void ServerMenu::refreshUI(
    const std::string &levelFilePath,
    const sf::Font &font,
    int fontSize,
    control::ServerPerformer &serverPerformer
) {
    refreshLocalLevels(levelFilePath, font, fontSize, serverPerformer);
    refreshGlobalLevels(font, fontSize, serverPerformer);
}

WonMenu::WonMenu(
    unsigned int windowWidth,
    unsigned int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    control::MenuPerformer &menuPerformer,
    control::LevelPerformer &levelPerformer,
    Platformer::gui::LevelGameplayWindow &levelGameplayWindow
) {
    sf::Vector2f startingButtonPosition = {270, 220};
    sf::Vector2f buttonIndent = {10, 5};
    auto colorsList =
        std::vector{sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192)};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 25, 150, 223, 120, windowWidth, windowHeight
    );
    std::vector<std::string> buttonStringLabels = {"Back to title screen"};
    for (int number = 0; number < buttonStringLabels.size(); number++) {
        createAndAddNewRectangleButton(
            buttonStringLabels[number], 3 + number, font, fontSize, colorsList,
            buttonDistance, startingButtonPosition, buttonIndent
        );
    }
    bindButton("Back to title screen", [&]() {
        levelPerformer.exit();
        menuPerformer.openMainMenu();
    });

    std::string textLastTimer = "not defined";
    if (!levelPerformer.getStatistics()->getIsFirstTime()) {
        textLastTimer = levelPerformer.getStatistics()->getTextLastTimer();
    }
    std::vector<std::string> statisticsStringText = {
        "Time: " + levelPerformer.getStatistics()->getTextTimer(),
        "Death count: " +
            std::to_string(levelPerformer.getStatistics()->getDeathCount()),
        "Last timer: " + textLastTimer};

    if (levelPerformer.getStatistics()->getIsNewBestDeathCount()) {
        statisticsStringText[1] += " - New record";
    }
    if (levelPerformer.getStatistics()->getIsNewBestTime()) {
        statisticsStringText[0] += " - New record";
    }

    auto statisticsColorsList = std::vector{
        sf::Color(118, 114, 111, 192), sf::Color(118, 114, 111, 192)};

    for (int number = 0; number < statisticsStringText.size(); number++) {
        createAndAddNewRectangleButton(
            statisticsStringText[number], number, font, fontSize,
            statisticsColorsList, buttonDistance, startingButtonPosition,
            buttonIndent
        );
    }
}
}  // namespace interface
