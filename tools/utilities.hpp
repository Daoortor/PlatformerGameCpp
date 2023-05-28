#ifndef PLATFORMERGAMECPP_UTILITIES_HPP_
#define PLATFORMERGAMECPP_UTILITIES_HPP_

#include <ostream>
#include <string>
#include <vector>

namespace Platformer::utilities {

int divide(int a, unsigned int b);

int sign(int n);

std::string getExtension(const std::string &filename);

std::string removeExtension(const std::string &filename);

std::vector<std::string> getLevelPaths(const std::string &levelFilePath);

std::vector<std::string> getLevelNames(const std::string &levelFilePath);
}  // namespace Platformer::utilities

#endif  // PLATFORMERGAMECPP_UTILITIES_HPP_
