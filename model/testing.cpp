#include <iostream>
#include "../tools/json.hpp"
#include "include/game.hpp"
#include "include/player.hpp"

using json = nlohmann::json;

int main() {
    Platformer::Game game("model/levels/t01-box-with-ladder.json");
    for (int i = 0; i < 5; i++) {
        game.getPlayer()->act('D');
        game.update();
        std::cout << game.getPlayer()->getPos() << std::endl;
    }
    std::cout << std::endl;
    game.getPlayer()->act('W');
    game.update();
    for (int i = 0; i < 20; i++) {
        game.getPlayer()->act('D');
        game.update();
        std::cout << game.getPlayer()->getPos() << std::endl;
    }
    std::cout << std::endl;
    for (int i = 0; i < 25; i++) {
        game.getPlayer()->act('A');
        game.update();
        std::cout << game.getPlayer()->getPos() << std::endl;
    }
    std::cout << std::endl;
    for (int i = 0; i < 35; i++) {
        game.getPlayer()->act('W');
        game.update();
        std::cout << game.getPlayer()->getPos() << std::endl;
    }
    std::cout << std::endl;
    for (int i = 0; i < 20; i++) {
        game.getPlayer()->act('A');
        game.update();
        std::cout << game.getPlayer()->getPos() << std::endl;
    }

    return 0;
}
