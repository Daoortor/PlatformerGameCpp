#ifndef PLATFORMERGAMECPP_BLOCK_HPP_
#define PLATFORMERGAMECPP_BLOCK_HPP_

#include <iostream>

namespace Platformer {
class Block {
    std::string name_;
    bool isSolid_;
    bool isHangableOn_;

public:
    Block() = default;

    [[nodiscard]] std::string name() {
        return name_;
    }

    [[nodiscard]] bool isSolid() const {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const {
        return isHangableOn_;
    }

    void operator+=(Block T);
};

class Stone : public Block {
    const std::string name_ = "stone";
    const bool isSolid_ = true;
    const bool isHangableOn_ = false;

public:
    Stone() = default;
};

class Air : public Block {
    const std::string name_ = "air";
    const bool isSolid_ = false;
    const bool isHangableOn_ = false;

public:
    Air() = default;
};


class Ladder : public Block {
    const std::string name_ = "ladder";
    const bool isSolid_ = false;
    const bool isHangableOn_ = true;

public:
    Ladder() = default;
};


}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BLOCK_HPP_
