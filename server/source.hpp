#ifndef JSON_FILE_EXCHANGE_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SOURCE_HPP

#include <string>
#include <filesystem>
#include <fstream>

// TODO: naming
// TODO: enum class for actions: add, get, delete, check, ...

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

void delete_file(const std::string &file_name) {
    std::filesystem::remove(file_name);
}

#endif//JSON_FILE_EXCHANGE_SOURCE_HPP
