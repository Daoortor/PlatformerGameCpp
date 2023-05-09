#ifndef PLATFORMERGAMECPP_PLAYER_HPP_
#define PLATFORMERGAMECPP_PLAYER_HPP_

#include <memory>
#include <ostream>
#include <vector>
#include "../../tools/utilities.hpp"
#include "block.hpp"
#include "game.hpp"
#include "model-constants.hpp"
#include "observers.hpp"

namespace Platformer {

enum class Pose { LOOKING_LEFT, LOOKING_RIGHT };

class Player;

namespace observers {
class PhysicsObserver : public Observer {
    Player *player;
    void updateCollision();
    void updateGravity();
    void updateMovement();

public:
    explicit PhysicsObserver(Player *player_);
    void update() override;
};
}  // namespace observers

class Player {
    sf::Vector2i pos;
    sf::Vector2i speed;
    int height = BLOCK_SIZE * 3 / 2;
    int width = BLOCK_SIZE * 3 / 4;
    Pose pose = Pose::LOOKING_RIGHT;
    std::vector<std::unique_ptr<observers::Observer>> observerCollection;
    Game *game = nullptr;

    sf::Vector2i distByVector(sf::Vector2i pos, sf::Vector2i moveVector);

    std::vector<sf::Vector2i> coordsAbove();
    std::vector<sf::Vector2i> coordsBelow();

    std::vector<const Block *> blocksAbove();
    std::vector<const Block *> blocksBelow();

public:
    Player() = default;
    explicit Player(Game *game_);
    Player(Game *game_, sf::Vector2i pos_);
    Player(Game *game_, sf::Vector2i pos_, int width_, int height_);

    [[nodiscard]] sf::Vector2i getPos() const {
        return pos;
    }

    [[nodiscard]] sf::Vector2i getSpeed() const {
        return speed;
    }

    [[nodiscard]] Pose getPose() const {
        return pose;
    }

    [[nodiscard]] sf::Vector2i getSize() const {
        return {width, height};
    }

    void moveLeft();
    void moveRight();
    void jump();
    void moveDown();
    void move(sf::Vector2i delta);
    void notifyAll();

    int distAbove();
    int distBelow();
    int distLeft();
    int distRight();

    bool isHanging();
    bool isStanding();
    bool canJump();

    [[nodiscard]] bool contains(sf::Vector2i position) const;

    friend observers::PhysicsObserver;
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_PLAYER_HPP_
