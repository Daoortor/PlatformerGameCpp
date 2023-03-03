#ifndef PLATFORMERGAMECPP_PLAYER_HPP_
#define PLATFORMERGAMECPP_PLAYER_HPP_

#include "observers.hpp"
#include "block.hpp"
#include <vector>
#include <memory>

namespace Platformer {
    class Vector {
        int x = 0;
        int y = 0;
    public:
        Vector() = default;
        Vector(int x_, int y_);
        Vector operator+(Vector other) const;
        Vector operator-(Vector other) const;
        Vector operator+=(Vector other);
        Vector operator-=(Vector other);
        Vector operator*(float k) const;
        Vector operator*=(float k);
    };

    enum class Pose {
        LOOKING_LEFT, LOOKING_RIGHT
    };

    class Player {
        Vector pos;
        Vector speed;
        Vector acceleration;
        Pose pose = Pose::LOOKING_RIGHT;
        std::vector<std::unique_ptr<observers::Observer>> observerCollection;
    public:
        Player() = default;
        explicit Player(Vector pos_);
        void move(Vector delta);
        void notifyAll();
        Block *blockAbove();
        Block *blockBelow();
        Block *blockInside();
    };
}

#endif  // PLATFORMERGAMECPP_PLAYER_HPP_
