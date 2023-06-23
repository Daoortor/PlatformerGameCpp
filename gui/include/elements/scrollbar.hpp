#ifndef PLATFORMERGAMECPP_SCROLLBAR_HPP
#define PLATFORMERGAMECPP_SCROLLBAR_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "elements/button.hpp"

namespace interface {

template <typename ItemType>
class Scrollbar {
protected:
    sf::Vector2f position;
    std::vector<std::unique_ptr<ItemType>> items;
    float moveButtonsHeight;
    std::size_t capacity;
    std::size_t offset = 0;
    ButtonWithImage upButton;
    ButtonWithImage downButton;
    sf::Vector2f itemSize;
    sf::Vector2f edgeMargin;
    float itemMargin;

public:
    Scrollbar(
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
              position +
                  sf::Vector2f(
                      0,
                      moveButtonsHeight_ + static_cast<float>(capacity_) *
                                               (itemSize_.y + itemMargin_)
                  ),
              buttonColorsList,
              [this]() { this->scrollDown(); },
              false,
              0,
              itemChosenColor_
          ){};

    void loadInWindow(sf::RenderWindow &window, sf::Event event) {
        upButton.update(window, event);
        downButton.update(window, event);
        upButton.drawInWindow(window);
        downButton.drawInWindow(window);
        for (std::size_t itemNum = offset;
             itemNum < std::min(items.size(), offset + capacity); itemNum++) {
            items[itemNum]->drawInWindow(window);
            items[itemNum]->update(window, event);
        }
    };

    virtual void addItem(ItemType &item) {
        sf::Vector2f squarePos = {
            position.x,
            position.y +
                static_cast<float>(items.size()) * (itemSize.y + itemMargin) +
                moveButtonsHeight};
        sf::Vector2f itemPos =
            squarePos + sf::Vector2f(edgeMargin.x, itemMargin / 2);
        item.setPosition(itemPos);
        items.push_back(std::make_unique<ItemType>(item));
    };

    // TODO: unfortunately does not take derived classes

    virtual void scrollUp() {
        if (offset == 0) {
            return;
        }
        offset--;
        sf::Vector2f moveVector(0, itemSize.y + itemMargin);
        for (const auto &item : items) {
            item->setPosition(item->getPosition() + moveVector);
        }
    };

    virtual void scrollDown() {
        if (offset + capacity == items.size()) {
            return;
        }
        offset++;
        sf::Vector2f moveVector(0, itemSize.y + itemMargin);
        for (const auto &item : items) {
            item->setPosition(item->getPosition() - moveVector);
        }
    };

    virtual void reset() {
        sf::Vector2f moveVector(0, (itemSize.y + itemMargin) * offset);
        for (const auto &item : items) {
            item->setPosition(item->getPosition() + moveVector);
        }
        offset = 0;
    }

    virtual void clear() {
        offset = 0;
        items.clear();
    }
};

class ImageButtonScrollbar : public Scrollbar<ButtonWithImage> {
protected:
    int itemChosen = -1;

public:
    ImageButtonScrollbar(
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
        : Scrollbar<ButtonWithImage>(
              position_,
              itemSize_,
              edgeMargin_,
              itemMargin_,
              moveButtonsHeight_,
              capacity_,
              buttonColorsList,
              itemChosenColor_,
              iconFilepath
          ){};
    void chooseItem(int item);
    void deactivate();
};
}  // namespace interface

#endif  // PLATFORMERGAMECPP_SCROLLBAR_HPP
