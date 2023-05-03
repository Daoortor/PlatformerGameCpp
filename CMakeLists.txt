cmake_minimum_required(VERSION 3.23)
project(PlatformerGameCpp)

SET(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

include_directories(gui/assets model/levels model/include gui/include)

add_executable(game
        gui/main.cpp
        gui/src/level-render.cpp
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        tools/utilities.cpp
        gui/src/button.cpp
        gui/src/menu.cpp
        gui/src/performer.cpp
        model/src/board.cpp gui/src/scrollbar.cpp gui/src/textbox.cpp)

target_link_libraries(game sfml-graphics)
