
#include "../include/player.hpp"
#include <algorithm>
#include <iostream>

namespace Platformer {

namespace observers {
PhysicsObserver::PhysicsObserver(Player *player_) : player(player_) {
}

void PhysicsObserver::updateCollision() {
    if ((player->isStanding() && player->speed.get_y() > 0)) {
        player->speed = {player->speed.get_x(), 0};
    } else if (!player->canJump()) {
        int newSpeed = -static_cast<int>(
            static_cast<float>(player->speed.get_y()) * BOUNCE_COEFFICIENT
        );
        player->speed = {player->speed.get_x(), newSpeed};
    }
    if ((player->distLeft() == 0 && player->speed.get_x() < 0) ||
        (player->distRight() == 0 && player->speed.get_x() > 0
        )) {  // Hitting walls
        player->speed = {0, player->speed.get_y()};
    }
}

void PhysicsObserver::updateGravity() {
    if (!player->isStanding() && !player->isHanging()) {
        player->speed = {
            player->speed.get_x(),
            player->speed.get_y() + FREEFALL_ACCELERATION};
    } else if (player->isHanging()) {
        player->speed = {player->speed.get_x(), 0};
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
    if (isStanding() && canJump()) {
        speed = {speed.get_x(), -PLAYER_SPEED};
    } else if (isHanging()) {
        speed = {0, 0};
        move({0, -PLAYER_SPEED});
    }
}

void Player::moveDown() {
    if (isHanging()) {
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

std::vector<utilities::Vector> Player::coordsAbove() {
    std::vector<utilities::Vector> result;
    utilities::Vector currentVector{
        pos.get_x() - width / 2 + 1, pos.get_y() - height / 2 - 2};
    while (currentVector.get_x() <= pos.get_x() + width / 2 + 1) {
        result.push_back(currentVector);
        currentVector += {BLOCK_SIZE, 0};
    }
    utilities::Vector upperRightCorner{
        pos.get_x() + width / 2 - 1, pos.get_y() - height / 2 - 2};
    result.push_back(upperRightCorner);
    return result;
}

std::vector<utilities::Vector> Player::coordsBelow() {
    std::vector<utilities::Vector> result;
    utilities::Vector currentVector{
        pos.get_x() - width / 2 + 1, pos.get_y() + height / 2};
    while (currentVector.get_x() <= pos.get_x() + width / 2 + 1) {
        result.push_back(currentVector);
        currentVector += {BLOCK_SIZE, 0};
    }
    utilities::Vector upperRightCorner{
        pos.get_x() + width / 2 - 1, pos.get_y() + height / 2};
    result.push_back(upperRightCorner);
    return result;
}

std::vector<const Block *> Player::blocksAbove() {
    auto coords = coordsAbove();
    std::vector<const Block *> result(coordsBelow().size());
    std::transform(
        coords.begin(), coords.end(), result.begin(),
        [this](utilities::Vector position) {
            return game->getBoardObject()
                .getBlockByCoordinates(position - utilities::Vector(0, 1))
                .get();
        }
    );
    return result;
}

std::vector<const Block *> Player::blocksBelow() {
    auto coords = coordsBelow();
    std::vector<const Block *> result(coordsBelow().size());
    std::transform(
        coords.begin(), coords.end(), result.begin(),
        [this](utilities::Vector position) {
            return game->getBoardObject().getBlockByCoordinates(position).get();
        }
    );
    return result;
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
    auto coordsAbove_ = coordsAbove();
    std::vector<int> distances(coordsAbove_.size());
    std::transform(
        coordsAbove_.begin(), coordsAbove_.end(), distances.begin(),
        [this](utilities::Vector position) {
            return distByVector(position, {0, -BLOCK_SIZE}).get_y();
        }
    );
    return *std::min_element(distances.begin(), distances.end());
}

int Player::distBelow() {
    auto coordsBelow_ = coordsBelow();
    std::vector<int> distances(coordsBelow_.size());
    std::transform(
        coordsBelow_.begin(), coordsBelow_.end(), distances.begin(),
        [this](utilities::Vector position) {
            return distByVector(position, {0, BLOCK_SIZE}).get_y();
        }
    );
    return *std::min_element(distances.begin(), distances.end());
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

bool Player::isHanging() {
    auto blocksBelow_ = blocksBelow();
    return std::any_of(
        blocksBelow_.begin(), blocksBelow_.end(),
        [](const Block *block) { return block->isHangableOn(); }
    );
}

bool Player::isStanding() {
    auto blocksBelow_ = blocksBelow();
    return std::any_of(
        blocksBelow_.begin(), blocksBelow_.end(),
        [](const Block *block) { return block->isSolid(); }
    );
}

bool Player::canJump() {
    auto blocksAbove_ = blocksAbove();
    return !std::any_of(
        blocksAbove_.begin(), blocksAbove_.end(),
        [](const Block *block) { return block->isSolid(); }
    );
}
}  // namespace Platformer
