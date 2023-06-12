#ifndef PLATFORMERGAMECPP_LEVEL_PLAY_HPP_
#define PLATFORMERGAMECPP_LEVEL_PLAY_HPP_

#include "level-window.hpp"

namespace Platformer::gui {
class LevelGameplayWindow : public LevelWindow {
    control::LevelPerformer *levelPerformerPtr;
    std::map<Platformer::Pose, sf::Texture> playerTextures;
    sf::Sprite playerSprite;

public:
    explicit LevelGameplayWindow(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &playerFilepath,
        const std::string &miscFilepath,
        const std::string &levelFilepath,
        control::LevelPerformer *levelPerformerPtr_
    );
    void loadInWindow(sf::RenderWindow &window);
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_PLAY_HPP_
