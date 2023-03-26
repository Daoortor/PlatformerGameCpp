#include "../include/overlord.hpp"

[[maybe_unused]] void control::MainMenuOverlord::openMainMenu() {
    state = CurrentProcess::MainMenu;
}

[[maybe_unused]] Platformer::Game control::MainMenuOverlord::loadLevel(
    int level_num
) {
    state = CurrentProcess::LevelRunning;
    return Platformer::Game(level_paths[level_num]); // TODO: is correct? No problems with unique_ptr?
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
}

[[maybe_unused]] void control::LevelOverlord::moveRight() {
}

void control::LevelOverlord::jump() {
}

[[maybe_unused]] void control::LevelOverlord::exit() {
}
