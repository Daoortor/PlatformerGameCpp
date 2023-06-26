#ifndef JSON_FILE_EXCHANGE_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SOURCE_HPP

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

// TODO: big TODO: naming
// TODO: enum class for actions - full implementation (how to use in message
// fields?)
namespace support {
class file_handling_exception : public std::runtime_error {
public:
    explicit file_handling_exception(const std::string &what_arg);
};

class no_such_file : public file_handling_exception {
public:
    explicit no_such_file(const std::string &file_name);
};

class file_already_exists : public file_handling_exception {
public:
    explicit file_already_exists();
};

class incomplete_level_file : public file_handling_exception {
public:
    explicit incomplete_level_file(const std::string &level_name);
};  // TODO: is needed at all? Of course, but how to implement

class leading_dots_in_file_name : public file_handling_exception {
public:
    explicit leading_dots_in_file_name(const std::string &level_name);
};

bool file_exists(const std::string &file_name);

void add_or_replace_json_file(
    const std::string &file_name,
    const std::string &file_content
);

std::string file_content_string(const std::string &file_name);

void delete_file(const std::string &file_name);
}  // namespace support

#endif  // JSON_FILE_EXCHANGE_SOURCE_HPP
