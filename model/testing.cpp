#include "../tools/json.hpp"
#include "include/menu.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

using json = nlohmann::json;

sf::Font safeLoadFont(const std::string& file){
    sf::Font font;
    if (!font.loadFromFile(file)) {
        std::cerr << "Failed to load font:" << file << '\n';
    }
    return font;
}

int main() {
    // Platformer::Game game("model/levels/t01-box-with-ladder.json");
    const int windowWidth = 800;
    const int windowHeight = 600;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game");
    window.setPosition(sf::Vector2i(200, 50));

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/TypefaceMarioWorldPixelFilledRegular-Yz84q.otf");

    auto mainMenu = interface::MainMenu(windowWidth,
                                        windowHeight,
                                        fontMario,
                                        20,
                                        50,
                                        "../gui/assets/textures/interface/wallpaper.png");
    auto selectionMenu = interface::LevelSelectionMenu(windowWidth,
                                                       windowHeight,
                                                       fontMario,
                                                       20,
                                                       50,
                                                       "../gui/assets/textures/interface/wallpaper.png",
                                                       "../model/levels/");
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        mainMenu.loadInWindow(window, event);
        window.display();
    }
    return 0;
}
