#include "../include/button.hpp"
#include <iostream>
#include <utility>

namespace interface {
[[maybe_unused]] void Button::setCurrentState(ButtonState newCurrentState) {
    currentState = newCurrentState;
}

[[maybe_unused]] [[maybe_unused]] ButtonState Button::getCurrentState() const {
    return currentState;
}

[[maybe_unused]] sf::Text Button::getLabel() const {
    return label;
}

[[maybe_unused]] void Button::setLabel(sf::Text newLabel) {
    label = std::move(newLabel);
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

void Button::bind(std::function<void()> func) {
    action = std::move(func);
}

void RectangleButton::update(sf::RenderWindow &window, sf::Event &event) {
    bool mouseInsideButton = this->shape.getGlobalBounds().contains(
        window.mapPixelToCoords(sf::Mouse::getPosition(window))
    );

    if (event.type == sf::Event::MouseMoved) {
        if (mouseInsideButton && currentState == ButtonState::Available) {
            currentState = ButtonState::Chosen;
        }
        if (!mouseInsideButton && currentState == ButtonState::Chosen) {
            currentState = ButtonState::Available;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        if (mouseInsideButton && currentState == ButtonState::Chosen) {
            currentState = ButtonState::Clicked;
            // TODO: rewrite next line?
            action();
        } else {
            currentState = ButtonState::Available;
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        if (mouseInsideButton) {
            currentState = ButtonState::Chosen;
        } else {
            currentState = ButtonState::Available;
        }
    }
}

[[maybe_unused]] sf::Color RectangleButton::getCurrentColor() {
    switch (currentState) {
        case ButtonState::Unavailable: {
            return colorUnavailable;
        }
        case ButtonState::Available: {
            return colorAvailable;
        }
        case ButtonState::Chosen: {
            return colorChosen;
        }
        case ButtonState::Clicked: {
            return colorClicked;
        }
    }
    return {255, 255, 255};
}

void RectangleButton::drawInWindow(sf::RenderWindow &window) {
    auto buttonShape = getCurrentShape();
    auto buttonColor = getCurrentColor();
    buttonShape->setPosition(getPosition());
    buttonShape->setFillColor(buttonColor);
    window.draw(*buttonShape);
    auto buttonText = getLabel();
    buttonText.setPosition(getPosition() + getIndent());
    window.draw(buttonText);
}

sf::RectangleShape *RectangleButton::getCurrentShape() {
    return &shape;
}

}  // namespace interface