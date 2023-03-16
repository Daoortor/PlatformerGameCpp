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

public:
    Block() = default;
    Block(std::string name, bool isSolid, bool isHangableOn)
        : name_(std::move(name)),
          isSolid_(isSolid),
          isHangableOn_(isHangableOn){};

    [[nodiscard]] std::string name() const {
        return name_;
    }

    [[nodiscard]] bool isSolid() const {
        return isSolid_;
    }

    [[nodiscard]] bool isHangableOn() const {
        return isHangableOn_;
    }
};

class Stone : public Block {
public:
    Stone() : Block("Stone", true, false) {
    }
};

class Air : public Block {
public:
    Air() : Block("Air", false, false) {
    }
};

class Ladder : public Block {
public:
    Ladder() : Block("Ladder", false, true) {
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
