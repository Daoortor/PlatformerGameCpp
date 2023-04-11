#ifndef PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
#define PLATFORMERGAMECPP_LEVEL_RENDER_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <memory>
#include <string>
#include "../../model/include/game.hpp"
#include "../../model/include/player.hpp"
#include "overlord.hpp"

namespace Platformer::gui {

sf::Sprite makeBlockSprite(
    const std::string &type,
    sf::Texture &blockTexture,
    int size,
    sf::Vector2f pos
);
std::vector<std::vector<sf::Sprite>> makeBlockSprites(
    Game &game,
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures
);
std::unique_ptr<sf::Texture> makeBlockTexture(const std::string &type);

class levelWindow {
    control::LevelOverlord &overlord;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::map<std::string, std::unique_ptr<sf::Texture>> blockTextures;
    std::vector<std::vector<sf::Sprite>> boardSprites;
    std::map<Platformer::Pose, sf::Texture> playerTextures;
    sf::Sprite playerSprite;

public:
    levelWindow(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &playerFilepath,
        const std::string &levelFilepath,
        control::LevelOverlord &overlord
    );
    void loadInWindow(sf::RenderWindow &window);
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
