#ifndef PLATFORMERGAMECPP_CONSTANTS_HPP_
#define PLATFORMERGAMECPP_CONSTANTS_HPP_

#include <SFML/Window.hpp>
#include <string>
#include <vector>

namespace Platformer::gui {
const unsigned int WINDOW_WIDTH = sf::VideoMode::getDesktopMode().width * 2 / 5;
const unsigned int WINDOW_HEIGHT =
    sf::VideoMode::getDesktopMode().height * 11 / 20;
const unsigned int LEVEL_WIDTH = WINDOW_WIDTH * 2 / 3;
const unsigned int LEVEL_HEIGHT = WINDOW_HEIGHT * 2 / 3;
const std::vector<std::string> BLOCK_NAMES = {"ladder", "stone", "air"};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_CONSTANTS_HPP_
