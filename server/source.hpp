#ifndef JSON_FILE_EXCHANGE_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SOURCE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <random>
#include <iostream>

// TODO: big TODO: naming
// TODO: enum class for actions - full implementation (how to use in message fields?)

class file_handling_exception : public std::runtime_error {
public:
    explicit file_handling_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg + '\n'){};
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

class level_exception : public std::runtime_error {
public:
    explicit level_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg + '\n'){};
};

class action_not_allowed : public level_exception {
public:
    explicit action_not_allowed(const std::string &file_name)
        : level_exception("you don't have the right to edit or delete this file: " + file_name){};
};

class file_already_exists : public level_exception {
public:
    explicit file_already_exists()
        : level_exception("file already exists.\n"){};
};

// TODO: move server errors & methods to separate file
// TODO: check error messages \n at the end of line usage

bool file_exists(const std::string &file_name) {
    return std::filesystem::exists(file_name);
}

void add_or_replace_json_file(const std::string &file_name, const std::string &file_content) {
    std::ofstream file(file_name, std::ios::trunc);// TODO: file prefix move here?
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

const int key_size = 100; // it's not actually key size! TODO: redo
const std::string key_file_name = "key";
void create_key_file(const std::string & directory) {
    std::ofstream file(directory+key_file_name, std::ios::trunc);
    for(size_t i = 0; i < key_size; ++i) {
        file << random();
    }
    file.close();
}

void validate_key_file_existence(const std::string & directory) {
    if (!file_exists(directory+key_file_name)) {
        std::cout << "No key file, creating new\n";
        create_key_file(directory);
    }
}

int32_t get_key(const std::string & directory) {
    std::ifstream file(directory+key_file_name);
    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    std::hash<std::string> hasher;
    return static_cast<int32_t>(hasher(content));
}

#endif//JSON_FILE_EXCHANGE_SOURCE_HPP
