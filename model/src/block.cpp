#include "block.hpp"
#include <memory>

namespace Platformer {
[[nodiscard]] const char *UnknownBlockType::what() const noexcept {
    return message.c_str();
}

std::unique_ptr<Block> makeBlock(const std::string &name) {
    if (name == "air") {
        return std::make_unique<Air>();
    } else if (name == "stone") {
        return std::make_unique<Stone>();
    } else if (name == "ladder") {
        return std::make_unique<Ladder>();
    } else {
        throw UnknownBlockType(name);
    }
};
}  // namespace Platformer
