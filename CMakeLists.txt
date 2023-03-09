cmake_minimum_required(VERSION 3.23)
project(ProjectGame)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(ProjectGame
        main.cpp
        button.cpp
        )

target_link_libraries(ProjectGame sfml-graphics)
