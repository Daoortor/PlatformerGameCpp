#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include "game_menu.hpp"

void start_game() {
    std::cout << "Game started!\n";

}
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platformer game");
    window.setPosition(sf::Vector2i(200, 50));
    interface::Menu menu;

    sf::Vector2f buttonSize = {100, 10}; // currently does nothing
    sf::Vector2f buttonPosition = {320, 250};
    sf::Vector2f buttonIndent = {10, 20}; // TODO: download royalty free font
    sf::Text buttonText;
    sf::Font font;
    if (!font.loadFromFile(R"(../assets/interface/fonts/Rough Anthem.ttf)")) {
        std::cerr << "Failed to load font!\n";
    }
    buttonText.setFont(font);
    buttonText.setString("Start game");
    buttonText.setCharacterSize(40);
    buttonText.setFillColor(sf::Color::Red);

    auto buttonWidth = buttonText.getGlobalBounds().width + 2*buttonIndent.x;
    auto buttonHeight = buttonText.getGlobalBounds().height + 2*buttonIndent.y;
    sf::RectangleShape Texture1(sf::Vector2f(buttonWidth,buttonHeight));
    sf::RectangleShape Texture2(sf::Vector2f(buttonWidth,buttonHeight));
    sf::RectangleShape Texture3(sf::Vector2f(buttonWidth,buttonHeight));
    sf::RectangleShape Texture4(sf::Vector2f(buttonWidth,buttonHeight));
    Texture1.setFillColor(sf::Color::Red); // Unavailable
    Texture2.setFillColor(sf::Color::Blue); // Available
    Texture3.setFillColor(sf::Color::Yellow); // Chosen
    Texture4.setFillColor(sf::Color::Green); // Clicked


    interface::RectangleShapeButton button(Texture1, Texture2, Texture3, Texture4,
                                           buttonText, buttonSize, buttonIndent,
                                           buttonPosition);
    menu.addButton(button, start_game);
    while(window.isOpen()) {
        sf::Event event{};
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed){window.close();}
        }

        window.clear(sf::Color::Magenta);
        menu.drawMenu(window);
        // window.draw(Texture1);
        menu.updateButtons(window, event);
        menu.runButtons();
        window.display();
    }

    return 0;
}