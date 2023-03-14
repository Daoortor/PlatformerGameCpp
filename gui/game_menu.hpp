#ifndef PROJECTGAME_GAME_MENU_HPP
#define PROJECTGAME_GAME_MENU_HPP

#include "button.hpp"
#include <map>
#include <utility>
#include <iostream>

namespace interface {
    struct Menu { // simple litter class to debug & showcase Button class usage
    private:
        std::vector<std::pair<RectangleShapeButton, std::function<void()>>> buttonsActionsList;
    public:
        Menu() = default;
        void addButton(const RectangleShapeButton& button, const std::function<void()>& buttonAction) {
            buttonsActionsList.emplace_back(button, buttonAction);
        }
        void runButtons() {
            for (auto & pair : buttonsActionsList) {
                if (pair.first.getCurrentState() == ButtonState::Clicked) {
                    pair.second();
                    pair.first.setCurrentState(ButtonState::Chosen); // TODO: find better way
                }
            }
        }
        void drawMenu(sf::RenderWindow& window) {
            for (auto & pair: buttonsActionsList) {
                auto button = pair.first;
                auto buttonShape = *button.getCurrentShape();
                buttonShape.setPosition(button.getPosition());
                window.draw(buttonShape);
                auto buttonText = button.getLabel();
                buttonText.setPosition(button.getPosition() + button.getIndent());
                window.draw(buttonText);
            }
        }
        void updateButtons(sf::RenderWindow& window, sf::Event event) {
            for (auto & pair: buttonsActionsList) {
                pair.first.update(window, event);
            }
        }
    };

}

#endif //PROJECTGAME_GAME_MENU_HPP
