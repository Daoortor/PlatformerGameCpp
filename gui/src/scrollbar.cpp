#include "../include/scrollbar.hpp"
#include <iostream>
#include "../include/gui-constants.hpp"

namespace interface {
void moveDrawing(std::vector<sf::VertexArray> &drawing, sf::Vector2f delta) {
    for (auto &line : drawing) {
        for (int vertexNum = 0; vertexNum < line.getVertexCount();
             vertexNum++) {
            line[vertexNum] = sf::Vertex(line[vertexNum].position + delta);
        }
    }
}

Scrollbar::Scrollbar(
    sf::Vector2f position_,
    sf::Vector2f itemSize_,
    sf::Vector2f edgeMargin_,
    float itemMargin_,
    float moveButtonsHeight_,
    std::size_t capacity_,
    const std::vector<sf::Color> &buttonColorsList,
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
    for (std::size_t itemNum = offset;
         itemNum < std::min(items.size(), offset + capacity); itemNum++) {
        items[itemNum]->drawInWindow(window);
        for (const auto &line : outline[itemNum]) {
            window.draw(line);
        }
    }
    upButton.update(window, event);
    downButton.update(window, event);
    upButton.drawInWindow(window);
    downButton.drawInWindow(window);
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
    outline.emplace_back();
    auto &newOutline = outline[outline.size() - 1];
    newOutline.emplace_back(sf::LineStrip, 2);
    newOutline[newOutline.size() - 1][0] = squarePos;
    newOutline[newOutline.size() - 1][1] =
        squarePos + sf::Vector2f(itemSize.x + 2 * edgeMargin.x, 0);
    newOutline.emplace_back(sf::LineStrip, 2);
    newOutline[newOutline.size() - 1][0] = squarePos;
    newOutline[newOutline.size() - 1][1] =
        squarePos + sf::Vector2f(0, itemSize.y + itemMargin);
    newOutline.emplace_back(sf::LineStrip, 2);
    newOutline[newOutline.size() - 1][0] =
        squarePos + sf::Vector2f(itemSize.x + 2 * edgeMargin.x, 0);
    newOutline[newOutline.size() - 1][1] =
        squarePos +
        sf::Vector2f(itemSize.x + 2 * edgeMargin.x, itemSize.y + itemMargin);
    newOutline.emplace_back(sf::LineStrip, 2);
    newOutline[newOutline.size() - 1][0] =
        squarePos + sf::Vector2f(0, itemSize.y + itemMargin);
    newOutline[newOutline.size() - 1][1] =
        squarePos +
        sf::Vector2f(itemSize.x + 2 * edgeMargin.x, itemSize.y + itemMargin);
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
        interface::moveDrawing(outline[buttonNum], moveVector);
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
        interface::moveDrawing(outline[buttonNum], -moveVector);
    }
}
}  // namespace interface
