#ifndef PLATFORMERGAMECPP_MENU_HPP
#define PLATFORMERGAMECPP_MENU_HPP

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "button.hpp"
#include "level-render.hpp"
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
    void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void
    bindButton(const std::string &label_string, std::function<void()> func);
    void addNewButton(
        const std::string &label_string,
        int number,
        const sf::Font &font,
        int fontSize,
        std::vector<sf::Color> &buttonColorsList,
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
        Platformer::gui::LevelWindow &levelWindow
    );
};

struct LevelSelectionMenu : Menu {
public:
    LevelSelectionMenu(
        unsigned int windowWidth,
        unsigned int windowHeight,
        const sf::Font &font,
        int fontSize,
        int buttonDistance,
        const std::string &BackgroundTextureFilepath,
        const std::string &LevelFilePath,
        control::MenuPerformer &menuPerformer,
        control::LevelPerformer &levelPerformer,
        Platformer::gui::LevelWindow &levelWindow
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
