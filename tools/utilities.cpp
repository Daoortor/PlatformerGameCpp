#include "utilities.hpp"
#include <algorithm>
#include <filesystem>

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

std::string getExtension(const std::string &filename) {
    std::size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos);
}

std::string removeExtension(const std::string &filename) {
    return filename.substr(0, filename.size() - getExtension(filename).size());
}

std::vector<std::string> getLevelPaths(const std::string &levelFilePath) {
    std::vector<std::pair<std::string, std::string>> filenames;
    for (auto &fileIt : std::filesystem::directory_iterator(
             std::filesystem::path{levelFilePath}
         )) {
        const std::string levelPath = fileIt.path();
        std::string levelName = levelPath.substr(levelFilePath.size());
        if (Platformer::utilities::getExtension(levelName) != ".json") {
            continue;
        }
        filenames.emplace_back(
            Platformer::utilities::removeExtension(levelName), levelPath
        );
    }
    std::sort(filenames.begin(), filenames.end());
    std::vector<std::string> paths;
    paths.reserve(filenames.size());
    for (auto &filename : filenames) {
        paths.push_back(std::move(filename.second));
    }
    return paths;
}

std::vector<std::string> getLevelNames(const std::string &levelFilePath) {
    auto paths = getLevelPaths(levelFilePath);
    std::vector<std::string> result;
    result.reserve(paths.size());
    for (auto &path : paths) {
        result.push_back(
            removeExtension(std::move(path).substr(levelFilePath.size()))
        );
    }
    return result;
}
}  // namespace Platformer::utilities
