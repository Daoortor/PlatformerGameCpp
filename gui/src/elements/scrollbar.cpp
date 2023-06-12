#include "elements/scrollbar.hpp"

namespace interface {
void ImageButtonScrollbar::chooseItem(int item) {
    if (itemChosen != -1) {
        items[itemChosen]->deactivate();
    }
    itemChosen = item;
    items[item]->activate();
}

void ImageButtonScrollbar::deactivate() {
    if (itemChosen != -1) {
        items[itemChosen]->deactivate();
        itemChosen = -1;
    }
}
}  // namespace interface
