#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "../tools/json.hpp"
#include "include/gui-constants.hpp"
#include "include/level-render.hpp"
#include "include/menu.hpp"
#include "include/performer.hpp"

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
    control::MenuPerformer menuPerformer(window, "../model/levels/");
    control::LevelPerformer levelPerformer(window);

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/lofi.ttf");

    auto levelWindow = Platformer::gui::LevelWindow(
        windowHeight, "../gui/assets/textures/interface/level-background.png",
        "../gui/assets/textures/player", "../gui/assets/textures/misc",
        "../model/levels/t02-hard-jumps.json", &levelPerformer
    );

    auto mainMenu = interface::MainMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/main-menu-background.png",
        menuPerformer, levelPerformer, levelWindow
    );

    auto loadMenu = interface::LevelSelectionMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-selection-menu-background.png",
        "../model/levels/", menuPerformer, levelPerformer, levelWindow
    );

    auto pauseMenu = interface::PauseMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/transparent.jpg", menuPerformer,
        levelPerformer
    );

    auto wonMenu = interface::WonMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-background.png",
        menuPerformer, levelPerformer, levelWindow
    );


    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        // TODO: some kind of global state?
        switch (levelPerformer.getState()) {
            case control::LevelState::Empty:
                break;
            case control::LevelState::Running:
                levelWindow.loadInWindow(window);
                break;
            case control::LevelState::Paused:
                // TODO: resolve flickering when using line below
                // LevelWindow.loadInWindow(window);
                // TODO: line below is very ugly, but RN no way to get
                //  signal from inside level-renderer to mainMenu
                menuPerformer.openPauseMenu();
                break;
            case control::LevelState::Won:
                if (!menuPerformer.getIsGameIsEnded()) { // TODO
                    wonMenu = interface::WonMenu(
                        windowWidth, windowHeight, fontMario, 20, 50,
                        "../gui/assets/textures/interface/level-background.png",
                        menuPerformer, levelPerformer, levelWindow
                    );
                }
                menuPerformer.openWonMenu();
                levelPerformer.setState(control::LevelState::Won);
                levelPerformer.reset();
        }
        switch (menuPerformer.getState()) {
            case control::MenuState::MainMenu:
                mainMenu.loadInWindow(window, event);
                break;
            case control::MenuState::LoadMenu:
                loadMenu.loadInWindow(window, event);
                break;
            case control::MenuState::PauseMenu:
                pauseMenu.loadInWindow(window, event);
                break;
            case control::MenuState::WonMenu:
                wonMenu.loadInWindow(window, event);
            case control::MenuState::Empty:
                break;
        }
        window.display();
    }
    return 0;
}
