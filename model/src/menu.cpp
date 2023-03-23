#include <filesystem>
#include "../include/menu.hpp"
#include "../include/button.hpp"
namespace interface {
std::unique_ptr<RectangleButton> &interface::Menu::addRectangleButton(
    interface::RectangleButton buttonSample
) {
    rectangleButtons.emplace_back(
        std::make_unique<RectangleButton>(std::move(buttonSample))
    );
    return rectangleButtons[rectangleButtons.size() - 1];
}

void Menu::loadBackgroundSpriteFromTextureFile(
    const std::string &texturePath,
    int r,
    int g,
    int b,
    int a,
    int width,
    int height
) {
    if (!backgroundTexture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load background texture: " << texturePath << " invalid\n";
    }
    else {
        auto textureSize = backgroundTexture.getSize();

        float scaleWidth = static_cast<float>(width) / static_cast<float>(textureSize.x);
        float scaleHeight = static_cast<float>(height) / static_cast<float>(textureSize.y);

        backgroundSprite.setTexture(backgroundTexture); // TODO: different types, all OK?
        backgroundSprite.setColor(sf::Color(r, g, b, a));
        backgroundSprite.setScale(scaleWidth, scaleHeight);
    }
}

void Menu::loadBackgroundInWindow(sf::RenderWindow &window) {
    window.draw(backgroundSprite);
}

void Menu::loadRectangleButtonsInWindow(sf::RenderWindow &window) {
    for (auto & rectangleButton : rectangleButtons) {
        rectangleButton->drawInWindow(window);
    }
}

void Menu::updateButtons(sf::RenderWindow &window, sf::Event event) {
    for (auto & button : rectangleButtons) {
        button->update(window, event);
    }
}

void Menu::loadInWindow(sf::RenderWindow & window, sf::Event event) {
    loadBackgroundInWindow(window);
    updateButtons(window, event);
    loadRectangleButtonsInWindow(window);
}

MainMenu::MainMenu(
    int windowWidth,
    int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath
)
{
    sf::Vector2f startingButtonPosition = {340, 250}; // TODO: window Width & Height dependency
    sf::Vector2f buttonIndent = {10, 5};
    loadBackgroundSpriteFromTextureFile(BackgroundTextureFilepath,
                                        255, 255, 255, 128, windowWidth, windowHeight);

    std::vector<std::string> buttonStringLabels = {"Start game", "Load game", "Settings", "Author info", "Feedback"};
    for (int i = 0; i < buttonStringLabels.size(); i++) {
        sf::Text buttonText = sf::Text(buttonStringLabels[i], font, fontSize);
        auto buttonWidth = buttonText.getGlobalBounds().width + 2 * buttonIndent.x;
        auto buttonHeight = buttonText.getGlobalBounds().height + 2 * buttonIndent.y;
        sf::RectangleShape buttonShape({buttonWidth, buttonHeight});
        interface::RectangleButton button(buttonShape,
                                          sf::Color(255, 0, 48, 192),
                                          sf::Color(118, 114, 111, 192),
                                          sf::Color(0, 209, 255, 192),
                                          sf::Color(255, 95, 0, 192),
                                          buttonText,
                                          buttonIndent,
                                          startingButtonPosition + sf::Vector2f(0, static_cast<float>(i*buttonDistance)));
        addRectangleButton(button); // TODO: add button action when implemented
    }
}

LevelSelectionMenu::LevelSelectionMenu(
    int windowWidth,
    int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    const std::string &LevelFilePath
) {
    loadBackgroundSpriteFromTextureFile(BackgroundTextureFilepath,
                                        255, 255, 255, 128, windowWidth, windowHeight);
    // TODO: basically empty class, after MVP should add level preview, min time etc
    // right now uses almost the same code as MainMenu, but this will change
    int count = 0;
    for (auto& p : std::filesystem::directory_iterator(std::filesystem::path {LevelFilePath})) {
        count++;
    }
    sf::Vector2f startingButtonPosition = {340, 250}; // TODO: window Width & Height dependency
    sf::Vector2f buttonIndent = {10, 5};
    for (int i = 1; i <= count; i++) {
        sf::Text buttonText = sf::Text("Level " + std::to_string(i), font, fontSize);
        auto buttonWidth = buttonText.getGlobalBounds().width + 2 * buttonIndent.x;
        auto buttonHeight = buttonText.getGlobalBounds().height + 2 * buttonIndent.y;
        sf::RectangleShape buttonShape({buttonWidth, buttonHeight});
        interface::RectangleButton button(buttonShape,
                                          sf::Color(255, 0, 48, 192),
                                          sf::Color(118, 114, 111, 192),
                                          sf::Color(178, 160, 53, 192),
                                          sf::Color(255, 95, 0, 192),
                                          buttonText,
                                          buttonIndent,
                                          startingButtonPosition + sf::Vector2f(0, static_cast<float>(i*buttonDistance)));
        addRectangleButton(button); // TODO: add button action when implemented
    }
}
} // namespace interface