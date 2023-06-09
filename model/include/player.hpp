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
class PlayerObserver : public Observer {
protected:
    Player *player;

public:
    explicit PlayerObserver(Player *player_) : player(player_){};

    void setPlayer(Player *player_) {
        player = player_;
    }

    Player *getPlayer() {
        return player;
    }
};

class PhysicsObserver : public PlayerObserver {
    void updateCollision();
    void updateGravity();
    void updateMovement();

public:
    explicit PhysicsObserver(Player *player_) : PlayerObserver(player_){};
    void update() override;

    ~PhysicsObserver() override = default;
};

class DeathObserver : public PlayerObserver {
    void updateKillerDeath();
    void updateFallDeath();

public:
    explicit DeathObserver(Player *player_) : PlayerObserver(player_){};
    void update() override;

    ~DeathObserver() override = default;
};
}  // namespace observers

class Player {
    sf::Vector2i pos;
    sf::Vector2i speed;
    int ownSpeed = 0;
    int height = BLOCK_SIZE * 3 / 2;
    int width = BLOCK_SIZE * 3 / 4;
    Pose pose = Pose::LOOKING_RIGHT;
    std::vector<std::unique_ptr<observers::PlayerObserver>> observerCollection;
    Game *game = nullptr;

    sf::Vector2i distByVector(sf::Vector2i pos, sf::Vector2i moveVector);

    [[nodiscard]] std::vector<sf::Vector2i> coordsAbove() const;
    [[nodiscard]] std::vector<sf::Vector2i> coordsBelow() const;
    [[nodiscard]] std::vector<sf::Vector2i> coordsLeft() const;
    [[nodiscard]] std::vector<sf::Vector2i> coordsRight() const;

    std::vector<const Block *> blocksAbove();
    std::vector<const Block *> blocksBelow();
    std::vector<const Block *> blocksLeft();
    std::vector<const Block *> blocksRight();

public:
    Player() = default;
    explicit Player(Game *game_);
    Player(Game *game_, sf::Vector2i pos_);
    Player(Game *game_, sf::Vector2i pos_, int width_, int height_);

    [[nodiscard]] sf::Vector2i getPos() const {
        return pos;
    }

    void setPos(sf::Vector2i pos_) {
        pos = pos_;
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

    [[nodiscard]] Game *getGame() const {
        return game;
    }

    [[nodiscard]] const std::vector<
        std::unique_ptr<observers::PlayerObserver>> &
    getObservers() {
        return observerCollection;
    }

    void moveLeft();
    void moveRight();
    void jump();
    void moveDown();
    void stop();
    void move(sf::Vector2i delta);
    void notifyAll();

    int distAbove();
    int distBelow();
    int distLeft();
    int distRight();

    float getBounce();

    bool isHanging();
    bool isStanding();
    bool isDying();
    bool canJump();

    [[nodiscard]] bool contains(sf::Vector2i position) const;

    void reset();

    friend observers::PhysicsObserver;
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_PLAYER_HPP_
