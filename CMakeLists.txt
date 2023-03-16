project(PlatformerGameCpp)

SET(CMAKE_CXX_STANDARD 17)

add_executable(test
        model/testing.cpp
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        model/src/utilities.cpp)
