#ifndef PLATFORMERGAMECPP_LEVEL_HPP
#define PLATFORMERGAMECPP_LEVEL_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <filesystem>
#include "menu.hpp"
#include "../../model/include/game.hpp"

namespace control {
enum class CurrentProcess {MainMenu, LoadMenu};
struct Overlord { // TODO: struct -> class if private fields exist
protected:
    sf::RenderWindow & window;
    CurrentProcess state = CurrentProcess::MainMenu;
public:
    explicit Overlord(sf::RenderWindow & window_) : window(window_){};
    CurrentProcess getState();
    void setState(CurrentProcess newState);
};

struct MainMenuOverlord : Overlord {
private:
    std::vector<std::string> level_paths;
public:
    MainMenuOverlord(sf::RenderWindow & window_, const std::string& levels_directory) : Overlord(window_) {
        // TODO: same meaning as LoadMenu constructor; should trim one of two
        for (const auto & entry : std::filesystem::directory_iterator(levels_directory))
            level_paths.push_back(entry.path().string());
    }
    void openMainMenu();
    void loadLevel(int level_num);
    void openLoadLevelMenu();
};

struct LevelOverlord : Overlord {
private:
    Platformer::Game game;
public:
    void pauseOrResume();
    void moveLeft();
    void moveRight();
    void jump();
    void exit();
};
}

#endif  // PLATFORMERGAMECPP_LEVEL_HPP
