#include "utilities.hpp"
#include <cmath>

namespace Platformer::utilities {
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

std::ostream &operator<<(std::ostream &os, const Vector &vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

int divide(int a, unsigned int b) {  // Always rounded down
    if (a < 0 && a % b != 0) {
        return a / static_cast<int>(b) - 1;
    }
    return a / static_cast<int>(b);
}
}  // namespace Platformer::utilities
