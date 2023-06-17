#ifndef JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP

namespace server {
const int32_t AdminID = 1443241;

class level_handling_exception : public std::runtime_error {
public:
    explicit level_handling_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg + '\n'){};
};

class action_not_allowed : public level_handling_exception {
public:
    explicit action_not_allowed(const std::string &file_name)
        : level_handling_exception(
              "you don't have the right to edit or delete this file: " +
              file_name
          ){};
};

std::string get_file_extension(const std::string &filename) {
    std::size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos);
}  // TODO: stolen from utilities.cpp as I want to store server separately
}  // namespace server

#endif  // JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP
// TODO: even if it is included in 1 executable, it doesn't make it right to
// write definitions in .hpp!