#ifndef JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP
#define JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP
namespace server {
    class level_handling_exception : public std::runtime_error {
    public:
        explicit level_handling_exception(const std::string &what_arg)
            : std::runtime_error("Error: " + what_arg + '\n'){};
    };

    class action_not_allowed : public level_handling_exception {
    public:
        explicit action_not_allowed(const std::string &file_name)
            : level_handling_exception("you don't have the right to edit or delete this file: " + file_name){};
    };
}

#endif//JSON_FILE_EXCHANGE_SERVER_SOURCE_HPP
