#ifndef PLATFORMERGAMECPP_CONSTANTS_HPP_
#define PLATFORMERGAMECPP_CONSTANTS_HPP_

#include <vector>
#include <string>

namespace Platformer::gui {
    unsigned int const WINDOW_WIDTH = 800;
    unsigned int const WINDOW_HEIGHT = 800;
    unsigned int const LEVEL_WIDTH = 600;
    unsigned int const LEVEL_HEIGHT = 600;
    std::vector<std::string> const BLOCK_NAMES = {"ladder", "stone", "air"};
}

#endif //PLATFORMERGAMECPP_CONSTANTS_HPP_
