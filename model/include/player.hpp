#ifndef PLATFORMERGAMECPP_PLAYER_HPP_
#define PLATFORMERGAMECPP_PLAYER_HPP_

#include <memory>
#include <ostream>
#include <vector>
#include "block.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "observers.hpp"
#include "utilities.hpp"

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
    utilities::Vector pos;
    utilities::Vector speed;
    int height = BLOCK_SIZE * 3 / 2;
    int width = BLOCK_SIZE * 3 / 4;
    Pose pose = Pose::LOOKING_RIGHT;
    std::vector<std::unique_ptr<observers::Observer>> observerCollection;
    Game *game = nullptr;

    utilities::Vector
    distByVector(utilities::Vector pos, utilities::Vector moveVector);

public:
    Player() = default;
    explicit Player(Game *game_);
    Player(Game *game_, utilities::Vector pos_);
    Player(Game *game_, utilities::Vector pos_, int width_, int height_);

    utilities::Vector getPos() {
        return pos;
    }

    utilities::Vector getSpeed() {
        return speed;
    }

    void moveLeft();
    void moveRight();
    void jump();
    void moveDown();
    void move(utilities::Vector delta);
    void notifyAll();

    const std::unique_ptr<Block> &blockInside();
    const std::unique_ptr<Block> &blockAbove();
    const std::unique_ptr<Block> &blockBelow();
    const std::unique_ptr<Block> &blockLeft();
    const std::unique_ptr<Block> &blockRight();

    int distAbove();
    int distBelow();
    int distLeft();
    int distRight();

    friend observers::PhysicsObserver;
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_PLAYER_HPP_
