#include "../include/menu.hpp"
#include <filesystem>
#include <utility>

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
    int width,
    int height
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
    int i = buttonLabelToNum[label_string];
    // TODO: exception handling
    rectangleButtons[i]->bind(std::move(func));
}

MainMenu::MainMenu(
    int windowWidth,
    int windowHeight,
    const sf::Font &font,
    int fontSize,
    int buttonDistance,
    const std::string &BackgroundTextureFilepath,
    sf::Vector2f startingButtonPosition
) {
    // TODO: window Width & Height dependency
    sf::Vector2f buttonIndent = {10, 5};
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 128, windowWidth, windowHeight
    );

    std::vector<std::string> buttonStringLabels = {
        "Start game", "Load game", "Settings", "Author info", "Feedback"};
    for (int i = 0; i < buttonStringLabels.size(); i++) {
        sf::Text buttonText = sf::Text(buttonStringLabels[i], font, fontSize);
        auto buttonWidth =
            buttonText.getGlobalBounds().width + 2 * buttonIndent.x;
        auto buttonHeight =
            buttonText.getGlobalBounds().height + 2 * buttonIndent.y;
        sf::RectangleShape buttonShape({buttonWidth, buttonHeight});
        // TODO: change to Overlord class methods
        std::function<void()> action = [buttonStringLabels, i]() {
            std::cout << "<" << buttonStringLabels[i]
                      << "> was pressed; no current bind\n";
        };
        interface::RectangleButton button(
            buttonShape, sf::Color(255, 0, 48, 192),
            sf::Color(118, 114, 111, 192), sf::Color(0, 209, 255, 192),
            sf::Color(255, 95, 0, 192), buttonText, buttonIndent,
            startingButtonPosition +
                sf::Vector2f(0, static_cast<float>(i * buttonDistance)),
            action
        );
        addRectangleButton(button);
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
    loadBackgroundSpriteFromTextureFile(
        BackgroundTextureFilepath, 255, 255, 255, 128, windowWidth, windowHeight
    );
    int count = 0;
    for (auto &p : std::filesystem::directory_iterator(std::filesystem::path{
             LevelFilePath})) {
        count++;
    }
    auto add_new_button = [&](const std::string &label_string, int number,
                              sf::Vector2f startingButtonPosition = {340, 250},
                              sf::Vector2f buttonIndent = {10, 5}) {
        sf::Text buttonText = sf::Text(label_string, font, fontSize);
        auto buttonWidth =
            buttonText.getGlobalBounds().width + 2 * buttonIndent.x;
        auto buttonHeight =
            buttonText.getGlobalBounds().height + 2 * buttonIndent.y;
        sf::RectangleShape buttonShape({buttonWidth, buttonHeight});
        // TODO: change to Overlord class methods
        std::function<void()> action = [&number]() {
            std::cout << "<"
                      << "Button" + std::to_string(number) << "> was chosen\n";
        };
        interface::RectangleButton button(
            buttonShape, sf::Color(255, 0, 48, 192),
            sf::Color(118, 114, 111, 192), sf::Color(178, 160, 53, 192),
            sf::Color(255, 95, 0, 192), buttonText, buttonIndent,
            startingButtonPosition +
                sf::Vector2f(0, static_cast<float>(number * buttonDistance)),
            action
        );
        addRectangleButton(button);
    };
    add_new_button("Return", 0);
    for (int i = 1; i <= count; i++) {
        add_new_button("Level" + std::to_string(i), i);
    }
}
}  // namespace interface