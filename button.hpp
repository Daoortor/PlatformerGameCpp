#ifndef PROJECTGAME_BUTTON_HPP
#define PROJECTGAME_BUTTON_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <utility>

namespace interface {
    enum class ButtonState {
        Unavailable, Available, Chosen, Clicked
    };
    class Button { // should probably derive TextButton and the like from this one;
        // also this one can probably be derived from block, but shaky about this
    protected:
        ButtonState currentState;
        sf::Text label;
        sf::Vector2f size; // TODO: resolve label dependency; rn size does nothing - but it should
        sf::Vector2f indent; // for readability purposes probably should change it to a pair of ints
        sf::Vector2f position;
    public:
        // TODO: base constructor!
        Button() {
            currentState = ButtonState::Available;
        }

        [[maybe_unused]] void setCurrentState(ButtonState newCurrentState);
        ButtonState getCurrentState() const;
        sf::Text getLabel() const;

        [[maybe_unused]] void setLabel(sf::Text newLabel);

        [[maybe_unused]] sf::Vector2f getSize() const;

        [[maybe_unused]] void setSize(sf::Vector2f newSize);

        [[maybe_unused]] sf::Vector2f getIndent() const;

        [[maybe_unused]] void setIndent(sf::Vector2f newIndent);
        sf::Vector2f getPosition() const;

        [[maybe_unused]] void setPosition(sf::Vector2f newPosition);

        void update(sf::RenderWindow& window, sf::Event& event);
    };
    class RectangleShapeButton : public Button {
    private:
        sf::RectangleShape shapeUnavailable; // like if there are no saved games, you can't click "Load game"
        sf::RectangleShape shapeAvailable;
        sf::RectangleShape shapeChosen; // dunno about naming; for situations when your cursor is hovering above it
        // or you chose it via arrows/keyboard, but did not activate
        sf::RectangleShape shapeClicked;
    public:
        RectangleShapeButton(sf::RectangleShape & newShapeUnavailable,
        sf::RectangleShape & newShapeAvailable,
        sf::RectangleShape & newShapeChosen,
        sf::RectangleShape & newShapeClicked,
                sf::Text newLabel,
                sf::Vector2f newSize,
                sf::Vector2f newIndent,
                sf::Vector2f newPosition) :
                shapeUnavailable(newShapeUnavailable), shapeAvailable(newShapeAvailable),
                shapeChosen(newShapeChosen),
                shapeClicked(newShapeClicked) {
            label = std::move(newLabel);
            size = newSize;
            indent = newIndent;
            position = newPosition;
            currentState = ButtonState::Available;
        }
        sf::RectangleShape * getCurrentShape();
    };
    class [[maybe_unused]] SpriteButton : Button {
    private:
        sf::Sprite spriteUnavailable; // like if there are no saved games, you can't click "Load game"
        sf::Sprite spriteAvailable;
        sf::Sprite spriteChosen; // dunno about naming; for situations when your cursor is hovering above it
        // or you chose it via arrows/keyboard, but did not activate
        sf::Sprite spriteClicked; // TODO: right now spriteClicked never appears because of a need to
                                // disable ButtonState::Clicked right away to avoid multiple button executions by 1 click
    public:
        [[maybe_unused]] SpriteButton(sf::Sprite & newSpriteUnavailable,
                sf::Sprite & newSpriteAvailable,
                sf::Sprite & newSpriteClicked,
                sf::Sprite & newSpriteChosen,
                sf::Text newLabel,
                sf::Vector2f newSize,
                sf::Vector2f newIndent,
                sf::Vector2f newPosition) :
        spriteUnavailable(newSpriteUnavailable),
        spriteAvailable(newSpriteAvailable),
        spriteChosen(newSpriteChosen),
        spriteClicked(newSpriteClicked) {
            label = std::move(newLabel);
            size = newSize;
            indent = newIndent;
            position = newPosition;
            currentState = ButtonState::Available;
        }

        [[maybe_unused]] sf::Sprite * getCurrentSprite();
    };
}
#endif //PROJECTGAME_BUTTON_HPP
