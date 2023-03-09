#include <iostream>
#include "button.hpp"

namespace interface {
    [[maybe_unused]] void Button::setCurrentState(ButtonState newCurrentState) {
        currentState = newCurrentState;
    }
    [[maybe_unused]] ButtonState Button::getCurrentState() const {
        return currentState;
    }
    [[maybe_unused]] sf::Text Button::getLabel() const {
        return label;
    }

    [[maybe_unused]] void Button::setLabel(sf::Text newLabel) {
        label = std::move(newLabel);
    }

    [[maybe_unused]] sf::Vector2f Button::getSize() const {
        return size;
    }

    [[maybe_unused]] void Button::setSize(sf::Vector2f newSize) {
        size = newSize;
    }

    [[maybe_unused]] sf::Vector2f Button::getIndent() const {
        return indent;
    }

    [[maybe_unused]] void Button::setIndent(sf::Vector2f newIndent) {
        indent = newIndent;
    }
    [[maybe_unused]] sf::Vector2f Button::getPosition() const {
        return position;
    }

    [[maybe_unused]] void Button::setPosition(sf::Vector2f newPosition) {
        position = newPosition;
    }

    void Button::update(sf::RenderWindow& window, sf::Event& event) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f buttonLeftLowCorner = {this->position.x, this->position.y};
        sf::Vector2f buttonRightHighCorner = {this->position.x + this->label.getGlobalBounds().width + 2*this->indent.x,
                                              this->position.y + this->label.getGlobalBounds().height + 2*this->indent.y};
        bool mouseInsideButton = (static_cast<float>(mousePosition.x) >= buttonLeftLowCorner.x &&
                static_cast<float>(mousePosition.x) <= buttonRightHighCorner.x &&
                static_cast<float>(mousePosition.y) >= buttonLeftLowCorner.y &&
                static_cast<float>(mousePosition.y) <= buttonRightHighCorner.y);
        // TODO: move to .contains(sprite.getGlobalBounds()), much prettier
        if (event.type == sf::Event::MouseMoved) {
            if (mouseInsideButton && currentState == ButtonState::Available) {
                currentState = ButtonState::Chosen;
            }
        }
        // TODO: two types of button: execute on press & execute on release? Possible ButtonState rework?
        if (event.type == sf::Event::MouseButtonPressed) {
            if (mouseInsideButton && currentState == ButtonState::Chosen) {
                // std::cout << "Pressed & clicked\n";
                currentState = ButtonState::Clicked;
            }
            else {
                // std::cout << "Pressed & missed\n";
                currentState = ButtonState::Available;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            if (mouseInsideButton) {
                // std::cout << "Released button\n";
                currentState = ButtonState::Chosen;
            }
            else {
                // std::cout << "Released air\n";
                currentState = ButtonState::Available;
            }
        }
    }

    [[maybe_unused]] sf::RectangleShape *RectangleShapeButton::getCurrentShape() {
        switch(currentState) {
            case ButtonState::Unavailable: {
                return & shapeUnavailable;
            }
            case ButtonState::Available: {
                return & shapeAvailable;
            }
            case ButtonState::Chosen: {
                return & shapeChosen;
            }
            case ButtonState::Clicked: {
                return & shapeClicked;
            }
        }
        return nullptr;
    }

    [[maybe_unused]] [[maybe_unused]] sf::Sprite *SpriteButton::getCurrentSprite() {
        switch(currentState) {
            case ButtonState::Unavailable: {
                return & spriteUnavailable;
            }
            case ButtonState::Available: {
                return & spriteAvailable;
            }
            case ButtonState::Chosen: {
                return & spriteChosen;
            }
            case ButtonState::Clicked: {
                return & spriteClicked;
            }
        }
        return nullptr;
    }
}