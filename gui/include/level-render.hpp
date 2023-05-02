#ifndef PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
#define PLATFORMERGAMECPP_LEVEL_RENDER_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <memory>
#include <string>
#include "../../model/include/game.hpp"
#include "../../model/include/player.hpp"
#include "../include/button.hpp"
#include "performer.hpp"
#include "scrollbar.hpp"

namespace Platformer::gui {

float getBlockSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getGameFrameSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getTopLeftCorner(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getPlayerSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f
getCoordinates(utilities::Vector pos, std::unique_ptr<Platformer::Game> &game);
sf::Vector2f
getPlayerCoordinates(sf::Vector2f pos, std::unique_ptr<Platformer::Game> &game);
sf::Vector2i
getInGameCoordinates(sf::Vector2f pos, std::unique_ptr<Platformer::Game> &game);
[[maybe_unused]] sf::Sprite makeBlockSprite(
    const std::unique_ptr<sf::Texture> &blockTexture,
    float size,
    sf::Vector2f pos
);
[[maybe_unused]] std::vector<std::vector<sf::Sprite>> makeBlockSprites(
    Game &game,
    std::map<std::string, std::unique_ptr<sf::Texture>> &blockTextures
);
std::unique_ptr<sf::Texture> makeBlockTexture(const std::string &type);

class LevelWindow {
protected:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::map<std::string, std::unique_ptr<sf::Texture>> blockTextures;
    std::vector<std::vector<sf::Sprite>> boardSprites;

public:
    LevelWindow(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &miscFilepath,
        const std::string &levelFilepath
    );
    void loadLevel(sf::RenderWindow &window, std::unique_ptr<Game> &game);
};

class LevelGameplayWindow : public LevelWindow {
    control::LevelPerformer *levelPerformerPtr;
    std::map<Platformer::Pose, sf::Texture> playerTextures;
    sf::Sprite playerSprite;
    sf::Texture levelEndTexture;
    sf::Sprite levelEndSprite;

public:
    explicit LevelGameplayWindow(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &playerFilepath,
        const std::string &miscFilepath,
        const std::string &levelFilepath,
        control::LevelPerformer *levelPerformerPtr
    );
    void loadInWindow(sf::RenderWindow &window);
};

class LevelEditor : public LevelWindow {
    std::unique_ptr<Game> game;
    interface::Scrollbar blockSelectionBar;
    sf::RectangleShape levelBorder;
    std::string blockChosen;

public:
    LevelEditor(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &blockFilepath,
        const std::string &miscFilepath,
        const std::string &levelFilepath
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void addBlock(sf::Vector2u pos, const std::string &name);
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
