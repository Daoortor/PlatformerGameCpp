#include "windows/level-play.hpp"

namespace Platformer::gui {
LevelGameplayWindow::LevelGameplayWindow(
    unsigned int windowHeight,
    const std::string &backgroundTextureFilepath,
    const std::string &playerFilepath,
    const std::string &miscFilepath,
    const std::string &levelFilepath,
    control::LevelPerformer *levelPerformerPtr_
)
    : levelPerformerPtr(levelPerformerPtr_),
      LevelWindow(
          windowHeight,
          backgroundTextureFilepath,
          miscFilepath,
          levelFilepath
      ) {
    levelPerformerPtr->setLevel(std::make_unique<Game>(levelFilepath));
    playerTextures[Platformer::Pose::LOOKING_LEFT] = sf::Texture();
    playerTextures[Platformer::Pose::LOOKING_LEFT].loadFromFile(
        playerFilepath + "/player-left.png"
    );
    playerTextures[Platformer::Pose::LOOKING_RIGHT] = sf::Texture();
    playerTextures[Platformer::Pose::LOOKING_RIGHT].loadFromFile(
        playerFilepath + "/player-right.png"
    );
    playerSprite.setTexture(
        playerTextures[levelPerformerPtr->getLevel()->getPlayer()->getPose()]
    );
    float playerScale =
        getPlayerSize(levelPerformerPtr->getLevel()).y /
        static_cast<float>(playerSprite.getTexture()->getSize().y);
    playerSprite.setScale(playerScale, playerScale);
    sf::Vector2f playerCoordinates =
        getPlayerCoordinates(levelPerformerPtr->getLevel());
    playerSprite.setPosition(playerCoordinates.x, playerCoordinates.y);
}

void LevelGameplayWindow::loadInWindow(sf::RenderWindow &window) {
    window.clear();
    if (levelPerformerPtr->getLevel()->getPlayer()->contains(
            levelPerformerPtr->getLevel()->getEndPos()
        )) {
        levelPerformerPtr->setState(control::LevelState::Won);
        return;
    }
    if (levelPerformerPtr->getState() == control::LevelState::Running) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            levelPerformerPtr->jump();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            levelPerformerPtr->moveLeft();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            levelPerformerPtr->moveDown();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            levelPerformerPtr->moveRight();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            levelPerformerPtr->pause();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) {
            levelPerformerPtr->reset();
        }
    } else if (levelPerformerPtr->getState() == control::LevelState::Paused) {
        /*
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            levelPerformerPtr.resume();
            std::cout << "Game resumed\n" << '\n';
        } // dubs as 'Resume' button in Pause menu
         */
        // TODO: some kind of timer to check there is no loop pause-resume,
        //  as is right now; or should actually rewrite button to isKeyReleased
    }
    backgroundSprite.setTexture(backgroundTexture);
    // TODO: resolve texture loss that occurs after removing line above
    loadLevel(window, levelPerformerPtr->getLevel());
    levelPerformerPtr->getLevel()->update();
    sf::Vector2f playerCoordinates =
        getPlayerCoordinates(levelPerformerPtr->getLevel());
    playerSprite.setPosition(playerCoordinates.x, playerCoordinates.y);
    playerSprite.setTexture(
        playerTextures[levelPerformerPtr->getLevel()->getPlayer()->getPose()]
    );
    levelEndSprite.setTexture(levelEndTexture);
    window.draw(levelEndSprite);
    // TODO: resolve texture loss that occurs after removing line above
    window.draw(playerSprite);
    window.display();
}
}  // namespace Platformer::gui