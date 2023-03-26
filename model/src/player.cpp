#include "../include/player.hpp"
#include <algorithm>

namespace Platformer {

namespace observers {
PhysicsObserver::PhysicsObserver(Player *player_) : player(player_) {
}

void PhysicsObserver::updateCollision() {
    if ((player->blockAbove()->isSolid() && player->speed.get_y() < 0) ||
        (player->blockBelow()->isSolid() && player->speed.get_y() > 0
        )) {  // Hitting ceiling or ground
        player->speed = {player->speed.get_x(), 0};
    }
    if ((player->blockLeft()->isSolid() && player->speed.get_x() < 0) ||
        (player->blockRight()->isSolid() && player->speed.get_x() > 0
        )) {  // Hitting walls
        player->speed = {0, player->speed.get_y()};
    }
}

void PhysicsObserver::updateGravity() {
    if (!player->blockBelow()->isSolid() &&
        !player->blockBelow()->isHangableOn()) {
        player->speed = {
            player->speed.get_x(),
            player->speed.get_y() + FREEFALL_ACCELERATION};
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

Player::Player(Game *game_, utilities::Vector pos_) {
    game = game_;
    pos = pos_;
    auto physObs =
        std::unique_ptr<observers::Observer>(new observers::PhysicsObserver(this
        ));
    observerCollection = std::vector<std::unique_ptr<observers::Observer>>();
    observerCollection.push_back(std::move(physObs));
}

Player::Player(Game *game_, utilities::Vector pos_, int width_, int height_)
    : Player(game_, pos_) {
    width = width_;
    height = height_;
}

void Player::moveLeft() {
    move({-PLAYER_SPEED, 0});
    pose = Pose::LOOKING_LEFT;
}

void Player::moveRight() {
    move({PLAYER_SPEED, 0});
    pose = Pose::LOOKING_RIGHT;
}

void Player::jump() {
    if (blockBelow()->isHangableOn()) {
        speed = {0, 0};
        move({0, -PLAYER_SPEED});
    } else if (blockBelow()->isSolid() && !blockAbove()->isSolid()) {
        speed = {speed.get_x(), -PLAYER_SPEED};
    }
}

void Player::moveDown() {
    if (blockBelow()->isHangableOn()) {
        move({0, PLAYER_SPEED});
    }
}

void Player::move(utilities::Vector delta) {
    int deltaX, deltaY;
    if (delta.get_x() > 0) {
        deltaX = std::min(delta.get_x(), distRight());
    } else {
        deltaX = std::max(delta.get_x(), -distLeft());
    }
    if (delta.get_y() > 0) {
        deltaY = std::min(delta.get_y(), distBelow());
    } else {
        deltaY = std::max(delta.get_y(), -distAbove());
    }
    pos += {deltaX, deltaY};
}

void Player::notifyAll() {
    for (auto &obs : observerCollection) {
        obs->update();
    }
}

void Player::act(char command) {
    if (command == 'W') {
        jump();
    } else if (command == 'A') {
        moveLeft();
    } else if (command == 'S') {
        moveDown();
    } else if (command == 'D') {
        moveRight();
    }
}

const std::unique_ptr<Block> &Player::blockInside() {
    return game->getBoardObject().getBlockByCoordinates(
        {pos.get_x(), pos.get_y()}
    );
}

const std::unique_ptr<Block> &Player::blockAbove() {
    return game->getBoardObject().getBlockByCoordinates(
        {pos.get_x(), pos.get_y() - height / 2}
    );
}

const std::unique_ptr<Block> &Player::blockBelow() {
    return game->getBoardObject().getBlockByCoordinates(
        {pos.get_x(), pos.get_y() + height / 2}
    );
}

const std::unique_ptr<Block> &Player::blockLeft() {
    return game->getBoardObject().getBlockByCoordinates(
        {pos.get_x() - width / 2, pos.get_y()}
    );
}

const std::unique_ptr<Block> &Player::blockRight() {
    return game->getBoardObject().getBlockByCoordinates(
        {pos.get_x() + width / 2, pos.get_y()}
    );
}

utilities::Vector
Player::distByVector(utilities::Vector position, utilities::Vector moveVector) {
    utilities::Vector currentCordinates{
        utilities::divide(position.get_x(), BLOCK_SIZE) * BLOCK_SIZE,
        utilities::divide(position.get_y(), BLOCK_SIZE) * BLOCK_SIZE};
    utilities::Vector padding;
    if (moveVector.get_x() < 0) {
        padding = {1, 0};
    }
    if (moveVector.get_y() < 0) {
        padding = {0, 1};
    }
    currentCordinates -= padding;
    for (int it = 0; it < MAX_FIELD_SIZE; it++) {
        if (game->getBoardObject()
                .getBlockByCoordinates(currentCordinates)
                ->isSolid()) {
            return utilities::Vector(
                       std::abs(position.get_x() - currentCordinates.get_x()),
                       std::abs(position.get_y() - currentCordinates.get_y())
                   ) -
                   padding;
        }
        currentCordinates += moveVector;
    }
    return {INT32_MAX, INT32_MAX};
}

int Player::distAbove() {
    utilities::Vector upperLeftCorner{
        pos.get_x() - width / 2 + 1, pos.get_y() - height / 2};
    utilities::Vector upperRightCorner{
        pos.get_x() + width / 2 - 1, pos.get_y() - height / 2};
    return std::min(
        distByVector(upperLeftCorner, {0, -BLOCK_SIZE}).get_y(),
        distByVector(upperRightCorner, {0, -BLOCK_SIZE}).get_y()
    );
}

int Player::distBelow() {
    utilities::Vector lowerLeftCorner{
        pos.get_x() - width / 2 + 1, pos.get_y() + height / 2};
    utilities::Vector lowerRightCorner{
        pos.get_x() + width / 2 - 1, pos.get_y() + height / 2};
    return std::min(
        distByVector(lowerLeftCorner, {0, BLOCK_SIZE}).get_y(),
        distByVector(lowerRightCorner, {0, BLOCK_SIZE}).get_y()
    );
}

int Player::distLeft() {
    utilities::Vector lowerLeft{
        pos.get_x() - width / 2, pos.get_y() + height / 2 - 1};
    utilities::Vector middleLeft{pos.get_x() - width / 2, pos.get_y()};
    utilities::Vector upperLeft{
        pos.get_x() - width / 2, pos.get_y() - height / 2 + 1};
    return std::min(
        {distByVector(lowerLeft, {-BLOCK_SIZE, 0}).get_x(),
         distByVector(middleLeft, {-BLOCK_SIZE, 0}).get_x(),
         distByVector(upperLeft, {-BLOCK_SIZE, 0}).get_x()}
    );
}

int Player::distRight() {
    utilities::Vector lowerRight{
        pos.get_x() + width / 2, pos.get_y() + height / 2 - 1};
    utilities::Vector middleRight{pos.get_x() + width / 2, pos.get_y()};
    utilities::Vector upperRight{
        pos.get_x() + width / 2, pos.get_y() - height / 2 + 1};
    return std::min(
        {distByVector(lowerRight, {BLOCK_SIZE, 0}).get_x(),
         distByVector(middleRight, {BLOCK_SIZE, 0}).get_x(),
         distByVector(upperRight, {BLOCK_SIZE, 0}).get_x()}
    );
}
}  // namespace Platformer
