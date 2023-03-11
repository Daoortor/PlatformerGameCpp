#ifndef PLATFORMERGAMECPP_BLOCK_HPP_
#define PLATFORMERGAMECPP_BLOCK_HPP_

#include <string>

namespace Platformer {
// Полиморфизм работает с std::unique_ptr.
// Можно переделать без использования наследования.
class Block {
protected:
    const std::string name_;
    const bool isSolid_ = false;
    const bool isHangableOn_ = false;

public:
    Block() = default;

    [[nodiscard]] virtual std::string name() const {
        return name_;
    }

    [[nodiscard]] virtual bool isSolid() const {
        return isSolid_;
    }

    [[nodiscard]] virtual bool isHangableOn() const {
        return isHangableOn_;
    }
};

class Stone : public Block {
protected:
    const std::string name_ = "stone";
    const bool isSolid_ = true;
    const bool isHangableOn_ = false;

public:
    Stone() = default;

    [[nodiscard]] std::string name() const final {
        return name_;
    }

    [[nodiscard]] bool isSolid() const final {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const final {
        return isHangableOn_;
    }
};

class Air : public Block {
protected:
    const std::string name_ = "air";
    const bool isSolid_ = false;
    const bool isHangableOn_ = false;

public:
    Air() = default;

    [[nodiscard]] std::string name() const final {
        return name_;
    }

    [[nodiscard]] bool isSolid() const final {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const final {
        return isHangableOn_;
    }
};

class Ladder : public Block {
protected:
    const std::string name_ = "ladder";
    const bool isSolid_ = false;
    const bool isHangableOn_ = true;

public:
    Ladder() = default;

    [[nodiscard]] std::string name() const final {
        return name_;
    }

    [[nodiscard]] bool isSolid() const final {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const final {
        return isHangableOn_;
    }
};

}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BLOCK_HPP_
