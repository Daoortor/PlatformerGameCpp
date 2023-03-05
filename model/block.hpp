#ifndef PLATFORMERGAMECPP_BLOCK_HPP_
#define PLATFORMERGAMECPP_BLOCK_HPP_

namespace Platformer {
class Block {
    bool isSolid_;
    bool isHangableOn_;

public:
    [[nodiscard]] bool isSolid() const {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const {
        return isHangableOn_;
    }
};
}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BLOCK_HPP_
