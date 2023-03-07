#include "player.hpp"
#include <cmath>

namespace Platformer {

namespace observers {
PhysicsObserver::PhysicsObserver(Player *player_) : player(player_) {
}

void PhysicsObserver::updateCollision() {
    if (player->blockAbove().isSolid() && player->speed.get_y() < 0 ||
        player->blockBelow().isSolid() &&
            player->speed.get_y() > 0) {  // Hitting ceiling or ground
        player->speed = {player->speed.get_x(), 0};
    }
    if (player->blockLeft().isSolid() && player->speed.get_x() < 0 ||
        player->blockRight().isSolid() &&
            player->speed.get_x() > 0) {  // Hitting walls
        player->speed = {0, player->speed.get_y()};
    }
}

void PhysicsObserver::updateGravity() {
    if (!player->blockBelow().isSolid() && !player->blockBelow().isHangableOn()) {
        player->speed = {
                player->speed.get_x(), player->speed.get_y() + FREEFALL_ACCELERATION};
    }
}

void PhysicsObserver::updateMovement() {
    player->move(player->speed);
}

void PhysicsObserver::update() {
    updateGravity();
    updateCollision();
    updateMovement();
}
}  // namespace observers

Player::Player(Game *game_) : game(game_) {
}

Player::Player(Game *game_, Vector pos_) : game(game_), pos(pos_) {
    auto physObs =
        std::unique_ptr<observers::Observer>(new observers::PhysicsObserver(this
        ));
    observerCollection = std::vector<std::unique_ptr<observers::Observer>>();
    observerCollection.push_back(std::move(physObs));
}

Player::Player(Game *game_, Vector pos_, int width_, int height_)
    : Player(game_, pos_) {
    width = width_;
    height = height_;
}

void Player::moveLeft() {
    if (!blockLeft().isSolid()) {
        move({-PLAYER_SPEED, 0});
    }
    pose = Pose::LOOKING_LEFT;
}

void Player::moveRight() {
    if (!blockRight().isSolid()) {
        move({PLAYER_SPEED, 0});
    }
    pose = Pose::LOOKING_RIGHT;
}

void Player::jump() {
    if (blockBelow().isHangableOn()) {
        speed = {0, 0};
        move({0, -PLAYER_SPEED});
    } else if (blockBelow().isSolid() && !blockAbove().isSolid()) {
        speed = {speed.get_x(), -PLAYER_SPEED};
    }
}

void Player::moveDown() {
    if (blockBelow().isHangableOn()) {
        move({0, PLAYER_SPEED});
    }
}

void Player::move(Vector delta) {
    pos += delta;
}

void Player::notifyAll() {
    for (auto &obs : observerCollection) {
        obs->update();
    }
}

Block Player::blockInside() {
    return game->getBlock(divide(pos.get_x(), BLOCK_SIZE), divide(pos.get_y(), BLOCK_SIZE));
}

Block Player::blockAbove() {
    return game->getBlock(
        divide(pos.get_y() - height / 2, BLOCK_SIZE), divide(pos.get_x(), BLOCK_SIZE)
    );
}

Block Player::blockBelow() {
    return game->getBlock(
        divide(pos.get_y() + height / 2, BLOCK_SIZE), divide(pos.get_x(), BLOCK_SIZE)
    );
}

Block Player::blockLeft() {
    return game->getBlock(
        divide(pos.get_y(), BLOCK_SIZE), divide(pos.get_x() - width / 2, BLOCK_SIZE)
    );
}

Block Player::blockRight() {
    return game->getBlock(
        divide(pos.get_y(), BLOCK_SIZE), divide(pos.get_x() + width / 2, BLOCK_SIZE)
    );
}
}  // namespace Platformer
