#include "button.hpp"
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

ButtonWithImage::ButtonWithImage(
    const std::string &imageFilename,
    sf::RectangleShape newShape,
    sf::Vector2f newIndent,
    sf::Vector2f newPosition,
    const std::vector<sf::Color> &buttonColorsList,
    std::function<void()> newAction,
    bool hasBorder,
    float borderMargin_,
    sf::Color colorActive_
)
    : RectangleButton(
          std::move(newShape),
          buttonColorsList[0],
          buttonColorsList[1],
          buttonColorsList[2],
          buttonColorsList[3],
          sf::Text(),
          newIndent,
          newPosition,
          std::move(newAction)
      ),
      borderMargin(borderMargin_),
      colorActive(colorActive_) {
    if (hasBorder) {
        border = sf::RectangleShape(
            shape.getSize() + 2.f * sf::Vector2f(borderMargin, borderMargin)
        );
        border.setPosition(
            newPosition - sf::Vector2f(borderMargin, borderMargin)
        );
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.f);
        border.setOutlineColor(sf::Color(255, 255, 255));
    }
    imageTexture.loadFromFile(imageFilename);
    float imageScaleX =
        static_cast<float>(shape.getSize().x - 2 * newIndent.x) /
        static_cast<float>(imageTexture.getSize().x);
    float imageScaleY =
        static_cast<float>(shape.getSize().y - 2 * newIndent.y) /
        static_cast<float>(imageTexture.getSize().y);
    float imageScale = std::min(imageScaleX, imageScaleY);
    imageSprite.setTexture(imageTexture);
    imageSprite.setScale(imageScale, imageScale);
    imageSprite.setPosition(newPosition + newIndent);
}

void ButtonWithImage::drawInWindow(sf::RenderWindow &window) {
    RectangleButton::drawInWindow(window);
    imageSprite.setTexture(imageTexture);
    window.draw(border);
    window.draw(imageSprite);
}

void ButtonWithImage::setPosition(sf::Vector2f newPosition) {
    Button::setPosition(newPosition);
    imageSprite.setPosition(newPosition);
    border.setPosition(newPosition - sf::Vector2f(borderMargin, borderMargin));
}

void ButtonWithImage::setBackgroundColor(sf::Color color) {
    border.setFillColor(color);
}

void ButtonWithImage::activate() {
    setBackgroundColor(colorActive);
}

void ButtonWithImage::deactivate() {
    setBackgroundColor(sf::Color::Transparent);
}
}  // namespace interface
