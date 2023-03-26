#include "../tools/json.hpp"
#include "include/menu.hpp"
#include "include/overlord.hpp"
#include "include/level-render.hpp"
#include "include/gui-constants.hpp"
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
    const int windowWidth = Platformer::gui::WINDOW_WIDTH;
    const int windowHeight = Platformer::gui::WINDOW_HEIGHT;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game");
    window.setPosition(sf::Vector2i(200, 50));
    window.setFramerateLimit(60);
    control::MainMenuOverlord overlord(window, "../model/levels/");

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/lofi.ttf");

    auto mainMenu = interface::MainMenu(windowWidth,
                                        windowHeight,
                                        fontMario,
                                        20,
                                        50,
                                        "../gui/assets/textures/interface/main-menu-background.png",
                                        overlord,
                                        {70, 290});

    auto loadMenu = interface::LevelSelectionMenu(windowWidth,
                                                       windowHeight,
                                                       fontMario,
                                                       20,
                                                       50,
                                                       "../gui/assets/textures/interface/level-selection-menu-background.png",
                                                       "../model/levels/");

    auto levelWindow = Platformer::gui::levelWindow(windowWidth,
                                                    windowHeight,
                                                    "assets/textures/interface/level-background.png",
                                                    "levels/t01-box-with-ladder.json");

    mainMenu.bindButton("Load game", [&](){overlord.setState(control::CurrentProcess::LoadMenu);});
    loadMenu.bindButton("Return", [&](){overlord.setState(control::CurrentProcess::MainMenu);});

    // Platformer::Game game("levels/t01-box-with-ladder.json");
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        switch(overlord.getState()) {
            case control::CurrentProcess::MainMenu :
                mainMenu.loadInWindow(window, event);
                break;
            case control::CurrentProcess::LoadMenu :
                loadMenu.loadInWindow(window, event);
                break;
            case control::CurrentProcess::LevelRunning:
                // TODO: load game object to window
                // TODO: game move assignment?
                levelWindow.loadInWindow(window);
                break;
            case control::CurrentProcess::LevelPaused:
                // TODO: load game object to window anyway, but different controls
                break;
        }

        window.display();
    }
    return 0;
}
