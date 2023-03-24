#ifndef PLATFORMERGAMECPP_MENU_HPP
#define PLATFORMERGAMECPP_MENU_HPP

#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include  "button.hpp"
namespace interface {
    struct Menu {
    protected:
        std::vector<std::unique_ptr<RectangleButton>> rectangleButtons;
        // TODO: button class should include link to GameEvent it triggers
        // TODO: do we even need unique_ptr here?
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;
    public:
        std::unique_ptr<RectangleButton> & addRectangleButton(
            RectangleButton buttonSample);
        void loadBackgroundSpriteFromTextureFile(const std::string& texturePath, int r, int g, int b, int a, int width, int height);
        void loadBackgroundInWindow(sf::RenderWindow & window);
        void loadRectangleButtonsInWindow(sf::RenderWindow & window);
        void updateButtons(sf::RenderWindow & window, sf::Event event);
        void loadInWindow(sf::RenderWindow & window, sf::Event event);
    };
    struct MainMenu : Menu {
    public:
        MainMenu(int windowWidth,
                 int windowHeight,
                 const sf::Font & font,
                 int fontSize,
                 int buttonDistance,
                 const std::string& BackgroundTextureFilepath,
                 sf::Vector2f startingButtonPosition = {340, 250});
    };
    struct LevelSelectionMenu : Menu {
    public:
        LevelSelectionMenu(int windowWidth,
                           int windowHeight,
                           const sf::Font & font,
                           int fontSize,
                           int buttonDistance,
                           const std::string& BackgroundTextureFilepath,
                           const std::string &LevelFilePath);
    };
}

#endif  // PLATFORMERGAMECPP_MENU_HPP
