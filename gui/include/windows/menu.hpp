#ifndef PLATFORMERGAMECPP_MENU_HPP
#define PLATFORMERGAMECPP_MENU_HPP

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "elements/button.hpp"
#include "level-editor.hpp"
#include "level-play.hpp"
#include "performer.hpp"

namespace interface {
struct Menu {
protected:
    std::vector<std::unique_ptr<RectangleButton>> rectangleButtons;
    // TODO: do we even need unique_ptr here?
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::map<std::string, int> buttonLabelToNum;

public:
    std::unique_ptr<RectangleButton> &addRectangleButton(
        RectangleButton buttonSample
    );
    void loadBackgroundSpriteFromTextureFile(
        const std::string &texturePath,
        int r,
        int g,
        int b,
        int a,
        unsigned int width,
        unsigned int height
    );
    void loadBackgroundInWindow(sf::RenderWindow &window);
    void loadRectangleButtonsInWindow(sf::RenderWindow &window);
    void updateButtons(sf::RenderWindow &window, sf::Event event);
    virtual void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void
    bindButton(const std::string &label_string, std::function<void()> func);
    void addNewButton(
        const std::string &label_string,
        int number,
        const sf::Font &font,
        int fontSize,
        const std::vector<sf::Color> &buttonColorsList,
        int buttonDistance,
        sf::Vector2f startingButtonPosition,
        sf::Vector2f buttonIndent
    );
};

struct MainMenu : Menu {
public:
    MainMenu(
        unsigned int windowWidth,
        unsigned int windowHeight,
        const sf::Font &font,
        int fontSize,
        int buttonDistance,
        const std::string &BackgroundTextureFilepath,
        control::MenuPerformer &menuPerformer,
        control::LevelPerformer &levelPerformer,
        Platformer::gui::LevelGameplayWindow &levelWindow
    );
};

struct LevelSelectionMenu : Menu {
    Scrollbar<interface::RectangleButton> buttonScrollbar;
    ButtonWithImage refreshButton;

public:
    LevelSelectionMenu(
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
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event) override;
    void update(
        const std::string &levelFilePath,
        const sf::Font &font,
        int fontSize,
        control::MenuPerformer &menuPerformer,
        control::LevelPerformer &levelPerformer,
        Platformer::gui::LevelGameplayWindow &levelWindow,
        unsigned int windowHeight
    );
};

struct PauseMenu : Menu {
public:
    PauseMenu(
        unsigned int windowWidth,
        unsigned int windowHeight,
        const sf::Font &font,
        int fontSize,
        int buttonDistance,
        const std::string &BackgroundTextureFilepath,
        control::MenuPerformer &menuPerformer,
        control::LevelPerformer &levelPerformer
    );
};

struct ServerMenu : Menu {
private:
    Scrollbar<interface::RectangleButton> localLevelsScrollbar;
    Scrollbar<interface::RectangleButton> serverLevelsScrollbar;
    ButtonWithImage refreshLocalButton;
    ButtonWithImage refreshServerButton;

    void refreshScrollbarButtonUtility(
        Scrollbar<interface::RectangleButton> & levelsScrollbar,
        const std::string & scrollbarType,
        const std::vector<std::string> & filenames,
        const sf::Font &font,
        int fontSize,
        control::ServerPerformer &serverPerformer
        ) {
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
                },
                18
            );
            levelsScrollbar.addItem(newButton);
        }
    }
public:
    ServerMenu(
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
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event) override;
};
}  // namespace interface

#endif  // PLATFORMERGAMECPP_MENU_HPP
