#ifndef PLATFORMERGAMECPP_LEVEL_HPP
#define PLATFORMERGAMECPP_LEVEL_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include "menu.hpp"
#include "../../model/include/game.hpp"

namespace control {
struct Overlord { // TODO: struct -> class if private fields exist
protected:
    sf::RenderWindow & window;
public:
    explicit Overlord(sf::RenderWindow & window_) : window(window_){};
};

struct MainMenuOverlord : Overlord {
private:
    std::vector<std::string> level_paths;
public:
    void startGame();
    void loadLevel(int level_num);
    void switchSubMenu(interface::Menu & menu);
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
