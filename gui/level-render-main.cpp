#include "include/gui-constants.hpp"
#include "include/level-render.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(Platformer::gui::WINDOW_WIDTH,
                                          Platformer::gui::WINDOW_HEIGHT), "Platformer",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Set up background
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("assets/textures/background.png");
    sf::Vector2u TextureSize = backgroundTexture.getSize();
    sf::Vector2u WindowSize = window.getSize();
    float BackgroundScale = static_cast<float>(WindowSize.y) / static_cast<float>(TextureSize.y);
    sf::Sprite background;
    background.setTexture(backgroundTexture);
    background.setScale(BackgroundScale, BackgroundScale);

    // Load block textures
    std::map<std::string, std::unique_ptr<sf::Texture>> blockTextures;
    for (const std::string& blockType : Platformer::gui::BLOCK_NAMES) {
        blockTextures[blockType] = Platformer::gui::makeBlockTexture(blockType);
    }

    Platformer::Game game("levels/t01-box-with-ladder.json");
    auto boardSprites = Platformer::gui::makeBlockSprites(game, blockTextures);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.draw(background);
        for (const auto& row : boardSprites) {
            for (const auto& sprite : row) {
                window.draw(sprite);
            }
        }
        window.display();
    }

    return 0;
}
