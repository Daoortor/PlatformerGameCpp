#ifndef JSON_FILE_EXCHANGE_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SOURCE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <random>

// TODO: big TODO: naming
// TODO: enum class for actions - full implementation (how to use in message fields?)
namespace support {
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

    class file_already_exists : public file_handling_exception {
    public:
        explicit file_already_exists()
            : file_handling_exception("file already exists.\n"){};
    };

    class incomplete_level_file : public file_handling_exception {
    public:
        explicit incomplete_level_file(const std::string &level_name)
            : file_handling_exception("missing crucial info for level file creation/overwrite: " + level_name){};
    }; // TODO: is needed at all? Of course, but how to implement

    class leading_dots_in_file_name : public file_handling_exception {
    public:
        explicit leading_dots_in_file_name(const std::string &level_name)
            : file_handling_exception("leading \"..\" in file name are not allowed: " + level_name){};
    };


    bool file_exists(const std::string &file_name) {
        return std::filesystem::exists(file_name);
    } // TODO: is it needed at all? Excessive dependency on this file by client_source.hpp because of it, and no code length advantages

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

    void delete_file(const std::string &file_name) {
        std::filesystem::remove(file_name);
    }
}

#endif//JSON_FILE_EXCHANGE_SOURCE_HPP
