#include "scrollbar.hpp"
#include "gui-constants.hpp"

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
      upButton(
          iconFilepath + "up-arrow.png",
          sf::RectangleShape(
              {itemSize_.x + 2 * edgeMargin_.x, moveButtonsHeight_}
          ),
          {0, 0},
          position,
          buttonColorsList,
          [this]() { this->scrollUp(); },
          false,
          0,
          itemChosenColor_
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
          [this]() { this->scrollDown(); },
          false,
          0,
          itemChosenColor_
      ) {
}

void Scrollbar::loadInWindow(sf::RenderWindow &window, sf::Event event) {
    upButton.update(window, event);
    downButton.update(window, event);
    upButton.drawInWindow(window);
    downButton.drawInWindow(window);
    for (std::size_t itemNum = offset;
         itemNum < std::min(items.size(), offset + capacity); itemNum++) {
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
}

void Scrollbar::scrollUp() {
    if (offset == 0) {
        return;
    }
    offset--;
    sf::Vector2f moveVector(0, itemSize.y + itemMargin);
    for (const auto &item : items) {
        item->setPosition(item->getPosition() + moveVector);
    }
}

void Scrollbar::scrollDown() {
    if (offset + capacity == items.size()) {
        return;
    }
    offset++;
    sf::Vector2f moveVector(0, itemSize.y + itemMargin);
    for (const auto &item : items) {
        item->setPosition(item->getPosition() - moveVector);
    }
}

void Scrollbar::chooseItem(std::size_t item) {
    items[itemChosen]->deactivate();
    itemChosen = item;
    items[item]->activate();
}

void Scrollbar::deactivate() {
    items[itemChosen]->deactivate();
}
}  // namespace interface
