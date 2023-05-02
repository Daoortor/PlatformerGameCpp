#ifndef PLATFORMERGAMECPP_SCROLLBAR_HPP
#define PLATFORMERGAMECPP_SCROLLBAR_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "button.hpp"

namespace interface {
void moveDrawing(std::vector<sf::VertexArray> &drawing, sf::Vector2f delta);

class Scrollbar {
protected:
    sf::Vector2f position;
    std::vector<std::unique_ptr<ButtonWithImage>> items;
    float moveButtonsHeight;
    std::size_t capacity;
    std::size_t offset = 0;
    ButtonWithImage upButton;
    ButtonWithImage downButton;
    sf::Vector2f itemSize;
    sf::Vector2f edgeMargin;
    float itemMargin;
    std::vector<std::vector<sf::VertexArray>> outline;

public:
    Scrollbar(
        sf::Vector2f position_,
        sf::Vector2f itemSize_,
        sf::Vector2f edgeMargin_,
        float itemMargin_,
        float moveButtonsHeight_,
        std::size_t capacity_,
        const std::vector<sf::Color> &buttonColorsList,
        const std::string &iconFilepath
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void addItem(ButtonWithImage &item);
    void scrollUp();
    void scrollDown();
};
}  // namespace interface

#endif  // PLATFORMERGAMECPP_SCROLLBAR_HPP