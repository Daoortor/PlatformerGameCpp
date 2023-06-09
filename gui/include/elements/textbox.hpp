#ifndef PLATFORMERGAMECPP_TEXTBOX_HPP_
#define PLATFORMERGAMECPP_TEXTBOX_HPP_

#include <SFML/Graphics.hpp>
#include <vector>

namespace interface {

enum class TextboxState { Unavailable, Available, Chosen, Active };

class Textbox {
    sf::Text text;
    sf::Text idleText;
    sf::Color colorUnavailable;
    sf::Color colorAvailable;
    sf::Color colorChosen;
    sf::Color colorActive;
    sf::RectangleShape shape;
    TextboxState currentState;
    std::size_t capacity;
    std::size_t cooldown;
    std::size_t ticksSinceLastErase = 0;
    void updateColor();

public:
    Textbox(
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
        std::size_t capacity_,
        std::size_t cooldown_ = 4
    );
    std::string getText();
    void setText(const std::string &text_);
    void loadInWindow(sf::RenderWindow &window, sf::Event &event);
    void update(sf::RenderWindow &window, sf::Event &event);
};

}  // namespace interface

#endif  // PLATFORMERGAMECPP_TEXTBOX_HPP_
