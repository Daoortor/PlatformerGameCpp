#ifndef JSON_FILE_EXCHANGE_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SOURCE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <random>

// TODO: big TODO: naming
// TODO: enum class for actions - full implementation (how to use in message fields?)

enum class Action {
    Add, Get, Delete, Check
};

class file_handling_exception : public std::runtime_error {
public:
    explicit file_handling_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg){};
};

class no_such_file : public file_handling_exception {
public:
    explicit no_such_file(const std::string &file_name)
        : file_handling_exception("no such file: " + file_name){};
};

class incomplete_level_file : public file_handling_exception {
public:
    explicit incomplete_level_file(const std::string &level_name)
        : file_handling_exception("missing crucial info for level file creation/overwrite: " + level_name){};
}; // TODO: is needed at all?

bool file_exists(const std::string &file_name) {
    return std::filesystem::exists(file_name);
}

void add_or_replace_json_file(const std::string &file_name, const std::string &file_content) {
    std::ofstream file(file_name);// TODO: file prefix move here?
    file << file_content;         // TODO: validity check?
    file.close();
}

std::string file_content_string(const std::string &file_name) {
    std::ifstream file(file_name);
    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    return std::move(content);
} // TODO: effective reading
// TODO: error handling

void delete_file(const std::string &file_name) {
    std::filesystem::remove(file_name);
}

int get_unique_key() {
    return static_cast<int>(random()); // TODO: actual generation
}

#endif//JSON_FILE_EXCHANGE_SOURCE_HPP
