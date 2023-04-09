#ifndef PLATFORMERGAMECPP_LEVEL_HPP
#define PLATFORMERGAMECPP_LEVEL_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <string>
#include "../../model/include/game.hpp"
#include "../../model/include/player.hpp"

namespace control {
enum class MenuState { MainMenu, LoadMenu, PauseMenu, Empty };
enum class LevelState { Empty, Running, Paused };

struct Performer {
protected:
    sf::RenderWindow &window;

public:
    explicit Performer(sf::RenderWindow &window_) : window(window_){};
};

class LevelPerformer;

class MenuPerformer : Performer {
private:
    std::vector<std::string> level_paths;
    MenuState state = MenuState::MainMenu;

public:
    MenuPerformer(
        sf::RenderWindow &window_,
        const std::string &levels_directory
    )
        : Performer(window_) {
        // TODO: same meaning as LoadMenu constructor; should trim one of two
        for (const auto &entry :
             std::filesystem::directory_iterator(levels_directory))
            level_paths.push_back(entry.path().string());
    }

    MenuState getState();

    [[maybe_unused]] void openMainMenu();
    [[maybe_unused]] Platformer::Game
    loadLevel(control::LevelPerformer &levelPerformer, int level_num);
    [[maybe_unused]] void openLoadLevelMenu();
    [[maybe_unused]] void openPauseMenu();
    void closeCurrentMenu();
    void closeWindow();
};

class LevelPerformer : Performer {
private:
    std::unique_ptr<Platformer::Game> game = nullptr;
    LevelState state = LevelState::Empty;

public:
    explicit LevelPerformer(sf::RenderWindow &window_);
    [[maybe_unused]] void pause();
    [[maybe_unused]] void resume();
    [[maybe_unused]] void moveLeft();
    [[maybe_unused]] void moveRight();
    [[maybe_unused]] void moveDown();
    void jump();
    [[maybe_unused]] void exit();
    void setLevel(std::unique_ptr<Platformer::Game> game_);

    std::unique_ptr<Platformer::Game> &getLevel();

    LevelState getState();
    void setState(LevelState newState);
};
}  // namespace control

#endif  // PLATFORMERGAMECPP_LEVEL_HPP
