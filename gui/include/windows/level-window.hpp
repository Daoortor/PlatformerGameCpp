#ifndef PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
#define PLATFORMERGAMECPP_LEVEL_RENDER_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <memory>
#include <string>
#include "elements/button.hpp"
#include "elements/scrollbar.hpp"
#include "elements/textbox.hpp"
#include "game.hpp"
#include "gui-constants.hpp"
#include "model-constants.hpp"
#include "performer.hpp"
#include "player.hpp"

namespace Platformer::gui {

float getBlockSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getGameFrameSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getTopLeftCorner(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getPlayerSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f
getCoordinates(sf::Vector2i pos, std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getPlayerCoordinates(std::unique_ptr<Platformer::Game> &game);
sf::Vector2i
getInGameCoordinates(sf::Vector2f pos, std::unique_ptr<Platformer::Game> &game);
[[maybe_unused]] sf::Sprite makeBlockSprite(
    const std::unique_ptr<sf::Texture> &blockTexture,
    float size,
    sf::Vector2f pos
);
[[maybe_unused]] std::vector<std::vector<sf::Sprite>> makeBlockSprites(
    Game &game,
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures,
    sf::Vector2<std::size_t> renderAreaTopLeftCorner = {0, 0},
    sf::Vector2<std::size_t> renderAreaBottomRightCorner = {0, 0}
);
std::unique_ptr<sf::Texture> makeBlockTexture(const std::string &type);

class LevelWindow {
protected:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::map<std::string, std::unique_ptr<sf::Texture>> blockTextures;
    std::vector<std::vector<sf::Sprite>> boardSprites;
    sf::Texture levelBeginTexture;
    sf::Sprite levelBeginSprite;
    sf::Texture levelEndTexture;
    sf::Sprite levelEndSprite;
    sf::Vector2f offset;

public:
    LevelWindow(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &miscFilepath,
        const std::string &levelFilepath,
        sf::Vector2f offset_ = {0, 0}
    );
    void loadLevel(sf::RenderWindow &window, std::unique_ptr<Game> &game);
    void updateAll(
        std::unique_ptr<Platformer::Game> &game,
        const std::string &miscFilepath
    );
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
