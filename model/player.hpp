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
    Vector pos;
    Vector speed;
    int height = BLOCK_SIZE * 3 / 2;
    int width = BLOCK_SIZE * 3 / 4;
    Pose pose = Pose::LOOKING_RIGHT;
    std::vector<std::unique_ptr<observers::Observer>> observerCollection;
    Game *game = nullptr;

public:
    Player() = default;
    explicit Player(Game *game_);
    Player(Game *game_, Vector pos_);
    Player(Game *game_, Vector pos_, int width_, int height_);

    Vector getPos() {
        return pos;
    }

    Vector getSpeed() {
        return speed;
    }

    void moveLeft();
    void moveRight();
    void jump();
    void moveDown();
    void move(Vector delta);
    void notifyAll();
    Block blockInside();
    Block blockAbove();
    Block blockBelow();
    Block blockLeft();
    Block blockRight();

    friend observers::PhysicsObserver;
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_PLAYER_HPP_
