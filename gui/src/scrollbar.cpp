#include "../include/scrollbar.hpp"
#include <iostream>
#include "../include/gui-constants.hpp"

namespace interface {
Scrollbar::Scrollbar(
    sf::Vector2f position_,
    sf::Vector2f itemSize_,
    sf::Vector2f edgeMargin_,
    float itemMargin_,
    float moveButtonsHeight_,
    std::size_t capacity_,
    const std::vector<sf::Color> &buttonColorsList,
    sf::Color itemChosenColor_,
    const std::string &iconFilepath
)
    : position(position_),
      capacity(capacity_),
      itemSize(itemSize_),
      edgeMargin(edgeMargin_),
      itemMargin(itemMargin_),
      moveButtonsHeight(moveButtonsHeight_),
      itemChosenColor(itemChosenColor_),
      upButton(
          iconFilepath + "up-arrow.png",
          sf::RectangleShape(
              {itemSize_.x + 2 * edgeMargin_.x, moveButtonsHeight_}
          ),
          {0, 0},
          position,
          buttonColorsList,
          [this]() { this->scrollUp(); }
      ),
      downButton(
          iconFilepath + "down-arrow.png",
          sf::RectangleShape(
              {itemSize_.x + 2 * edgeMargin_.x, moveButtonsHeight_}
          ),
          {0, 0},
          position + sf::Vector2f(
                         0,
                         moveButtonsHeight_ + static_cast<float>(capacity_) *
                                                  (itemSize_.y + itemMargin_)
                     ),
          buttonColorsList,
          [this]() { this->scrollDown(); }
      ) {
}

void Scrollbar::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    upButton.update(window, event);
    downButton.update(window, event);
    upButton.drawInWindow(window);
    downButton.drawInWindow(window);
    for (std::size_t itemNum = offset;
         itemNum < std::min(items.size(), offset + capacity); itemNum++) {
        window.draw(outline[itemNum]);
        items[itemNum]->drawInWindow(window);
        items[itemNum]->update(window, event);
    }
}

void Scrollbar::addItem(interface::ButtonWithImage &item) {
    sf::Vector2f squarePos = {
        position.x,
        position.y +
            static_cast<float>(items.size()) * (itemSize.y + itemMargin) +
            moveButtonsHeight};
    sf::Vector2f itemPos =
        squarePos + sf::Vector2f(edgeMargin.x, itemMargin / 2);
    item.setPosition(itemPos);
    items.push_back(std::make_unique<ButtonWithImage>(item));
    outline.emplace_back(
        sf::Vector2f(itemSize.x + 2 * edgeMargin.x, itemSize.x + itemMargin)
    );
    outline[outline.size() - 1].setPosition(squarePos);
    outline[outline.size() - 1].setFillColor(sf::Color::Transparent);
    outline[outline.size() - 1].setOutlineThickness(1.f);
    outline[outline.size() - 1].setOutlineColor(sf::Color::White);
}

void Scrollbar::scrollUp() {
    if (offset == 0) {
        return;
    }
    offset--;
    sf::Vector2f moveVector(0, itemSize.y + itemMargin);
    for (int buttonNum = 0; buttonNum < items.size(); buttonNum++) {
        items[buttonNum]->setPosition(
            items[buttonNum]->getPosition() + moveVector
        );
        sf::Vector2f oldOutlinePos = outline[buttonNum].getPosition();
        outline[buttonNum].setPosition(
            oldOutlinePos.x + moveVector.x, oldOutlinePos.y + moveVector.y
        );
    }
}

void Scrollbar::scrollDown() {
    if (offset + capacity == items.size()) {
        return;
    }
    offset++;
    sf::Vector2f moveVector(0, itemSize.y + itemMargin);
    for (int buttonNum = 0; buttonNum < items.size(); buttonNum++) {
        items[buttonNum]->setPosition(
            items[buttonNum]->getPosition() - moveVector
        );
        sf::Vector2f oldOutlinePos = outline[buttonNum].getPosition();
        outline[buttonNum].setPosition(
            oldOutlinePos.x - moveVector.x, oldOutlinePos.y - moveVector.y
        );
    }
}

void Scrollbar::chooseItem(std::size_t item) {
    outline[itemChosen].setFillColor(sf::Color::Transparent);
    itemChosen = item;
    outline[item].setFillColor(itemChosenColor);
}
}  // namespace interface
