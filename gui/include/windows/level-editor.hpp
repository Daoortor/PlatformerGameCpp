#ifndef PLATFORMERGAMECPP_LEVEL_EDITOR_HPP_
#define PLATFORMERGAMECPP_LEVEL_EDITOR_HPP_

#include "level-window.hpp"

namespace Platformer::gui {
enum class EditorState { Idle, BlockChosen, StartPosChosen, EndPosChosen };

class LevelEditor : public LevelWindow {
    control::LevelPerformer *levelPerformerPtr;
    control::MenuPerformer *menuPerformerPtr;
    std::unique_ptr<Game> game;
    std::unique_ptr<Game> gameFullCopy;
    interface::ImageButtonScrollbar blockSelectionBar;
    sf::RectangleShape levelBorder;
    std::string blockChosen;
    EditorState state;
    interface::Textbox levelNameTextbox;
    interface::RectangleButton saveButton;
    interface::RectangleButton loadButton;
    interface::ButtonWithImage startPosButton;
    interface::ButtonWithImage levelEndButton;
    interface::RectangleButton backToMenuButton;
    interface::RectangleButton resetButton;
    interface::RectangleButton plusButton;
    interface::RectangleButton minusButton;

public:
    LevelEditor(
        unsigned int windowHeight,
        control::LevelPerformer *levelPerformerPtr_,
        control::MenuPerformer *menuPerformerPtr_,
        const std::string &backgroundTextureFilepath,
        const std::string &blockFilepath,
        const std::string &miscFilepath,
        const std::string &emptyLevelFilepath,
        const std::string &levelsFilepath,
        const sf::Font &font,
        sf::Vector2f offset
    );
    void loadInWindow(sf::RenderWindow &window, sf::Event event);
    void setBlock(sf::Vector2<std::size_t> pos, const std::string &name);
    void setStartPos(sf::Vector2i pos);
    void setEndPos(sf::Vector2i pos);
    void deactivateAll();
};
}  // namespace Platformer::gui

#endif  // PLATFORMERGAMECPP_LEVEL_EDITOR_HPP_
