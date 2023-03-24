#include "../include/overlord.hpp"

void control::MainMenuOverlord::openMainMenu() {
    state = CurrentProcess::MainMenu;
}

void control::MainMenuOverlord::loadLevel(int level_num) {
}

void control::MainMenuOverlord::openLoadLevelMenu() {
    state = CurrentProcess::LoadMenu;
}

control::CurrentProcess control::Overlord::getState() {
    return state;
}

void control::Overlord::setState(CurrentProcess newState) {
    state = newState;
}

void control::LevelOverlord::pauseOrResume() {
}

void control::LevelOverlord::moveLeft() {
}

void control::LevelOverlord::moveRight() {
}

void control::LevelOverlord::jump() {
}

void control::LevelOverlord::exit() {
}
