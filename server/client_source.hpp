#ifndef JSON_FILE_EXCHANGE_CLIENT_SOURCE_HPP
#define JSON_FILE_EXCHANGE_CLIENT_SOURCE_HPP

const int key_size = 100; // it's not actually key size! TODO: redo!
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

#endif//JSON_FILE_EXCHANGE_CLIENT_SOURCE_HPP
