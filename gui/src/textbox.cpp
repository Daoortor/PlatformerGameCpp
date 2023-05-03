#include "textbox.hpp"
#include <utility>

namespace interface {
Textbox::Textbox(
    sf::RectangleShape shape_,
    const std::string &defaultText,
    const sf::Font &font,
    std::size_t textSize,
    sf::Color idleTextColor,
    sf::Color typedTextColor,
    sf::Color colorUnavailable_,
    sf::Color colorAvailable_,
    sf::Color colorChosen_,
    sf::Color colorActive_,
    sf::Vector2f position,
    sf::Vector2f indent,
    std::size_t capacity_
)
    : shape(std::move(shape_)),
      colorUnavailable(colorUnavailable_),
      colorAvailable(colorAvailable_),
      colorChosen(colorChosen_),
      colorActive(colorActive_),
      currentState(TextboxState::Available),
      capacity(capacity_) {
    shape.setPosition(position);
    idleText = sf::Text(defaultText, font, textSize);
    idleText.setFillColor(idleTextColor);
    idleText.setPosition(position + indent);
    text = sf::Text("", font, textSize);
    text.setFillColor(typedTextColor);
    text.setPosition(position + indent);
}

void Textbox::update(sf::RenderWindow &window, sf::Event &event) {
    bool mouseInsideTextbox = this->shape.getGlobalBounds().contains(
        window.mapPixelToCoords(sf::Mouse::getPosition(window))
    );

    if (event.type == sf::Event::MouseMoved) {
        if (mouseInsideTextbox && currentState == TextboxState::Available) {
            currentState = TextboxState::Chosen;
        }
        if (!mouseInsideTextbox && currentState == TextboxState::Chosen) {
            currentState = TextboxState::Available;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (mouseInsideTextbox && currentState == TextboxState::Chosen) {
            currentState = TextboxState::Active;
        } else {
            currentState = TextboxState::Available;
        }
    }
    if (event.type == sf::Event::TextEntered &&
        currentState == TextboxState::Active &&
        text.getString().getSize() < capacity) {
        if (event.text.unicode >= 32 && event.text.unicode < 128) {
            text.setString(
                text.getString() + static_cast<char>(event.text.unicode)
            );
        }
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace) &&
               currentState == TextboxState::Active &&
               !text.getString().isEmpty()) {
        text.setString(
            text.getString().substring(0, text.getString().getSize() - 1)
        );
    }
    updateColor();
}

void Textbox::updateColor() {
    switch (currentState) {
        case TextboxState::Unavailable:
            shape.setFillColor(colorUnavailable);
            break;
        case TextboxState::Available:
            shape.setFillColor(colorAvailable);
            break;
        case TextboxState::Chosen:
            shape.setFillColor(colorChosen);
            break;
        case TextboxState::Active:
            shape.setFillColor(colorActive);
    }
}

void Textbox::loadInWindow(sf::RenderWindow &window, sf::Event &event) {
    update(window, event);
    window.draw(shape);
    if (text.getString().isEmpty() && currentState != TextboxState::Active) {
        window.draw(idleText);
    } else {
        window.draw(text);
    }
}

std::string Textbox::getText() {
    return text.getString();
}
}  // namespace interface
