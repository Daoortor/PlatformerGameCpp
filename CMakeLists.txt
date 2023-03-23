cmake_minimum_required(VERSION 3.23)
project(PlatformerGameCpp)

SET(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(PlatformerGameCpp
        model/testing.cpp
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        model/src/utilities.cpp
        model/src/button.cpp
        model/src/menu.cpp
        )

target_link_libraries(PlatformerGameCpp sfml-graphics)