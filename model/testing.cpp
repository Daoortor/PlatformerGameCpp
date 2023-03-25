#include "../tools/json.hpp"
#include "../gui/include/menu.hpp"
#include "../gui/include/overlord.hpp"
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
    const int windowWidth = 800;
    const int windowHeight = 600;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game");
    window.setPosition(sf::Vector2i(200, 50));
    control::MainMenuOverlord overlord(window, "../model/levels/");

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/TypefaceMarioWorldPixelFilledRegular-Yz84q.otf");

    auto mainMenu = interface::MainMenu(windowWidth,
                                        windowHeight,
                                        fontMario,
                                        20,
                                        50,
                                        "../gui/assets/textures/interface/JockerArt.png",
                                        {70, 290});

    auto loadMenu = interface::LevelSelectionMenu(windowWidth,
                                                       windowHeight,
                                                       fontMario,
                                                       20,
                                                       50,
                                                       "../gui/assets/textures/interface/wallpaper.png",
                                                       "../model/levels/");
    // TODO: remove next two lines after showcase

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
                break;
            case control::CurrentProcess::LevelPaused:
                // TODO: load game object to window anyway, but different controls
                break;
        }

        window.display();
    }
    return 0;
}
