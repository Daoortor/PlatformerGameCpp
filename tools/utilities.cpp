#include "utilities.hpp"
#include <cmath>

namespace Platformer::utilities {

int divide(int a, unsigned int b) {  // Always rounded down
    if (a < 0 && a % b != 0) {
        return a / static_cast<int>(b) - 1;
    }
    return a / static_cast<int>(b);
}

int sign(int n) {
    if (n > 0) {
        return 1;
    } else if (n == 0) {
        return 0;
    }
    return -1;
}
}  // namespace Platformer::utilities
