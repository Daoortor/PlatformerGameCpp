#ifndef PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
#define PLATFORMERGAMECPP_LEVEL_RENDER_HPP_

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <memory>
#include "../../model/include/game.hpp"
#include "../../model/include/player.hpp"

namespace Platformer::gui {
    struct spriteWithTexture {
        sf::Texture texture;
        sf::Sprite sprite;
    };

    sf::Sprite makeBlockSprite(const std::string& type, sf::Texture& blockTexture, int size, sf::Vector2f pos);
    std::vector<std::vector<sf::Sprite>> makeBlockSprites(Game &game,
                                             std::map<std::string, std::unique_ptr<sf::Texture>>& blockTextures);
    std::unique_ptr<sf::Texture> makeBlockTexture(const std::string& type);

    class levelWindow {
        Platformer::Game game;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;
        std::map<std::string, std::unique_ptr<sf::Texture>> blockTextures;
        std::vector<std::vector<sf::Sprite>> boardSprites;

    public:
        levelWindow(int windowWidth, int windowHeight, const std::string &backgroundTextureFilepath,
                    const std::string &levelFilepath);
        void loadInWindow(sf::RenderWindow &window);
    };
}

#endif //PLATFORMERGAMECPP_LEVEL_RENDER_HPP_
