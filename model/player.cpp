#include "player.hpp"
#include <cmath>

namespace Platformer {
Vector::Vector(int x_, int y_) : x(x_), y(y_) {
}

Vector Vector::operator+(Vector other) const {
    return {x + other.x, y + other.y};
}

Vector Vector::operator+=(Vector other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector Vector::operator-(Vector other) const {
    return {x - other.x, y - other.y};
}

Vector Vector::operator-=(Vector other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector Vector::operator*(float k) const {
    return {
        static_cast<int>(std::lrint(static_cast<float>(x) * k)),
        static_cast<int>(std::lrint(static_cast<float>(y) * k))};
}

Vector Vector::operator*=(float k) {
    x = static_cast<int>(std::lrint(static_cast<float>(x) * k));
    y = static_cast<int>(std::lrint(static_cast<float>(y) * k));
    return *this;
}

Player::Player(Game *game_) : game(game_) {
}

Player::Player(Game *game_, Vector pos_) : game(game_), pos(pos_) {
    auto phys_obs = observers::PhysicsObserver(this);
    observerCollection = std::vector<std::unique_ptr<observers::Observer>>();
    observerCollection.push_back(std::make_unique<observers::Observer>(phys_obs)
    );
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
    if (!blockLeft().isSolid()) {
        move({PLAYER_SPEED, 0});
    }
    pose = Pose::LOOKING_RIGHT;
}

void Player::jump() {
    if (blockBelow().isSolid() && !blockAbove().isSolid()) {
        speed = {speed.get_x(), -PLAYER_SPEED};
    } else if (blockInside().isHangableOn()) {
        move({0, -PLAYER_SPEED});
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
    return game->getField()[pos.get_x() / BLOCK_SIZE][pos.get_y() / BLOCK_SIZE];
}

Block Player::blockAbove() {
    return game->getField(
    )[pos.get_x() / BLOCK_SIZE][(pos.get_y() - height / 2) / BLOCK_SIZE];
}

Block Player::blockBelow() {
    return game->getField(
    )[pos.get_x() / BLOCK_SIZE][(pos.get_y() + height / 2) / BLOCK_SIZE];
}

Block Player::blockLeft() {
    return game->getField(
    )[(pos.get_x() - width / 2) / BLOCK_SIZE][pos.get_y() / BLOCK_SIZE];
}

Block Player::blockRight() {
    return game->getField(
    )[(pos.get_x() + width / 2) / BLOCK_SIZE][pos.get_y() / BLOCK_SIZE];
}

namespace observers {
PhysicsObserver::PhysicsObserver(Player *player_) : player(player_) {
}

void PhysicsObserver::updateCollision() {
    if (player->blockAbove().isSolid() &&
        player->speed.get_y() < 0) {  // Hitting ceiling
        player->speed = {player->speed.get_x(), -player->speed.get_y()};
    }
    if (player->blockLeft().isSolid() && player->speed.get_x() < 0 ||
        player->blockRight().isSolid() &&
            player->speed.get_x() > 0) {  // Hitting walls
        player->speed = {0, player->speed.get_y()};
    }
    if (player->blockBelow().isSolid() &&
        player->speed.get_y() > 0) {  // Hitting ground
        player->speed = {player->speed.get_x(), 0};
    }
}

void PhysicsObserver::updateGravity() {
    player->speed = {
        player->speed.get_x(), player->speed.get_y() + FREEFALL_ACCELERATION};
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
}  // namespace Platformer
