#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "../tools/json.hpp"
#include "include/gui-constants.hpp"
#include "include/level-render.hpp"
#include "include/menu.hpp"
#include "include/overlord.hpp"

using json = nlohmann::json;

sf::Font safeLoadFont(const std::string &file) {
    sf::Font font;
    if (!font.loadFromFile(file)) {
        std::cerr << "Failed to load font:" << file << '\n';
    }
    return font;
}

int main() {
    const unsigned int windowWidth = Platformer::gui::WINDOW_WIDTH;
    const unsigned int windowHeight = Platformer::gui::WINDOW_HEIGHT;
    sf::RenderWindow window(
        sf::VideoMode(windowWidth, windowHeight), "Game",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setPosition(sf::Vector2i(200, 50));
    window.setFramerateLimit(60);
    control::MainMenuOverlord mainMenuOverlord(window, "../model/levels/");
    control::LevelOverlord levelOverlord(
        window, std::make_unique<Platformer::Game>(
                    "../model/levels/t01-box-with-ladder.json"
                )
    );

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/lofi.ttf");

    auto mainMenu = interface::MainMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/main-menu-background.png",
        mainMenuOverlord, {70, 290}
    );

    auto loadMenu = interface::LevelSelectionMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-selection-menu-background.png",
        "../model/levels/"
    );

    auto levelWindow = Platformer::gui::levelWindow(
        windowHeight, "../gui/assets/textures/interface/level-background.png",
        "../gui/assets/textures/player",
        "../model/levels/t01-box-with-ladder.json", levelOverlord
    );

    mainMenu.bindButton("Load game", [&]() {
        mainMenuOverlord.setState(control::CurrentProcess::LoadMenu);
    });
    loadMenu.bindButton("Return", [&]() {
        mainMenuOverlord.setState(control::CurrentProcess::MainMenu);
    });

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        switch (mainMenuOverlord.getState()) {
            case control::CurrentProcess::MainMenu:
                mainMenu.loadInWindow(window, event);
                break;
            case control::CurrentProcess::LoadMenu:
                loadMenu.loadInWindow(window, event);
                break;
            case control::CurrentProcess::LevelRunning:
                // TODO: load game object to window
                // TODO: game move assignment?
                levelWindow.loadInWindow(window);
                break;
            case control::CurrentProcess::LevelPaused:
                // TODO: load game object to window anyway, but different
                // controls
                break;
        }

        window.display();
    }
    return 0;
}
