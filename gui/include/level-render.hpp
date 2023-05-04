#ifndef PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
#define PLATFORMERGAMECPP_LEVEL_RENDER_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <memory>
#include <string>
#include "button.hpp"
#include "game.hpp"
#include "performer.hpp"
#include "player.hpp"
#include "scrollbar.hpp"
#include "textbox.hpp"

namespace Platformer::gui {

float getBlockSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getGameFrameSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getTopLeftCorner(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f getPlayerSize(std::unique_ptr<Platformer::Game> &game);
sf::Vector2f
getCoordinates(sf::Vector2i pos, std::unique_ptr<Platformer::Game> &game);
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
    sf::Texture levelBeginTexture;
    sf::Sprite levelBeginSprite;
    sf::Texture levelEndTexture;
    sf::Sprite levelEndSprite;

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

enum class EditorState { Idle, BlockChosen, StartPosChosen, EndPosChosen };

class LevelEditor : public LevelWindow {
    std::unique_ptr<Game> game;
    interface::Scrollbar blockSelectionBar;
    sf::RectangleShape levelBorder;
    std::string blockChosen;
    EditorState state;
    interface::Textbox levelNameTextbox;
    interface::RectangleButton saveButton;
    interface::ButtonWithImage startPosButton;
    interface::ButtonWithImage levelEndButton;

public:
    LevelEditor(
        unsigned int windowHeight,
        const std::string &backgroundTextureFilepath,
        const std::string &blockFilepath,
        const std::string &miscFilepath,
        const std::string &emptyLevelFilepath,
        const std::string &levelsFilepath,
        const sf::Font &font
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void addBlock(sf::Vector2u pos, const std::string &name);
    void setStartPos(sf::Vector2i pos);
    void setEndPos(sf::Vector2i pos);
    void deactivateAll();
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
