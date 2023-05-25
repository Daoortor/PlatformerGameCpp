#include "scrollbar.hpp"
#include "gui-constants.hpp"

namespace interface {
void ImageButtonScrollbar::chooseItem(std::size_t item) {
    items[itemChosen]->deactivate();
    itemChosen = item;
    items[item]->activate();
}

void ImageButtonScrollbar::deactivate() {
    items[itemChosen]->deactivate();
}
}  // namespace interface
