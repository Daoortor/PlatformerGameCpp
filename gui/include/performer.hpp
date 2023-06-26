#ifndef PLATFORMERGAMECPP_LEVEL_HPP
#define PLATFORMERGAMECPP_LEVEL_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
#include <set>
#include <string>
#include "../../tools/utilities.hpp"
#include "client.hpp"
#include "game.hpp"
#include "player.hpp"
#include "statistics.hpp"

namespace control {
enum class MenuState {
    MainMenu,
    LoadMenu,
    PauseMenu,
    ServerMenu,
    WonMenu,
    Empty
};
enum class LevelState { Empty, Running, Paused, Won, Editor };

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
    bool isGameIsEnded = false;

public:
    MenuPerformer(
        sf::RenderWindow &window_,
        const std::string &levels_directory
    )
        : Performer(window_),
          level_paths(Platformer::utilities::getLevelPaths(levels_directory)) {
    }

    MenuState getState();

    [[maybe_unused]] void openMainMenu();
    [[maybe_unused]] void loadLevel(LevelPerformer &levelPerformer);
    [[maybe_unused]] void openLoadLevelMenu();
    [[maybe_unused]] void openPauseMenu();
    [[maybe_unused]] void openWonMenu();
    void closeCurrentMenu();
    void closeWindow();
    void setState(MenuState newState);
    std::string getLevelFilePath(int num);
    [[nodiscard]] bool getIsGameIsEnded() const;
};

class LevelPerformer : Performer {
private:
    std::unique_ptr<Platformer::Game> game = nullptr;
    LevelState state = LevelState::Empty;
    std::unique_ptr<Platformer::Statistics> statistics = nullptr;

public:
    explicit LevelPerformer(sf::RenderWindow &window_);
    [[maybe_unused]] void pause();
    [[maybe_unused]] void resume();
    [[maybe_unused]] void reset();
    [[maybe_unused]] void moveLeft();
    [[maybe_unused]] void moveRight();
    [[maybe_unused]] void moveDown();
    void jump();
    void stop();
    [[maybe_unused]] void exit();
    void setLevel(std::unique_ptr<Platformer::Game> game_);

    void setStatistics(std::unique_ptr<Platformer::Statistics> statistics_) {
        statistics = std::move(statistics_);
    }

    std::unique_ptr<Platformer::Game> &getLevel();
    std::unique_ptr<Platformer::Statistics> &getStatistics();
    LevelState getState();
    void setState(LevelState newState);
};

class ServerPerformer : Performer {
private:
    client::LevelClient client;
    std::string level_dir_path;
    std::set<std::string> chosen_local_levels;
    std::set<std::string> chosen_global_levels;

public:
    explicit ServerPerformer(
        sf::RenderWindow &window_,
        const std::string &address,
        const std::string &key_directory,
        std::string level_dir_path_
    );
    bool getLevel(const std::string &level_name);
    bool sendLevel(const std::string &level_name);
    bool deleteLevel(const std::string &level_name);
    [[maybe_unused]] bool checkLevel(const std::string &level_name);
    std::vector<std::string> loadAllAvailableLevelNames();

    void switch_in_local_set(const std::string &name);
    void switch_in_global_set(const std::string &name);
    void clear_local_levels_selection();
    void clear_global_levels_selection();

    void sendSelectedToServer();
    void getSelectedFromServer();
    void deleteSelectedFromServer();

    void debug_local_set();
    void debug_global_set();
};
}  // namespace control

#endif  // PLATFORMERGAMECPP_LEVEL_HPP
