#include "../include/overlord.hpp"

[[maybe_unused]] void control::MainMenuOverlord::openMainMenu() {
    state = CurrentProcess::MainMenu;
}

[[maybe_unused]] Platformer::Game control::MainMenuOverlord::loadLevel(
    int level_num
) {
    state = CurrentProcess::LevelRunning;
    return Platformer::Game(level_paths[level_num]
    );  // TODO: is correct? No problems with unique_ptr?
}

[[maybe_unused]] void control::MainMenuOverlord::openLoadLevelMenu() {
    state = CurrentProcess::LoadMenu;
}

control::CurrentProcess control::Overlord::getState() {
    return state;
}

void control::Overlord::setState(CurrentProcess newState) {
    state = newState;
}

[[maybe_unused]] void control::LevelOverlord::pauseOrResume() {
}

[[maybe_unused]] void control::LevelOverlord::moveLeft() {
    game->getPlayer()->moveLeft();
}

[[maybe_unused]] void control::LevelOverlord::moveRight() {
    game->getPlayer()->moveRight();
}

[[maybe_unused]] void control::LevelOverlord::moveDown() {
    game->getPlayer()->moveDown();
}

void control::LevelOverlord::jump() {
    game->getPlayer()->jump();
}

[[maybe_unused]] void control::LevelOverlord::exit() {
}

control::LevelOverlord::LevelOverlord(sf::RenderWindow &window_)
    : Overlord(window_) {
}

void control::LevelOverlord::setLevel(std::unique_ptr<Platformer::Game> game_) {
    game = std::move(game_);
}
