#ifndef PLATFORMERGAMECPP_UTILITIES_HPP_
#define PLATFORMERGAMECPP_UTILITIES_HPP_

#include <ostream>

namespace Platformer::utilities {
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

        [[nodiscard]] int get_x() const {
            return x;
        }

        [[nodiscard]] int get_y() const {
            return y;
        }

        friend std::ostream &operator<<(std::ostream &os, const utilities::Vector &vec);
    };

    int divide(int a, unsigned int b);

}  // namespace Platformer::utilities


#endif  // PLATFORMERGAMECPP_UTILITIES_HPP_