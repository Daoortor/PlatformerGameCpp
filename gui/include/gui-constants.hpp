#ifndef PLATFORMERGAMECPP_CONSTANTS_HPP_
#define PLATFORMERGAMECPP_CONSTANTS_HPP_

#include <SFML/Window.hpp>
#include <string>
#include <vector>

namespace Platformer::gui {
const unsigned int WINDOW_HEIGHT = 600;
const unsigned int WINDOW_WIDTH = WINDOW_HEIGHT * 4 / 3;

namespace levels {
const unsigned int LEVEL_HEIGHT = WINDOW_HEIGHT * 2 / 3;
const unsigned int LEVEL_WIDTH = LEVEL_HEIGHT * 7 / 5;
const unsigned int INITIAL_LEVEL_BLOCK_HEIGHT = 5;
const std::vector<std::string> BLOCK_NAMES = {"ladder", "stone", "air"};
const std::string EMPTY_LEVEL_NAME = "empty.json";
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
static sf::Color CHOSEN_COLOR = sf::Color(0, 255, 0, 128);
}  // namespace colors
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_CONSTANTS_HPP_
