cmake_minimum_required(VERSION 3.23)
project(PlatformerGameCpp)

SET(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

file(COPY gui/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY model/levels DESTINATION ${CMAKE_BINARY_DIR})
file(COPY model/include DESTINATION ${CMAKE_BINARY_DIR})
file(COPY gui/include DESTINATION ${CMAKE_BINARY_DIR})

add_executable(game
        gui/src/level-render.cpp
        gui/level-render-main.cpp
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        model/src/utilities.cpp
        model/src/board.cpp)

target_link_libraries(game sfml-graphics)
