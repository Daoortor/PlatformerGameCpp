#include "../include/performer.hpp"

namespace control {
[[maybe_unused]] void MenuPerformer::openMainMenu() {
    state = MenuState::MainMenu;
}

[[maybe_unused]] Platformer::Game
MenuPerformer::loadLevel(LevelPerformer &levelPerformer, int level_num) {
    state = MenuState::Empty;
    levelPerformer.setState(LevelState::Running);
    return Platformer::Game(level_paths[level_num]
    );  // TODO: rewrite with proper method owner & state field usage;
        //  right now it doesn't do anything
}

[[maybe_unused]] void MenuPerformer::openLoadLevelMenu() {
    state = MenuState::LoadMenu;
}

void MenuPerformer::openPauseMenu() {
    state = MenuState::PauseMenu;
}

MenuState MenuPerformer::getState() {
    return state;
}

void MenuPerformer::closeCurrentMenu() {
    state = MenuState::Empty;
}

void MenuPerformer::closeWindow() {
    window.close();
}

[[maybe_unused]] void LevelPerformer::pause() {
    state = LevelState::Paused;
}

[[maybe_unused]] void LevelPerformer::resume() {
    state = LevelState::Running;
}

[[maybe_unused]] void LevelPerformer::moveLeft() {
    game->getPlayer()->moveLeft();
}

[[maybe_unused]] void LevelPerformer::moveRight() {
    game->getPlayer()->moveRight();
}

[[maybe_unused]] void LevelPerformer::moveDown() {
    game->getPlayer()->moveDown();
}

void LevelPerformer::jump() {
    game->getPlayer()->jump();
}

[[maybe_unused]] void LevelPerformer::exit() {
    // TODO: level reset
    state = LevelState::Empty;
}

LevelPerformer::LevelPerformer(sf::RenderWindow &window_) : Performer(window_) {
    state = LevelState::Empty;
    // TODO: rewrite after solving LevelPerformer - Game dependencies
}

void LevelPerformer::setLevel(std::unique_ptr<Platformer::Game> game_) {
    game = std::move(game_);
}

LevelState LevelPerformer::getState() {
    return state;
}

void LevelPerformer::setState(LevelState newState) {
    state = newState;
}

std::unique_ptr<Platformer::Game> &LevelPerformer::getLevel() {
    return game;
}
}  // namespace control
