#ifndef PROJECTGAME_BUTTON_HPP
#define PROJECTGAME_BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <utility>

namespace interface {
enum class ButtonState { Unavailable, Available, Chosen, Clicked };

class Button {
protected:
    ButtonState currentState;
    sf::Text label;

    sf::Vector2f indent;  // for readability purposes probably should change it
                          // to a pair of ints
    sf::Vector2f position;

    std::function<void()> action;
public:
    Button() {
        currentState = ButtonState::Available;
    }

    [[maybe_unused]] void setCurrentState(ButtonState newCurrentState);

    [[maybe_unused]] ButtonState getCurrentState() const;

    sf::Text getLabel() const;

    [[maybe_unused]] void setLabel(sf::Text newLabel);

    [[maybe_unused]] sf::Vector2f getIndent() const;

    [[maybe_unused]] void setIndent(sf::Vector2f newIndent);
    sf::Vector2f getPosition() const;

    [[maybe_unused]] void setPosition(sf::Vector2f newPosition);
};

class RectangleButton : public Button {
private:
    sf::RectangleShape shape;
    sf::Color colorUnavailable;  // like if there are no saved games,
                                          // you can't click "Load game"
    sf::Color colorAvailable;
    sf::Color colorChosen;  // dunno about naming; for situations when
                                     // your cursor is hovering above it,
    // or you chose it via arrows/keyboard, but did not activate
    sf::Color colorClicked;

public:
    RectangleButton(
        sf::RectangleShape newShape,
        sf::Color newColorUnavailable,
        sf::Color newColorAvailable,
        sf::Color newColorChosen,
        sf::Color newColorClicked,
        sf::Text newLabel,
        sf::Vector2f newIndent,
        sf::Vector2f newPosition,
        std::function<void()> newAction
    )
        : shape(std::move(newShape)),
          colorUnavailable(newColorUnavailable),
          colorAvailable(newColorAvailable),
          colorChosen(newColorChosen),
          colorClicked(newColorClicked) {
        label = std::move(newLabel);
        indent = newIndent;
        position = newPosition;
        currentState = ButtonState::Available;
        action = std::move(newAction);
    }

    sf::RectangleShape * getCurrentShape();
    sf::Color getCurrentColor();
    void drawInWindow(sf::RenderWindow & window);
    void update(sf::RenderWindow &window, sf::Event &event);
};

}  // namespace interface
#endif  // PROJECTGAME_BUTTON_HPP
