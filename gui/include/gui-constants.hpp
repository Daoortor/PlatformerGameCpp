#ifndef PLATFORMERGAMECPP_CONSTANTS_HPP_
#define PLATFORMERGAMECPP_CONSTANTS_HPP_

#include <SFML/Window.hpp>
#include <string>
#include <vector>

namespace Platformer::gui {
const unsigned int WINDOW_WIDTH = sf::VideoMode::getDesktopMode().width * 2 / 5;
const unsigned int WINDOW_HEIGHT =
    sf::VideoMode::getDesktopMode().height * 11 / 20;

namespace levels {
const unsigned int LEVEL_WIDTH = WINDOW_WIDTH * 2 / 3;
const unsigned int LEVEL_HEIGHT = WINDOW_HEIGHT * 2 / 3;
const std::vector<std::string> BLOCK_NAMES = {"ladder", "stone", "air"};
const std::string EMPTY_LEVEL_NAME = "empty-level.json";
}  // namespace levels

namespace colors {
const auto BUTTON_COLORS_LIST = std::vector{
    sf::Color(255, 0, 48, 192), sf::Color(118, 114, 111, 192),
    sf::Color(0, 209, 255, 192), sf::Color(255, 95, 0, 192)};
static sf::Color ITEM_CHOSEN_COLOR(0, 255, 0, 127);
const auto TEXTBOX_COLORS_LIST =
    std::vector{sf::Color(128, 128, 128),    sf::Color(0, 0, 0),
                sf::Color(255, 0, 48, 192),  sf::Color(118, 114, 111, 192),
                sf::Color(0, 209, 255, 192), sf::Color(255, 95, 0, 192)};
}  // namespace colors
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_CONSTANTS_HPP_
