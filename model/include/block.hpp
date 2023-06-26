#ifndef PLATFORMERGAMECPP_BLOCK_HPP_
#define PLATFORMERGAMECPP_BLOCK_HPP_

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace Platformer {
// Полиморфизм работает с std::unique_ptr.
// Можно переделать без использования наследования.
class Block {
protected:
    const std::string name_;
    const bool isSolid_ = false;
    const bool isHangableOn_ = false;
    const bool isDeadly_ = false;
    const float fallBounceCoefficient_ = 0;

public:
    Block() = default;
    explicit Block(
        std::string name,
        bool isSolid = false,
        bool isHangableOn = false,
        bool isDeadly = false,
        float fallBounceCoefficient = 0
    )
        : name_(std::move(name)),
          isSolid_(isSolid),
          isHangableOn_(isHangableOn),
          isDeadly_(isDeadly),
          fallBounceCoefficient_(fallBounceCoefficient){};

    [[nodiscard]] std::string name() const {
        return name_;
    }

    [[nodiscard]] bool isSolid() const {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const {
        return isHangableOn_;
    }

    [[nodiscard]] bool isDeadly() const {
        return isDeadly_;
    }

    [[nodiscard]] float getFallBounceCoefficient() const {
        return fallBounceCoefficient_;
    }
};

class Stone : public Block {
public:
    Stone() : Block("stone", true) {
    }
};

class Air : public Block {
public:
    Air() : Block("air") {
    }
};

class Ladder : public Block {
public:
    Ladder() : Block("ladder", false, true) {
    }
};

class Killer : public Block {
public:
    Killer() : Block("killer", true, false, true) {
    }
};

class Trampoline : public Block {
public:
    Trampoline() : Block("trampoline", true, false, false, 0.9) {
    }
};

class UnknownBlockType : public std::exception {
    std::string message;

public:
    explicit UnknownBlockType(std::string blockName)
        : message("Unknown block type: " + std::move(blockName)){};
    [[nodiscard]] const char *what() const noexcept override;
};

const auto AIR_BLOCK = std::make_unique<Block>(Air());

std::unique_ptr<Block> makeBlock(const std::string &name);

}  // namespace Platformer

#endif  // PLATFORMERGAMECPP_BLOCK_HPP_
