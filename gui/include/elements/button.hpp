#ifndef PROJECTGAME_BUTTON_HPP
#define PROJECTGAME_BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <climits>
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

    [[maybe_unused]] virtual void setPosition(sf::Vector2f newPosition);

    void bind(std::function<void()> func);

    virtual void act();
};

class RectangleButton : public Button {
protected:
    sf::RectangleShape shape;
    sf::Color colorUnavailable;
    sf::Color colorAvailable;
    sf::Color colorChosen;
    sf::Color colorClicked;
    int capacity;

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
        std::function<void()> newAction = []() {},
        int capacity_ = INT32_MAX
    )
        : shape(std::move(newShape)),
          colorUnavailable(newColorUnavailable),
          colorAvailable(newColorAvailable),
          colorChosen(newColorChosen),
          colorClicked(newColorClicked),
          capacity(capacity_) {
        label = std::move(newLabel);
        if (label.getString().getSize() > capacity - 3) {
            label.setString(
                label.getString().substring(0, capacity - 3) + "..."
            );
        }
        indent = newIndent;
        position = newPosition;
        currentState = ButtonState::Available;
        action = std::move(newAction);
    }

    sf::RectangleShape *getCurrentShape();
    sf::Color getCurrentColor();
    virtual void drawInWindow(sf::RenderWindow &window);
    void update(sf::RenderWindow &window, sf::Event &event);
};

class SwitchRectangleButton : public RectangleButton {
private:
    void switchColors();
public:
    SwitchRectangleButton(
        sf::RectangleShape newShape,
        sf::Color newColorUnavailable,
        sf::Color newColorAvailable,
        sf::Color newColorChosen,
        sf::Color newColorClicked,
        sf::Text newLabel,
        sf::Vector2f newIndent,
        sf::Vector2f newPosition,
        std::function<void()> newAction = []() {},
        int capacity_ = INT32_MAX
    )
        : RectangleButton(
              std::move(newShape),
              newColorUnavailable,
              newColorAvailable,
              newColorChosen,
              newColorClicked,
              std::move(newLabel),
              newIndent,
              newPosition,
              std::move(newAction),
              capacity_
              ) {}

    void act() override;
};


class ButtonWithImage : public RectangleButton {
private:
    sf::Sprite imageSprite;
    sf::Texture imageTexture;
    sf::RectangleShape border;
    float borderMargin;
    sf::Color colorActive;

public:
    ButtonWithImage(
        const std::string &imageFilename,
        sf::RectangleShape newShape,
        sf::Vector2f newIndent,
        sf::Vector2f newPosition,
        const std::vector<sf::Color> &buttonColorsList,
        std::function<void()> newAction = []() {},
        bool hasBorder = true,
        float borderMargin = 0,
        sf::Color colorActive_ = sf::Color::Transparent
    );
    void drawInWindow(sf::RenderWindow &window) override;
    void setPosition(sf::Vector2f newPosition) override;
    void setBackgroundColor(sf::Color color);
    void activate();
    void deactivate();
};

}  // namespace interface
#endif  // PROJECTGAME_BUTTON_HPP
