#include "player.hpp"
#include <cmath>

namespace Platformer {
    Vector::Vector(int x_, int y_) : x(x_), y(y_){};
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
        return {static_cast<int>(std::lrint(static_cast<float>(x) * k)),
                static_cast<int>(std::lrint(static_cast<float>(y) * k))};
    }
    Vector Vector::operator*=(float k) {
        x = static_cast<int>(std::lrint(static_cast<float>(x) * k));
        y = static_cast<int>(std::lrint(static_cast<float>(y) * k));
        return *this;
    }

    Player::Player(Vector pos_) : pos(pos_) {
        auto phys_obs = observers::PhysicsObserver(this);
        auto pose_obs = observers::PoseObserver(this);
        observerCollection = std::vector<std::unique_ptr<observers::Observer>>();
        observerCollection.push_back(std::make_unique<observers::Observer>(phys_obs));
        observerCollection.push_back(std::make_unique<observers::Observer>(pose_obs));
    }

    void Player::move(Vector delta) {
        pos += delta;
    }

    void Player::notifyAll() {
        for (auto &obs : observerCollection) {
            obs->update();
        }
    }
}
