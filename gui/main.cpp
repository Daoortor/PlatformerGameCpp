#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../tools/json.hpp"
#include "include/gui-constants.hpp"
#include "windows/menu.hpp"

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
    control::ServerPerformer serverPerformer(
        window, "0.0.0.0:50051", "../server/client_test_directory/",
        "../model/levels/"
    );

    sf::Font fontMario = safeLoadFont("../gui/assets/interface/fonts/lofi.ttf");

    auto levelGameplayWindow = Platformer::gui::LevelGameplayWindow(
        windowHeight, "../gui/assets/textures/interface/level-background.png",
        "../gui/assets/textures/player", "../gui/assets/textures/misc/",
        "../model/levels/level 2.json", &levelPerformer
    );

    auto levelEditor = Platformer::gui::LevelEditor(
        windowHeight, &levelPerformer, &menuPerformer,
        "../gui/assets/textures/interface/level-background.png",
        "../gui/assets/textures/blocks/", "../gui/assets/textures/misc/",
        "../model/" + Platformer::gui::levels::EMPTY_LEVEL_NAME,
        "../model/levels/", fontMario, {45, 0}
    );

    auto mainMenu = interface::MainMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/main-menu-background.png",
        menuPerformer, levelPerformer, levelGameplayWindow
    );

    auto loadMenu = interface::LevelSelectionMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-selection-menu-background.png",
        "../model/levels/", "../gui/assets/textures/misc/", menuPerformer,
        levelPerformer, levelGameplayWindow
    );

    auto pauseMenu = interface::PauseMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/transparent.jpg", menuPerformer,
        levelPerformer
    );

    auto serverMenu = interface::ServerMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-selection-menu-background.png",
        "../model/levels/", "../gui/assets/textures/misc/", menuPerformer,
        levelPerformer, serverPerformer, levelGameplayWindow
    );

    auto wonMenu = interface::WonMenu(
        windowWidth, windowHeight, fontMario, 20, 50,
        "../gui/assets/textures/interface/level-background.png", menuPerformer,
        levelPerformer, levelGameplayWindow
    );

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        switch (levelPerformer.getState()) {
            case control::LevelState::Empty:
                break;
            case control::LevelState::Running:
                levelGameplayWindow.loadInWindow(window);
                break;
            case control::LevelState::Paused:
                menuPerformer.openPauseMenu();
                break;
            case control::LevelState::Won:
                if (!menuPerformer.getIsGameIsEnded()) {  // TODO
                    wonMenu = interface::WonMenu(
                        windowWidth, windowHeight, fontMario, 20, 50,
                        "../gui/assets/textures/interface/level-background.png",
                        menuPerformer, levelPerformer, levelGameplayWindow
                    );
                }
                menuPerformer.openWonMenu();
                levelPerformer.setState(control::LevelState::Won);
                levelPerformer.reset();
                break;
            case control::LevelState::Editor:
                levelEditor.loadInWindow(window, event);
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
            case control::MenuState::ServerMenu:
                serverMenu.loadInWindow(window, event);
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
