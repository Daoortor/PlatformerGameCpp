#include "../include/performer.hpp"

namespace control {
[[maybe_unused]] void MenuPerformer::openMainMenu() {
    state = MenuState::MainMenu;
}

[[maybe_unused]] void control::MenuPerformer::loadLevel(
    LevelPerformer &levelPerformer
) {
    setState(control::MenuState::Empty);
    levelPerformer.setState(control::LevelState::Running);
    // TODO: rewrite or rename, cross or underpants
}

[[maybe_unused]] void MenuPerformer::openLoadLevelMenu() {
    state = MenuState::LoadMenu;
}

[[maybe_unused]] void MenuPerformer::openWonMenu() {
    state = MenuState::WonMenu;
}

void MenuPerformer::openPauseMenu() {
    state = MenuState::PauseMenu;
}

MenuState MenuPerformer::getState() {
    return state;
}

void MenuPerformer::setState(MenuState newState) {
    state = newState;
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

[[maybe_unused]] void LevelPerformer::reset() {
    game = std::move(std::make_unique<Platformer::Game>(
        "../model/levels/t02-hard-jumps.json"
    ));
    // TODO: redo
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

std::unique_ptr<Platformer::Statistics> &LevelPerformer::getStatistics() {
    return statistics;
}

std::string MenuPerformer::getLevelFilePath(int num) {
    return level_paths[num];
}
}  // namespace control
