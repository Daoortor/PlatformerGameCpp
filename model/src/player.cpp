#include "player.hpp"
#include <algorithm>

namespace Platformer {

namespace observers {
PhysicsObserver::PhysicsObserver(Player *player_) : player(player_) {
}

void PhysicsObserver::updateCollision() {
    if ((player->isStanding() && player->speed.y > 0)) {
        player->speed = {player->speed.x, 0};
    } else if (!player->canJump()) {
        int newSpeed = -static_cast<int>(
            static_cast<float>(player->speed.y) * BOUNCE_COEFFICIENT
        );
        player->speed = {player->speed.x, newSpeed};
    }
    if ((player->distLeft() == 0 && player->speed.x < 0) ||
        (player->distRight() == 0 && player->speed.x > 0)) {  // Hitting walls
        player->speed = {0, player->speed.y};
    }
}

void PhysicsObserver::updateGravity() {
    if (!player->isStanding() && !player->isHanging()) {
        player->speed = {
            player->speed.x, player->speed.y + FREEFALL_ACCELERATION};
    } else if (player->isHanging()) {
        player->speed = {player->speed.x, 0};
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

Player::Player(Game *game_, sf::Vector2i pos_) {
    game = game_;
    pos = pos_;
    auto physObs =
        std::unique_ptr<observers::Observer>(new observers::PhysicsObserver(this
        ));
    observerCollection = std::vector<std::unique_ptr<observers::Observer>>();
    observerCollection.push_back(std::move(physObs));
}

Player::Player(Game *game_, sf::Vector2i pos_, int width_, int height_)
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
        speed = {speed.x, -PLAYER_SPEED};
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

void Player::move(sf::Vector2i delta) {
    int deltaX, deltaY;
    if (delta.x > 0) {
        deltaX = std::min(delta.x, distRight());
    } else {
        deltaX = std::max(delta.x, -distLeft());
    }
    if (delta.y > 0) {
        deltaY = std::min(delta.y, distBelow());
    } else {
        deltaY = std::max(delta.y, -distAbove());
    }
    pos += {deltaX, deltaY};
}

void Player::notifyAll() {
    for (auto &obs : observerCollection) {
        obs->update();
    }
}

std::vector<sf::Vector2i> Player::coordsAbove() {
    std::vector<sf::Vector2i> result;
    sf::Vector2i currentVector{pos.x - width / 2 + 1, pos.y - height / 2 - 2};
    while (currentVector.x <= pos.x + width / 2 + 1) {
        result.push_back(currentVector);
        currentVector += {BLOCK_SIZE, 0};
    }
    sf::Vector2i upperRightCorner{
        pos.x + width / 2 - 1, pos.y - height / 2 - 2};
    result.push_back(upperRightCorner);
    return result;
}

std::vector<sf::Vector2i> Player::coordsBelow() {
    std::vector<sf::Vector2i> result;
    sf::Vector2i currentVector{pos.x - width / 2 + 1, pos.y + height / 2};
    while (currentVector.x <= pos.x + width / 2 + 1) {
        result.push_back(currentVector);
        currentVector += {BLOCK_SIZE, 0};
    }
    sf::Vector2i upperRightCorner{pos.x + width / 2 - 1, pos.y + height / 2};
    result.push_back(upperRightCorner);
    return result;
}

std::vector<const Block *> Player::blocksAbove() {
    auto coords = coordsAbove();
    std::vector<const Block *> result(coordsBelow().size());
    std::transform(
        coords.begin(), coords.end(), result.begin(),
        [this](sf::Vector2i position) {
            return game->getBoardObject()
                .getBlockByCoordinates(position - sf::Vector2i(0, 1))
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
        [this](sf::Vector2i position) {
            return game->getBoardObject().getBlockByCoordinates(position).get();
        }
    );
    return result;
}

sf::Vector2i
Player::distByVector(sf::Vector2i position, sf::Vector2i moveVector) {
    sf::Vector2i currentCordinates{
        utilities::divide(position.x, BLOCK_SIZE) * BLOCK_SIZE,
        utilities::divide(position.y, BLOCK_SIZE) * BLOCK_SIZE};
    sf::Vector2i padding;
    if (moveVector.x < 0) {
        padding = {1, 0};
    }
    if (moveVector.y < 0) {
        padding = {0, 1};
    }
    currentCordinates -= padding;
    for (int it = 0; it < MAX_FIELD_SIZE; it++) {
        if (game->getBoardObject()
                .getBlockByCoordinates(currentCordinates)
                ->isSolid()) {
            return sf::Vector2i(
                       std::abs(position.x - currentCordinates.x),
                       std::abs(position.y - currentCordinates.y)
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
        [this](sf::Vector2i position) {
            return distByVector(position, {0, -BLOCK_SIZE}).y;
        }
    );
    return *std::min_element(distances.begin(), distances.end());
}

int Player::distBelow() {
    auto coordsBelow_ = coordsBelow();
    std::vector<int> distances(coordsBelow_.size());
    std::transform(
        coordsBelow_.begin(), coordsBelow_.end(), distances.begin(),
        [this](sf::Vector2i position) {
            return distByVector(position, {0, BLOCK_SIZE}).y;
        }
    );
    return *std::min_element(distances.begin(), distances.end());
}

int Player::distLeft() {
    sf::Vector2i lowerLeft{pos.x - width / 2, pos.y + height / 2 - 1};
    sf::Vector2i middleLeft{pos.x - width / 2, pos.y};
    sf::Vector2i upperLeft{pos.x - width / 2, pos.y - height / 2 + 1};
    return std::min(
        {distByVector(lowerLeft, {-BLOCK_SIZE, 0}).x,
         distByVector(middleLeft, {-BLOCK_SIZE, 0}).x,
         distByVector(upperLeft, {-BLOCK_SIZE, 0}).x}
    );
}

int Player::distRight() {
    sf::Vector2i lowerRight{pos.x + width / 2, pos.y + height / 2 - 1};
    sf::Vector2i middleRight{pos.x + width / 2, pos.y};
    sf::Vector2i upperRight{pos.x + width / 2, pos.y - height / 2 + 1};
    return std::min(
        {distByVector(lowerRight, {BLOCK_SIZE, 0}).x,
         distByVector(middleRight, {BLOCK_SIZE, 0}).x,
         distByVector(upperRight, {BLOCK_SIZE, 0}).x}
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

bool Player::contains(sf::Vector2i position) const {
    int top = pos.y - height / 2;
    int bottom = pos.y + height / 2;
    int left = pos.x - width / 2;
    int right = pos.x + width / 2;
    return top <= position.y && position.y <= bottom && left <= position.x &&
           position.x <= right;
}
}  // namespace Platformer
