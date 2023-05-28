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
}  // namespace interface

#endif  // PLATFORMERGAMECPP_MENU_HPP
